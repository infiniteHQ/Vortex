//
//  vortex.cpp
//  Sources of main context features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../include/vortex.h"
#ifndef VORTEX_DISABLE
#include "../include/vortex_internals.h"

// Current runtime pointer.
#ifndef CVortexMaker
VxContext *CVortexMaker = NULL;
#endif

#ifndef DISABLE_DEFAULT_ALLOCATORS

static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(user_data);  // Unused parameter
  return malloc(size);   // Call standard malloc
}

static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(user_data);  // Unused parameter
  free(ptr);             // Call standard free
}

#else  // DISABLE_DEFAULT_ALLOCATORS

static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(size);       // Unused parameter
  VX_UNUSED(user_data);  // Unused parameter
  VX_ASSERT(0);          // Trigger assertion, should not be called
  return nullptr;        // Return nullptr
}

static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(ptr);        // Unused parameter
  VX_UNUSED(user_data);  // Unused parameter
  VX_ASSERT(0);          // Trigger assertion, should not be called
}

#endif  // DISABLE_DEFAULT_ALLOCATORS

static VortexMakerMemAllocFunc CVxAllocatorAllocFunc = MallocWrapper;
static VortexMakerMemFreeFunc CVxAllocatorFreeFunc = FreeWrapper;
static void *CVxAllocatorUserData = NULL;

VORTEX_API VxContext *vxe::create_context() {
  VxContext *prev_ctx = get_current_context();

  VxContext *ctx = VX_NEW(VxContext);
  set_current_context(ctx);
  initialize();

  if (prev_ctx != nullptr)
    set_current_context(prev_ctx);

  return ctx;
}

void vxe::set_current_context(VxContext *ctx) {
#ifdef USE_CURRENT_CONTEXT_FUNC
  USE_CURRENT_CONTEXT_FUNC(ctx);
#else
  CVortexMaker = ctx;
#endif
}

VORTEX_API void vxe::destroy_context(VxContext *ctx) {
  // Save the previous context before destroying the specified context
  VxContext *prev_ctx = get_current_context();

  // If no context is provided, destroy the current context
  if (ctx == nullptr)
    ctx = prev_ctx;

  // Set the current context to the one to be destroyed
  set_current_context(ctx);

  // If the previous context is different from the one to be destroyed, restore
  // it
  set_current_context((prev_ctx != ctx) ? prev_ctx : nullptr);

  // Deallocate memory for the context
  VX_DELETE(ctx);
}

VORTEX_API void vxe::initialize() {
  // Get the reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Set initialized flags
  ctx.initialized = true;
}

/**
 * @brief Set custom allocator functions and user data for VortexMaker.
 *
 * This function allows setting custom allocator functions and user data
 * for VortexMaker. These custom functions and data will be used by VortexMaker
 * for memory allocation and deallocation.
 *
 * @param alloc_func The custom allocator function pointer.
 * @param free_func The custom free function pointer.
 * @param user_data The custom user data pointer.
 */
void vxe::SetAllocatorFunctions(VortexMakerMemAllocFunc alloc_func, VortexMakerMemFreeFunc free_func, void *user_data) {
  // Set the custom allocator functions and user data for VortexMaker
  CVxAllocatorAllocFunc = alloc_func;
  CVxAllocatorFreeFunc = free_func;
  CVxAllocatorUserData = user_data;
}

/**
 * @brief Retrieve allocator functions and user data.
 *
 * This function is provided to facilitate copying allocators from one
 * static/DLL boundary to another. It retrieves the allocator functions and user
 * data used by the VortexMaker.
 *
 * @param p_alloc_func Pointer to store the allocator function.
 * @param p_free_func Pointer to store the free function.
 * @param p_user_data Pointer to store the user data.
 */
void vxe::GetAllocatorFunctions(
    VortexMakerMemAllocFunc *p_alloc_func,
    VortexMakerMemFreeFunc *p_free_func,
    void **p_user_data) {
  // Retrieve the allocator functions and user data from the VortexMaker
  *p_alloc_func = CVxAllocatorAllocFunc;
  *p_free_func = CVxAllocatorFreeFunc;
  *p_user_data = CVxAllocatorUserData;
}

/**
 * @brief Get the current Vortex context.
 *
 * This function returns a pointer to the current Vortex context.
 * It simply returns the value of the CVortexMaker pointer.
 *
 * @return A pointer to the current Vortex context.
 */
VORTEX_API VxContext *vxe::get_current_context() {
  return CVortexMaker;  // Return the current Vortex context pointer
}

/**
 * @brief Allocate memory using the Vortex memory allocator.
 *
 * This function allocates memory of the specified size using the Vortex memory
 * allocator. It calls the CVxAllocatorAllocFunc function pointer with the
 * provided size and allocator user data. If IMGUI debug tools are enabled, it
 * also calls DebugAllocHook to handle debug allocation information.
 *
 * @param size The size of memory to allocate.
 * @return A pointer to the allocated memory, or nullptr if allocation fails.
 */
void *vxe::MemAlloc(size_t size) {
  // Call the Vortex memory allocator function pointer to allocate memory
  void *ptr = (*CVxAllocatorAllocFunc)(size, CVxAllocatorUserData);

#ifndef VX_DISABLE_DEBUG_TOOLS
  // Check if IMGUI debug tools are enabled and CVortexMaker is valid
  if (VxContext *ctx = CVortexMaker) {
    // Call the debug allocation hook to handle debug information
    DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
  }
#endif

  return ptr;  // Return the allocated memory pointer
}

// Vx_FREE() == vxe::MemFree()
void vxe::MemFree(void *ptr) {
  if (ptr)
    if (VxContext *ctx = CVortexMaker)
      ctx->IO.MetricsActiveAllocations--;
  return (*CVxAllocatorFreeFunc)(ptr, CVxAllocatorUserData);
}

// We record the number of allocation in recent frames, as a way to
// audit/sanitize our guiding principles of "no allocations on idle/repeating
// frames"
void vxe::DebugAllocHook(VortexMakerDebugAllocInfo *info, void *ptr, size_t size) {
  // VortexMakerDebugAllocEntry* entry =
  // &info->LastEntriesBuf[info->LastEntriesIdx];
  VX_UNUSED(ptr);

  /*
  if (entry->FrameCount != frame_count)
  {
      info->LastEntriesIdx = (info->LastEntriesIdx + 1) %
  IM_ARRAYSIZE(info->LastEntriesBuf); entry =
  &info->LastEntriesBuf[info->LastEntriesIdx]; entry->FrameCount = frame_count;
      entry->AllocCount = entry->FreeCount = 0;
  }
  if (size != (size_t)-1)
  {
      entry->AllocCount++;
      info->TotalAllocCount++;
      //printf("[%05d] MemAlloc(%d) -> 0x%p\n", frame_count, size, ptr);
  }
  else
  {
      entry->FreeCount++;
      info->TotalFreeCount++;
      //printf("[%05d] MemFree(0x%p)\n", frame_count, ptr);
  }*/
}

//-----------------------------------------------------------------------------
// [SECTION] ImGuiTextBuffer, ImGuiTextIndex
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to
// 2013 doesn't have it.
#ifndef va_copy
#if defined(__GNUC__) || defined(__clang__)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#else
#define va_copy(dest, src) (dest = src)
#endif
#endif

char hString::EmptyString[1] = { 0 };

/**
 * @brief Append a string to the hString buffer.
 *
 * This function appends the provided string to the hString buffer.
 * It calculates the length of the string based on the provided end pointer
 * (str_end) or using strlen if str_end is null. If the buffer capacity is not
 * enough to hold the new string, it resizes the buffer to accommodate the new
 * string. The string is then copied into the buffer, and a zero-terminator is
 * added.
 *
 * @param str Pointer to the beginning of the string to append.
 * @param str_end Pointer to the end of the string to append (optional).
 */
void hString::append(const char *str, const char *str_end) {
  // Calculate the length of the string
  int len = str_end ? (int)(str_end - str) : (int)strlen(str);

  // Calculate the write offset and needed size
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;

  // Check if the buffer capacity is enough, resize if needed
  if (needed_sz >= Buf.Capacity) {
    // Double the capacity or use the needed size, whichever is greater
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  // Resize the buffer to accommodate the new string
  Buf.resize(needed_sz);

  // Copy the string into the buffer
  memcpy(&Buf[write_off - 1], str, (size_t)len);

  // Add zero-terminator
  Buf[write_off - 1 + len] = 0;
}

/**
 * @brief Check the version string and data layout for debugging purposes.
 *
 * This function compares the provided version string with the defined
 * VORTEX_VERSION. If the strings do not match, it sets an error flag and
 * asserts.
 *
 * @param version The version string to check.
 * @return True if the version string matches, false otherwise.
 */
bool vxe::DebugCheckVersionAndDataLayout(const char *version) {
  bool error = false;

  // Check if the provided version string matches the defined VORTEX_VERSION
  if (strcmp(version, VORTEX_VERSION) != 0) {
    error = true;
    // Assertion for debugging purposes, will only trigger in debug builds
    VX_ASSERT(strcmp(version, VORTEX_VERSION) == 0 && "Mismatched version string!");
  }

  return !error;  // Return true if no error occurred
}

VORTEX_API void vxe::InstallModuleToSystem(const std::string &path) {
  std::string modules_path = "~/.vx/modules";
  std::string json_file = path + "/module.json";

  try {
    auto json_data = vxe::dump_json(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    modules_path += "/" + name + "." + version;

    vxe::log_info("Core", "Installing the module " + name + "...");

    {
      std::string cmd = "mkdir " + modules_path;
      system(cmd.c_str());
    }

    {
      std::string cmd = "cp -r " + path + "/* " + modules_path;
      system(cmd.c_str());
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

VORTEX_API void vxe::AddModuleToProject(const std::string &module_name) {
}

VORTEX_API void vxe::InstallPluginToSystem(const std::string &path) {
  std::string plugins_path = "~/.vx/plugins";
  std::string json_file = path + "/plugin.json";

  try {
    auto json_data = vxe::dump_json(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    plugins_path += "/" + name + "." + version;

    vxe::log_info("Core", "Installing the plugin " + name + "...");

    {
      std::string cmd = "mkdir " + plugins_path;
      system(cmd.c_str());
    }

    {
      std::string cmd = "cp -r " + path + "/* " + plugins_path;
      system(cmd.c_str());
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

VORTEX_API void vxe::AddPluginToProject(const std::string &plugin_name) {
}

VORTEX_API std::string vxe::replacePlaceholders(
    const std::string &command,
    const std::unordered_map<std::string, std::string> &replacements) {
  std::string result = command;
  for (const auto &[placeholder, value] : replacements) {
    size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
      result.replace(pos, placeholder.length(), value);
      pos += value.length();
    }
  }
  return result;
}

VORTEX_API std::string vxe::getHomeDirectory() {
  if (vxe::is_linux() || vxe::is_macos()) {
    const char *homePath = std::getenv("HOME");
    if (homePath == nullptr) {
      throw std::runtime_error("HOME environment variable not set");
    }
    return std::string(homePath);
  } else if (vxe::is_windows()) {
    const char *homePath = std::getenv("USERPROFILE");
    if (homePath == nullptr) {
      const char *homeDrive = std::getenv("HOMEDRIVE");
      const char *homePathEnv = std::getenv("HOMEPATH");
      if (homeDrive == nullptr || homePathEnv == nullptr) {
        throw std::runtime_error("HOMEPATH environment variables not set");
      }
      return std::string(homeDrive) + std::string(homePathEnv);
    }
    return std::string(homePath);
  }

  throw std::runtime_error("Unknown platform: Unable to determine home directory");
}

VORTEX_API void vxe::InstallContentOnSystem(const std::string &directory) {
  fs::path dir_path(directory);

  if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
    vxe::log_error("Core", "So such file or directory.");
    return;
  }

  bool module_found = fs::exists(dir_path / "module.json");
  bool template_found = fs::exists(dir_path / "template.json");

  if (module_found) {
    vxe::InstallModuleToSystem(directory);
  } else if (template_found) {
    vxe::InstallTemplateOnSystem(directory);
  }
}

VORTEX_API void vxe::AddContentBrowserItem(const std::shared_ptr<ContenBrowserItem> &item) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  ctx.IO.contentbrowser_items.push_back(item);
}

VORTEX_API void vxe::AddGeneralUtility(const std::shared_ptr<ModuleInterfaceUtility> &utility) {
}

VORTEX_API void vxe::PostCustomFolderToJson() {
  VxContext &ctx = *CVortexMaker;
  std::string path = ctx.projectPath.string() + "/.vx/configs/content_browser/";
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/customized_folders.json";

  nlohmann::json json_data;

  json_data["custom_folders"] = nlohmann::json::array();

  for (auto folder : ctx.IO.contentbrowser_customfolders) {
    nlohmann::json folder_data;
    folder_data["color"] = folder->m_Color;
    folder_data["isFav"] = folder->m_IsFav;
    folder_data["path"] = folder->path;

    json_data["custom_folders"].push_back(folder_data);
  }

  vxe::populate_json(json_data, file_path);
}

VORTEX_API void vxe::PublishPool(const std::string &absolute_pool_path, const std::string &name) {
  VxContext &ctx = *CVortexMaker;

  std::size_t endPos = absolute_pool_path.size() - 1;

  if (absolute_pool_path[endPos] == '/' || absolute_pool_path[endPos] == '\\') {
    endPos--;
  }

  for (auto pool : ctx.IO.contentbrowser_pools) {
    if (pool.first == absolute_pool_path) {
      return;
    }
  }

  ctx.IO.contentbrowser_pools.push_back({ absolute_pool_path, name });

  vxe::PostPoolsToJson();
}

VORTEX_API void
vxe::PublishContentBrowserCustomFolder(const std::string &path, const std::string &hex_color, const bool &isFav) {
  VxContext &ctx = *CVortexMaker;

  for (auto folder : ctx.IO.contentbrowser_customfolders) {
    if (folder->path == path) {
      folder->m_Color = hex_color;
      folder->m_IsFav = isFav;
      folder->path = path;
      vxe::PostCustomFolderToJson();
      return;
    }
  }

  std::shared_ptr<ContentBrowserCustomFolder> new_folder = std::make_shared<ContentBrowserCustomFolder>();
  new_folder->m_Color = hex_color;
  new_folder->m_IsFav = isFav;
  new_folder->path = path;

  ctx.IO.contentbrowser_customfolders.push_back(new_folder);

  vxe::PostCustomFolderToJson();
}

VORTEX_API bool vxe::ContentBrowserFolderIsFav(const std::string &path) {
  VxContext &ctx = *CVortexMaker;

  for (auto customized_folder : ctx.IO.contentbrowser_customfolders) {
    if (customized_folder->path == path) {
      return customized_folder->m_IsFav;
    }
  }
  return false;
}

VORTEX_API bool vxe::GetContentBrowserFolderColor(const std::string &path, ImU32 *color) {
  VxContext &ctx = *CVortexMaker;

  for (auto customized_folder : ctx.IO.contentbrowser_customfolders) {
    if (customized_folder->path == path) {
      *color = HexToImU32(customized_folder->m_Color);
      return true;
    }
  }
  return false;
}

VORTEX_API ImU32 vxe::HexToImU32(const std::string &hex) {
  if (hex[0] != '#' || hex.length() != 7) {
    throw std::invalid_argument("Invalid hex format");
  }

  unsigned int r, g, b;
  std::stringstream ss;
  ss << std::hex << hex.substr(1, 2);
  ss >> r;
  ss.clear();
  ss << std::hex << hex.substr(3, 2);
  ss >> g;
  ss.clear();
  ss << std::hex << hex.substr(5, 2);
  ss >> b;

  return IM_COL32(r, g, b, 255);  // Alpha is set to 255 (opaque)
}

VORTEX_API std::string vxe::ImU32ToHex(ImU32 color) {
  unsigned char r = (color >> IM_COL32_R_SHIFT) & 0xFF;
  unsigned char g = (color >> IM_COL32_G_SHIFT) & 0xFF;
  unsigned char b = (color >> IM_COL32_B_SHIFT) & 0xFF;

  std::stringstream ss;
  ss << "#" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(r) << std::setfill('0') << std::setw(2)
     << std::hex << static_cast<int>(g) << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(b);

  return ss.str();
}

VORTEX_API void vxe::FetchPools() {
  VxContext &ctx = *CVortexMaker;
  std::string path = ctx.projectPath.string() + "/.vx/configs/content_browser/";
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/pools.json";

  nlohmann::json json_data;
  json_data["main_pool"] = ctx.projectPath.string();
  json_data["pools"] = nlohmann::json::array();

  vxe::create_file_if_not_exists(file_path, json_data);

  std::ifstream file(file_path);

  file >> json_data;

  // From master pool relative path (from project path)
  ctx.IO.contentbrowser_mainpool = json_data["main_pool"].get<std::string>();

  std::string projectPath = ctx.projectPath.string();
  ctx.IO.contentbrowser_absolute_mainpool = projectPath + "/" + ctx.IO.contentbrowser_mainpool;

  // From other pool absolute paths
  ctx.IO.contentbrowser_pools.clear();
  for (auto directory : json_data["pools"]) {
    std::string path = directory["path"].get<std::string>();
    std::string name = directory["name"].get<std::string>();
    ctx.IO.contentbrowser_pools.push_back({ path, name });
  }
}

VORTEX_API void vxe::PostPoolsToJson() {
  VxContext &ctx = *CVortexMaker;
  std::string path = ctx.projectPath.string() + "/.vx/configs/content_browser/";
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/pools.json";

  nlohmann::json json_data;

  json_data["pools"] = nlohmann::json::array();
  json_data["main_pool"] = ctx.IO.contentbrowser_mainpool;

  for (const auto &pool : ctx.IO.contentbrowser_pools) {
    nlohmann::json folder_data;
    folder_data["name"] = pool.second;
    folder_data["path"] = pool.first;

    json_data["pools"].push_back(folder_data);
  }

  vxe::populate_json(json_data, file_path);
}

VORTEX_API void vxe::OpenURL(const std::string &url) {
#if defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
  std::string command = "open " + url;
  system(command.c_str());
#elif defined(__linux__)
  if (fork() == 0) {
    execlp("xdg-open", "xdg-open", url.c_str(), (char *)nullptr);
    _exit(1);
  }
#else
  std::cerr << "Unsupported platform: unable to open URL." << std::endl;
#endif
}

VORTEX_API void vxe::FetchCustomFolders() {
  VxContext &ctx = *CVortexMaker;
  std::string path = (ctx.projectPath / ".vx/configs/content_browser").string();
  vxe::create_folder_if_not_exists(path);

  std::string file_path = path + "/customized_folders.json";

  nlohmann::json json_data = { { "custom_folders", nlohmann::json::array() }

  };

  vxe::create_file_if_not_exists(file_path, json_data);

  try {
    std::ifstream file(file_path);

    if (file.is_open()) {
      file >> json_data;
      file.close();
    } else {
      throw std::ios_base::failure("Impossible d'ouvrir le fichier JSON");
    }

    ctx.IO.contentbrowser_customfolders.clear();

    for (auto &directory : json_data["custom_folders"]) {
      std::shared_ptr<ContentBrowserCustomFolder> new_folder = std::make_shared<ContentBrowserCustomFolder>();
      new_folder->m_Color = directory["color"].get<std::string>();
      new_folder->m_IsFav = directory["isFav"].get<bool>();
      new_folder->path = directory["path"].get<std::string>();

      vxe::create_folder_if_not_exists(new_folder->path);

      ctx.IO.contentbrowser_customfolders.push_back(new_folder);
    }
  } catch (const std::exception &e) {
    std::cerr << "Erreur lors de la lecture ou du traitement du fichier JSON : " << e.what() << std::endl;
  }
}

VORTEX_API void vxe::Copy(std::vector<std::string> selection, bool in_addition) {
  VxContext &ctx = *CVortexMaker;

  if (!in_addition) {
    ctx.IO.copy_selection.clear();
  }

  for (auto selected : selection) {
    ctx.IO.copy_selection.push_back(selected);
  }
}

VORTEX_API void vxe::SubmitRename(const std::string &oldPathStr, const std::string &newName) {
  fs::path oldPath(oldPathStr);
  fs::path parentDir = oldPath.parent_path();

  fs::path newPath = parentDir / newName;

  if (oldPath == newPath)
    return;

  if (fs::exists(newPath)) {
    std::string baseName = newPath.stem().string();
    std::string extension = newPath.has_extension() ? newPath.extension().string() : "";

    std::regex suffixPattern(R"( \((\d+)\)$)");
    std::smatch match;

    int counter = 1;

    if (std::regex_search(baseName, match, suffixPattern)) {
      counter = std::stoi(match[1].str()) + 1;
      baseName = baseName.substr(0, baseName.size() - match[0].str().size());
    }

    fs::path candidate;
    do {
      candidate = parentDir / (baseName + " (" + std::to_string(counter) + ")" + extension);
      counter++;
    } while (fs::exists(candidate));

    newPath = candidate;
  }

  try {
    fs::rename(oldPath, newPath);
    std::cout << "Renamed : " << oldPath << " -> " << newPath << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error while rename : " << e.what() << "\n";
  }
}

VORTEX_API void vxe::Cut(std::vector<std::string> selection, bool in_addition) {
  VxContext &ctx = *CVortexMaker;

  if (!in_addition) {
    ctx.IO.cut_selection.clear();
  }

  for (auto selected : selection) {
    ctx.IO.cut_selection.push_back(selected);
  }
}

VORTEX_API void vxe::ClearCutSelection() {
  VxContext &ctx = *CVortexMaker;

  ctx.IO.cut_selection.clear();
}

VORTEX_API void vxe::ClearCopySelection() {
  VxContext &ctx = *CVortexMaker;

  ctx.IO.copy_selection.clear();
}

static bool isStrictSubPath(const fs::path &potentialSub, const fs::path &base) {
  auto rel = fs::weakly_canonical(potentialSub).lexically_relative(fs::weakly_canonical(base));
  return !rel.empty() && rel.native()[0] != '.';
}

void CopyDirectoryRecursively(const fs::path &src, const fs::path &dest, const fs::path &destRoot) {
  if (!fs::exists(src) || !fs::is_directory(src)) {
    // err
    return;
  }
  fs::create_directories(dest);
  for (const auto &entry : fs::directory_iterator(src)) {
    const fs::path &from = entry.path();
    fs::path to = dest / from.filename();
    fs::path fromCanonical;
    fs::path destRootCanonical;
    try {
      fromCanonical = fs::canonical(from);
    } catch (...) {
      fromCanonical = fs::weakly_canonical(from);
    }
    try {
      destRootCanonical = fs::canonical(destRoot);
    } catch (...) {
      destRootCanonical = fs::weakly_canonical(destRoot);
    }

    if (isStrictSubPath(fromCanonical, destRootCanonical) || fromCanonical == destRootCanonical) {
      continue;
    }
    try {
      if (fs::is_directory(from)) {
        CopyDirectoryRecursively(from, to, destRoot);
      } else {
        fs::copy_file(from, to, fs::copy_options::overwrite_existing);
      }
    } catch (const fs::filesystem_error &e) {
      // err
    }
  }
}

VORTEX_API void vxe::PasteAllSelections(const std::string &target_path_str) {
  VxContext &ctx = *CVortexMaker;
  fs::path targetPath(target_path_str);

  auto generateNonConflictingPath = [](const fs::path &targetDir, const fs::path &originalName) -> fs::path {
    fs::path newPath = targetDir / originalName;
    if (!fs::exists(newPath))
      return newPath;

    std::string stem = originalName.stem().string();
    std::string extension = originalName.has_extension() ? originalName.extension().string() : "";
    int counter = 1;

    while (true) {
      std::string candidateName = stem + " copy";
      if (counter > 1)
        candidateName += std::to_string(counter);
      fs::path candidatePath = targetDir / (candidateName + extension);
      if (!fs::exists(candidatePath))
        return candidatePath;
      ++counter;
    }
  };

  for (const auto &srcStr : ctx.IO.copy_selection) {
    fs::path srcPath(srcStr);
    if (!fs::exists(srcPath)) {
      std::cerr << "Source path n'existe pas : " << srcStr << "\n";
      continue;
    }

    fs::path destPath = generateNonConflictingPath(targetPath, srcPath.filename());

    // ❌ NE PLUS BLOQUER LA COPIE — la prévention est faite proprement en
    // récursif
    /*
    if (fs::is_directory(srcPath) && isStrictSubPath(destPath, srcPath)) {
      std::cerr << "Copie récursive détectée, opération ignorée : " << srcPath
                << " → " << destPath << "\n";
      continue;
    }
    */

    try {
      if (fs::is_directory(srcPath)) {
        CopyDirectoryRecursively(srcPath, destPath, destPath);
      } else {
        fs::copy_file(srcPath, destPath);
      }

    } catch (const std::exception &e) {
      std::cerr << "Erreur de copie : " << srcPath << " → " << destPath << " : " << e.what() << "\n";
    }
  }

  for (const auto &srcStr : ctx.IO.cut_selection) {
    fs::path srcPath(srcStr);
    if (!fs::exists(srcPath)) {
      std::cerr << "Source path n'existe pas (cut) : " << srcStr << "\n";
      continue;
    }

    fs::path destPath = generateNonConflictingPath(targetPath, srcPath.filename());

    // 🚫 Prévention contre le déplacement récursif
    if (fs::is_directory(srcPath) && isStrictSubPath(destPath, srcPath)) {
      std::cerr << "Déplacement récursif détecté, opération ignorée : " << srcPath << " → " << destPath << "\n";
      continue;
    }

    try {
      fs::rename(srcPath, destPath);
    } catch (const std::exception &e) {
      std::cerr << "Erreur de déplacement : " << srcPath << " → " << destPath << " : " << e.what() << "\n";
    }
  }
}

VORTEX_API void vxe::RenameFolder(const std::string &target_path, const std::string &new_name) {
  namespace fs = std::filesystem;

  try {
    fs::path target(target_path);
    if (!fs::exists(target) || !fs::is_directory(target)) {
      throw std::runtime_error("Target path does not exist or is not a directory.");
    }

    fs::path new_path = target.parent_path() / new_name;
    fs::rename(target, new_path);
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error(std::string("Filesystem error: ") + e.what());
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error: ") + e.what());
  }
}

VORTEX_API void vxe::RefreshProjectThemes() {
  VxContext &ctx = *CVortexMaker;

  std::string home = ctx.projectPath.string();
  std::string themes_path = home + "/.vx/configs/themes/data";
  std::string config_path = home + "/.vx/configs/themes";
  std::string json_file = config_path + "/themes.json";

  vxe::create_folder_if_not_exists(themes_path);
  vxe::create_folder_if_not_exists(config_path);

  nlohmann::json defaultData = { { "used_theme", "dark" }, { "override_themes", nlohmann::json::array() } };
  vxe::create_file_if_not_exists(json_file, defaultData);

  ctx.IO.themes.clear();

  for (const auto &entry : fs::directory_iterator(themes_path)) {
    if (entry.path().extension() == ".json") {
      try {
        std::ifstream file(entry.path());
        nlohmann::json j;
        file >> j;

        if (j.contains("label") && j.contains("name") && j.contains("theme")) {
          auto themeObj = std::make_shared<Theme>();
          themeObj->label = j.value("label", "");
          themeObj->name = j.value("name", "");
          themeObj->description = j.value("description", "");
          themeObj->authors = j.value("authors", "");

          for (auto &[key, value] : j["theme"].items()) {
            themeObj->theme[key] = value;
          }

          ctx.IO.themes.push_back(themeObj);
        }
      } catch (const std::exception &e) {
        vxe::log_error("Core", "Failed to parse theme file: " + entry.path().string());
        vxe::log_error("Core", e.what());
      }
    }
  }

  try {
    std::ifstream configFile(json_file);
    nlohmann::json configJson;
    configFile >> configJson;

    ctx.IO.used_theme = configJson.value("used_theme", "dark");
    ctx.IO.override_themes = configJson.value("override_themes", std::vector<std::string>());
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Failed to load theme config: " + std::string(e.what()));
  }
}

VORTEX_API void UpdateProjectTheme(const std::shared_ptr<Theme> &theme, const std::string &title) {
  std::string themes_path = vxe::getHomeDirectory() + "/.vx/configs/themes/data";
  vxe::create_folder_if_not_exists(themes_path);

  nlohmann::json j;
  j["label"] = theme->label;
  j["name"] = theme->name;
  j["description"] = theme->description;
  j["authors"] = theme->authors;

  nlohmann::json themeValues;

  for (const auto &[key, value] : theme->theme) {
    themeValues[key] = value;
  }

  j["theme"] = themeValues;

  std::string filepath = themes_path + "/" + theme->label + ".json";

  try {
    std::ofstream out(filepath);
    out << std::setw(4) << j << std::endl;
    vxe::log_info("Core", "Theme '" + theme->label + "' updated.");
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Failed to update theme file: " + filepath);
    vxe::log_error("Core", e.what());
  }
}

VORTEX_API std::shared_ptr<Theme> vxe::GetTheme(const std::string &label) {
  VxContext &ctx = *CVortexMaker;
  const auto &themes = ctx.IO.themes;

  for (const auto &theme : themes) {
    if (theme) {
      if (theme->label == label) {
        return theme;
      }
    }
  }
  return nullptr;
}
VORTEX_API std::shared_ptr<Theme> vxe::GetSelectedTheme() {
  VxContext &ctx = *CVortexMaker;
  const std::string &used = ctx.IO.used_theme;
  const auto &themes = ctx.IO.themes;

  for (const auto &theme : themes) {
    if (theme) {
      if (theme->label == used) {
        return theme;
      }
    }
  }

  vxe::log_error("Core", "No theme matched used_theme: '" + used + "'");
  return nullptr;
}

VORTEX_API void vxe::CreateNewTheme(const std::shared_ptr<Theme> &base_theme, const std::string &title) {
  if (!base_theme) {
    vxe::log_error("Theme", "Base theme is null. Cannot create new theme.");
    return;
  }

  std::string themes_path = vxe::get_current_context()->projectPath.string() + "/.vx/configs/themes/data";
  vxe::create_folder_if_not_exists(themes_path);

  std::string base_filename = title;
  std::transform(base_filename.begin(), base_filename.end(), base_filename.begin(), [](unsigned char c) {
    return std::isspace(c) ? '_' : std::tolower(c);
  });

  std::string final_filename = base_filename + ".json";
  std::string final_path = themes_path + "/" + final_filename;
  int counter = 1;
  while (fs::exists(final_path)) {
    final_filename = base_filename + "_" + std::to_string(counter) + ".json";
    final_path = themes_path + "/" + final_filename;
    ++counter;
  }

  nlohmann::json theme_json;
  theme_json["label"] = base_filename;
  theme_json["name"] = title;
  theme_json["description"] = base_theme->description;
  theme_json["authors"] = base_theme->authors;

  nlohmann::json theme_data;
  for (const auto &[key, value] : base_theme->theme) {
    theme_data[key] = value;
  }

  theme_json["theme"] = theme_data;

  try {
    std::ofstream file(final_path);
    file << std::setw(4) << theme_json << std::endl;
    vxe::log_info("Theme", "New theme created: " + final_filename);
  } catch (const std::exception &e) {
    vxe::log_error("Theme", "Failed to write new theme file: " + std::string(e.what()));
  }
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> vxe::GetAllItemHandlersFor(const std::string &type) {
  std::vector<std::shared_ptr<ItemHandlerInterface>> list;

  for (auto mod : vxe::get_current_context()->IO.em) {
    if (mod->m_state == "running") {
      for (auto handlers : mod->GetContentBrowserItemHandler()) {
        if (handlers->type == type)
          list.push_back(handlers);
      }
    }
  }

  for (auto plug : vxe::get_current_context()->IO.ep) {
    if (plug->m_state == "running") {
      for (auto handlers : plug->GetContentBrowserItemHandler()) {
        if (handlers->type == type)
          list.push_back(handlers);
      }
    }
  }

  return list;
}

VORTEX_API void vxe::VerifyAndPouplateThemes() {
  std::string themes_path = vxe::get_current_context()->projectPath.string() + "/.vx/configs/themes/data";

  vxe::create_folder_if_not_exists(themes_path);

  const std::vector<std::string> required_themes = { "dark.json", "clear.json" };

  for (const auto &theme_file : required_themes) {
    std::string path = themes_path + "/" + theme_file;
    bool needs_creation = false;

    if (!fs::exists(path)) {
      vxe::log_info("Theme", theme_file + " is missing. Recreating.");
      needs_creation = true;
    } else {
      try {
        std::ifstream in(path);
        nlohmann::json theme_json;
        in >> theme_json;

        if (!theme_json.contains("label") || !theme_json.contains("name") || !theme_json.contains("description") ||
            !theme_json.contains("authors") || !theme_json.contains("theme")) {
          vxe::log_error("Theme", theme_file + " is invalid. Recreating.");
          needs_creation = true;
        }
      } catch (...) {
        vxe::log_error("Theme", theme_file + " is corrupted. Recreating.");
        needs_creation = true;
      }
    }

    if (needs_creation && theme_file == "dark.json") {
      std::shared_ptr<Theme> dark_theme = std::make_shared<Theme>();
      dark_theme->description = "Default dark theme";
      dark_theme->authors = { "Vortex DevTeam" };

      // clang-format off
      std::map<std::string, std::string> properties = {
          {"color_header", "#2F2F2FFF"},
          {"color_header_hovered", "#2F2F2FFF"},
          {"color_header_active", "#2F2F2FFF"},
          {"color", "#383838C8"},
          {"color_hovered", "#464646FF"},
          {"color_active", "#38383896"},
          {"color_framebg", "#0F0F0FFF"},
          {"color_framebg_hovered", "#0F0F0FFF"},
          {"color_framebg_active", "#0F0F0FFF"},
          {"color_tab", "#151515FF"},
          {"color_tab_hovered", "#24242464"},
          {"color_tab_active", "#242424FF"},
          {"color_tab_unfocused", "#151515FF"},
          {"color_tab_unfocused_active", "#24242464"},
          {"color_text", "#898989FF"},
          {"color_titlebg", "#151515FF"},
          {"color_titlebg_active", "#151515FF"},
          {"color_titlebg_collapsed", "#262626FF"},
          {"color_resizegrip", "#E8E8E840"},
          {"color_resizegrip_hovered", "#CFCFCFAA"},
          {"color_resizegrip_active", "#757575F2"},
          {"color_scrollbar_bg", "#05050587"},
          {"color_scrollbar_grab", "#4F4F4FFF"},
          {"color_scrollbar_grab_hovered", "#696969FF"},
          {"color_scrollbar_grab_active", "#828282FF"},
          {"color_checkmark", "#C8C8C8FF"},
          {"color_slidergrab", "#828282B3"},
          {"color_slidergrab_active", "#A8A8A8FF"},
          {"color_separator", "#151515FF"},
          {"color_separator_active", "#27B9F2FF"},
          {"color_separator_hovered", "#151515FF"},
          {"color_window_bg", "#151515FF"},
          {"color_child_bg", "#242424FF"},
          {"color_popup_bg", "#383838FF"},
          {"color_border", "#242424FF"},
          {"color_table_header_bg", "#2F2F2FFF"},
          {"color_table_border_light", "#1A1A1AFF"},
          {"color_menubar_bg", "#242424FF"},

          // Buttons
          {"button_color_border", "#454545FF"},
          {"button_color_border_hovered", "#555555FF"},
          {"button_color_border_clicked", "#444444FF"},
          {"button_color_border_pressed", "#757575FF"},
          {"button_color_bg", "#242424FF"},
          {"button_color_bg_hovered", "#343434FF"},
          {"button_color_bg_pressed", "#444444FF"},
          {"button_color_bg_clicked", "#444444FF"},
          {"button_color_text", "#BCBCBCFF"},
          {"button_color_text_hovered", "#FFFFFFFF"},
          {"button_color_text_pressed", "#FFFFFFFF"},
          {"button_color_underline", "#242424FF"},
          {"button_color_underline_hovered", "#343434FF"},
          {"button_color_underline_pressed", "#444444FF"},
          {"button_size_x", "0"},
          {"button_size_y", "0"},
          {"button_padding_x", "6"},
          {"button_padding_y", "6"},
          {"button_scale", "0"},

          // Checkbox
          {"checkbox_color_border", "#454545B2"},
          {"checkbox_color_border_hovered", "#454545B2"},
          {"checkbox_color_border_clicked", "#454545B2"},
          {"checkbox_color_border_pressed", "#454545B2"},
          {"checkbox_color_bg", "#242424FF"},
          {"checkbox_color_bg_hovered", "#343434FF"},
          {"checkbox_color_bg_pressed", "#444444FF"},
          {"checkbox_color_bg_clicked", "#444444FF"},
          {"checkbox_size_x", "6"},
          {"checkbox_size_y", "6"},

          // Combo
          {"combo_color_border", "#454545B2"},
          {"combo_color_border_hovered", "#454545B2"},
          {"combo_color_border_clicked", "#454545B2"},
          {"combo_color_border_pressed", "#454545B2"},
          {"combo_color_bg", "#242424FF"},
          {"combo_color_bg_hovered", "#343434FF"},
          {"combo_color_bg_pressed", "#444444FF"},
          {"combo_color_bg_clicked", "#444444FF"},
          {"combo_size_x", "6"},
          {"combo_size_y", "6"},

          // Header
          {"header_color_border", "#454545B2"},
          {"header_color_border_hovered", "#454545B2"},
          {"header_color_border_clicked", "#454545B2"},
          {"header_color_border_pressed", "#454545B2"},
          {"header_color_bg", "#242424FF"},
          {"header_color_bg_hovered", "#343434FF"},
          {"header_color_bg_pressed", "#444444FF"},
          {"header_color_bg_clicked", "#444444FF"},
          {"header_size_x", "6"},
          {"header_size_y", "6"},
          {"header_button_color_border", "#454545FF"},
          {"header_button_color_border_hovered", "#555555FF"},
          {"header_button_color_border_clicked", "#444444FF"},
          {"header_button_color_border_pressed", "#757575FF"},
          {"header_button_color_bg", "#242424FF"},
          {"header_button_color_bg_hovered", "#343434FF"},
          {"header_button_color_bg_pressed", "#444444FF"},
          {"header_button_color_bg_clicked", "#444444FF"},
          {"header_button_color_text", "#BCBCBCFF"},
          {"header_button_color_text_hovered", "#FFFFFFFF"},
          {"header_button_color_text_pressed", "#FFFFFFFF"},

          // Image
          {"image_color_border", "#454545B2"},
          {"image_color_border_hovered", "#454545B2"},
          {"image_color_border_clicked", "#454545B2"},
          {"image_color_border_pressed", "#454545B2"},
          {"image_color_bg", "#242424FF"},
          {"image_color_bg_hovered", "#343434FF"},
          {"image_color_bg_pressed", "#444444FF"},
          {"image_color_bg_clicked", "#444444FF"},
          {"image_size_x", "6"},
          {"image_size_y", "6"},

          // Modal
          {"modal_padding_x", "6"},
          {"modal_padding_y", "6"},

          // Notification
          {"notification_color_border", "#454545FF"},
          {"notification_color_border_hovered", "#555555FF"},
          {"notification_color_bg", "#242424FF"},
          {"notification_color_bg_hovered", "#343434FF"},

          // Separator
          {"separator_color", "#424242"},
          {"separator_color_text", "#454545B2"},

          // Keyval
          {"keyval_color_text", "#454545B2"},

          // Table
          {"table_color_border", "#232323"},
          {"table_cell_padding_x_header", "6.0"},
          {"table_cell_padding_y_header", "6.0"},
          {"table_cell_padding_x_row", "6.0"},
          {"table_cell_padding_y_row", "6.0"},

          // Text Area
          {"text_area_color_text", "#FFFFFFFF"},

          // Text
          {"text_color_text", "#454545B2"},

          // Titles
          {"title_color_text", "#FFFFFFFF"},

          // Tooltip
          {"tooltip_color_border", "#454545B2"},
          {"tooltip_color_border_hovered", "#454545B2"},
          {"tooltip_color_border_clicked", "#454545B2"},
          {"tooltip_color_bg", "#242424FF"},
          {"tooltip_color_bg_hovered", "#343434FF"},
          {"tooltip_color_bg_clicked", "#444444FF"},

          // Blocks
          {"block_color", "#252525"},
          {"block_color_hovered", "#454545"},
          {"block_color_pressed", "#555555"},
          {"block_border_color", "#353535"},
          {"block_border_color_hovered", "#353535"},
          {"block_border_color_pressed", "#555555"},
          {"block_border_radius", "0.0"},
          {"block_border_size", "1.0"},
      };
      // clang-format on

      for (const auto &[k, v] : properties) {
        dark_theme->theme[k] = v;
      }

      CreateNewTheme(dark_theme, "Dark");
    } else if (needs_creation && theme_file == "clear.json") {
      std::shared_ptr<Theme> light_theme = std::make_shared<Theme>();
      light_theme->description = "Default light theme";
      light_theme->authors = { "Vortex DevTeam" };

      std::map<std::string, std::string> properties = { { "color_header", "#EAEAEA" },
                                                        { "color_header_hovered", "#DCDCDC" },
                                                        { "color_header_active", "#D0D0D0" },
                                                        { "color", "#F5F5F5" },
                                                        { "color_hovered", "#EBEBEB" },
                                                        { "color_active", "#DADADA" },
                                                        { "color_framebg", "#FFFFFF" },
                                                        { "color_framebg_hovered", "#F0F0F0" },
                                                        { "color_framebg_active", "#E0E0E0" },
                                                        { "color_text", "#252525FF" },
                                                        { "color_tab", "#F2F2F2" },
                                                        { "color_tab_hovered", "#E6E6E6" },
                                                        { "color_tab_active", "#DDDDDD" },
                                                        { "color_tab_unfocused", "#F2F2F2" },
                                                        { "color_tab_unfocused_active", "#E6E6E6" },
                                                        { "color_titlebg", "#F0F0F0" },
                                                        { "color_titlebg_active", "#E0E0E0" },
                                                        { "color_titlebg_collapsed", "#FAFAFA" },
                                                        { "color_resizegrip", "#AAAAAA40" },
                                                        { "color_resizegrip_hovered", "#888888AA" },
                                                        { "color_resizegrip_active", "#3D7EFF" },
                                                        { "color_scrollbar_bg", "#EEEEEE" },
                                                        { "color_scrollbar_grab", "#C2C2C2" },
                                                        { "color_scrollbar_grab_hovered", "#A8A8A8" },
                                                        { "color_scrollbar_grab_active", "#8F8F8F" },
                                                        { "color_checkmark", "#3D7EFF" },
                                                        { "color_slidergrab", "#BBBBBB" },
                                                        { "color_slidergrab_active", "#3D7EFF" },
                                                        { "color_separator", "#DDDDDD" },
                                                        { "color_separator_active", "#3D7EFF" },
                                                        { "color_separator_hovered", "#CCCCCC" },
                                                        { "color_window_bg", "#FFFFFF" },
                                                        { "color_child_bg", "#F9F9F9" },
                                                        { "color_popup_bg", "#FFFFFF" },
                                                        { "color_border", "#DDDDDD" },
                                                        { "color_table_header_bg", "#EDEDED" },
                                                        { "color_table_border_light", "#F3F3F3" },
                                                        { "color_menubar_bg", "#FAFAFA" },

                                                        // Buttons
                                                        { "button_color_border", "#CCCCCC" },
                                                        { "button_color_border_hovered", "#BBBBBB" },
                                                        { "button_color_border_clicked", "#AAAAAA" },
                                                        { "button_color_border_pressed", "#3D7EFF" },
                                                        { "button_color_bg", "#F0F0F0" },
                                                        { "button_color_bg_hovered", "#E6E6E6" },
                                                        { "button_color_bg_pressed", "#DADADA" },
                                                        { "button_color_bg_clicked", "#DADADA" },
                                                        { "button_color_text", "#333333" },
                                                        { "button_color_text_hovered", "#000000" },
                                                        { "button_color_text_pressed", "#000000" },
                                                        { "button_color_underline", "#CCCCCC" },
                                                        { "button_color_underline_hovered", "#AAAAAA" },
                                                        { "button_color_underline_pressed", "#3D7EFF" },
                                                        { "button_size_x", "0" },
                                                        { "button_size_y", "0" },
                                                        { "button_padding_x", "6" },
                                                        { "button_padding_y", "6" },
                                                        { "button_scale", "0" },

                                                        // Checkbox
                                                        { "checkbox_color_border", "#CCCCCC" },
                                                        { "checkbox_color_border_hovered", "#AAAAAA" },
                                                        { "checkbox_color_border_clicked", "#888888" },
                                                        { "checkbox_color_border_pressed", "#3D7EFF" },
                                                        { "checkbox_color_bg", "#FFFFFF" },
                                                        { "checkbox_color_bg_hovered", "#F5F5F5" },
                                                        { "checkbox_color_bg_pressed", "#E5E5E5" },
                                                        { "checkbox_color_bg_clicked", "#E5E5E5" },
                                                        { "checkbox_size_x", "6" },
                                                        { "checkbox_size_y", "6" },

                                                        // Tooltip
                                                        { "tooltip_color_border", "#CCCCCC" },
                                                        { "tooltip_color_border_hovered", "#AAAAAA" },
                                                        { "tooltip_color_border_clicked", "#888888" },
                                                        { "tooltip_color_bg", "#FAFAFA" },
                                                        { "tooltip_color_bg_hovered", "#F0F0F0" },
                                                        { "tooltip_color_bg_clicked", "#E0E0E0" },

                                                        // Text Area
                                                        { "text_area_color_text", "#222222" },

                                                        // Text
                                                        { "text_color_text", "#222222" },

                                                        // Titles
                                                        { "title_color_text", "#000000" },

                                                        // Notification
                                                        { "notification_color_border", "#AAAAAA" },
                                                        { "notification_color_border_hovered", "#3D7EFF" },
                                                        { "notification_color_bg", "#FFFFFF" },
                                                        { "notification_color_bg_hovered", "#F7F7F7" },

                                                        // Blocks
                                                        { "block_color", "#F5F5F5" },
                                                        { "block_color_hovered", "#E5E5E5" },
                                                        { "block_color_pressed", "#D5D5D5" },
                                                        { "block_border_color", "#CCCCCC" },
                                                        { "block_border_color_hovered", "#BBBBBB" },
                                                        { "block_border_color_pressed", "#3D7EFF" },
                                                        { "block_border_radius", "0.0" },
                                                        { "block_border_size", "1.0" } };

      for (const auto &[k, v] : properties) {
        light_theme->theme[k] = v;
      }

      CreateNewTheme(light_theme, "Clear");
    }
  }
}

VORTEX_API void vxe::UpdateProjectThemesComfig() {
  VxContext &ctx = *CVortexMaker;
  std::string config_path = vxe::getHomeDirectory() + "/.vx/configs/themes";
  std::string json_file = config_path + "/themes.json";

  nlohmann::json configJson;
  configJson["used_theme"] = ctx.IO.used_theme;
  configJson["override_themes"] = ctx.IO.override_themes;

  try {
    std::ofstream out(json_file);
    out << std::setw(4) << configJson << std::endl;
    vxe::log_info("Core", "Theme configuration updated.");
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Failed to update theme config file: " + std::string(e.what()));
  }
}

VORTEX_API std::string vxe::ConvertPathToWindowsStyle(const std::string &path) {
  std::string windowsPath = path;
  std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
  return windowsPath;
}

VORTEX_API void vxe::ThemeRebuilded() {
  VxContext &ctx = *CVortexMaker;
  ctx.IO.theme_changed = false;
}

VORTEX_API void vxe::RebuildTheme() {
  VxContext &ctx = *CVortexMaker;
  ctx.IO.theme_changed = true;
}

VORTEX_API bool vxe::IsThemeNeedsRebuild() {
  VxContext &ctx = *CVortexMaker;
  return ctx.IO.theme_changed;
}

VORTEX_API std::string vxe::CreateFile(const std::string &path) {
  fs::path basePath(path);
  std::string baseName = "New file";
  std::string extension = ".txt";
  fs::path fullPath = basePath / (baseName + extension);

  int counter = 2;
  while (fs::exists(fullPath)) {
    fullPath = basePath / (baseName + " " + std::to_string(counter) + extension);
    ++counter;
  }

  std::ofstream ofs(fullPath);
  ofs.close();

  return fs::absolute(fullPath).string();
}

VORTEX_API std::string vxe::CreateFolder(const std::string &path) {
  fs::path basePath(path);
  std::string baseName = "New folder";
  fs::path fullPath = basePath / baseName;

  int counter = 2;
  while (fs::exists(fullPath)) {
    fullPath = basePath / (baseName + " " + std::to_string(counter));
    ++counter;
  }

  fs::create_directories(fullPath);

  return fs::absolute(fullPath).string();
}

VORTEX_API void vxe::RenameFile(const std::string &target_path, const std::string &new_name) {
  namespace fs = std::filesystem;

  try {
    fs::path target(target_path);
    if (!fs::exists(target) || !fs::is_regular_file(target)) {
      throw std::runtime_error("Target path does not exist or is not a file.");
    }

    fs::path new_path = target.parent_path() / new_name;
    fs::rename(target, new_path);
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error(std::string("Filesystem error: ") + e.what());
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error: ") + e.what());
  }
}

VORTEX_API void vxe::DeleteFile(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path) || !std::filesystem::is_regular_file(target_path)) {
      throw std::invalid_argument("The specified path does not exist or is not a file.");
    }

    std::filesystem::remove(target_path);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  } catch (const std::exception &e) {
    std::cerr << "General error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while deleting the file." << '\n';
  }
}

VORTEX_API void vxe::DeleteFolder(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path) || !std::filesystem::is_directory(target_path)) {
      throw std::invalid_argument("The specified path does not exist or is not a directory.");
    }

    std::filesystem::remove_all(target_path);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  } catch (const std::exception &e) {
    std::cerr << "General error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while deleting the folder." << '\n';
  }
}

VORTEX_API void vxe::DeletePath(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path)) {
      throw std::invalid_argument("The specified path does not exist.");
    }

    if (std::filesystem::is_directory(target_path)) {
      DeleteFolder(target_path);
    } else if (std::filesystem::is_regular_file(target_path)) {
      DeleteFile(target_path);
    } else {
      throw std::invalid_argument("The specified path is neither a regular file nor a directory.");
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while processing the path." << '\n';
  }
}

VORTEX_API void vxe::AddCredits(const std::string &topic, const std::string &credit_file) {
  auto ctx = vxe::get_current_context();

  std::ifstream file(credit_file);
  if (!file.is_open()) {
    return;
  }

  TopicCredits &tc = ctx->credits[topic];

  std::string line;
  bool in_block = false;
  std::string current_title = "";

  while (std::getline(file, line)) {
    auto trim = [](std::string s) -> std::string {
      size_t start = s.find_first_not_of(" \t\r\n");
      size_t end = s.find_last_not_of(" \t\r\n");
      return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    };
    line = trim(line);

    if (line == "-----BEGIN CREDITS-----") {
      in_block = true;
      continue;
    }
    if (line == "-----END CREDITS-----") {
      break;
    }
    if (!in_block)
      continue;

    if (line.empty())
      continue;

    if (line[0] == '#') {
      std::string title = trim(line.substr(1));
      if (!title.empty()) {
        current_title = title;
        if (tc.sections.find(current_title) == tc.sections.end()) {
          tc.title_order.push_back(current_title);
          tc.sections[current_title] = {};
        }
      }
      continue;
    }

    CreditEntry entry;

    size_t lt = line.find('<');
    size_t gt = line.find('>');
    if (lt != std::string::npos && gt != std::string::npos && gt > lt) {
      entry.contact = line.substr(lt + 1, gt - lt - 1);
      entry.name = trim(line.substr(0, lt));
    } else {
      size_t lp = line.find('(');
      size_t rp = line.find(')');
      if (lp != std::string::npos && rp != std::string::npos && rp > lp) {
        entry.contact = line.substr(lp + 1, rp - lp - 1);
        entry.name = trim(line.substr(0, lp));
      } else {
        entry.name = line;
      }
    }

    if (current_title.empty()) {
      current_title = "";
      if (tc.sections.find("") == tc.sections.end()) {
        tc.title_order.push_back("");
        tc.sections[""] = {};
      }
    }

    tc.sections[current_title].push_back(entry);
  }
}

VORTEX_API void vxe::SetCreditsFile(const std::string &topic, const std::string &credit_file) {
  auto ctx = vxe::get_current_context();

  TopicCredits &tc = ctx->credits[topic];

  tc.sections.clear();
  tc.title_order.clear();

  std::ifstream file(credit_file);
  if (!file.is_open()) {
    return;
  }

  std::string line;
  bool in_block = false;
  std::string current_title = "";

  while (std::getline(file, line)) {
    auto trim = [](std::string s) -> std::string {
      size_t start = s.find_first_not_of(" \t\r\n");
      size_t end = s.find_last_not_of(" \t\r\n");
      return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    };

    line = trim(line);

    if (line == "-----BEGIN CREDITS-----") {
      in_block = true;
      continue;
    }
    if (line == "-----END CREDITS-----") {
      break;
    }
    if (!in_block || line.empty()) {
      continue;
    }

    if (line[0] == '#') {
      std::string title = trim(line.substr(1));
      if (!title.empty()) {
        current_title = title;
        if (tc.sections.find(current_title) == tc.sections.end()) {
          tc.title_order.push_back(current_title);
          tc.sections[current_title] = {};
        }
      }
      continue;
    }

    CreditEntry entry;
    size_t lt = line.find('<');
    size_t gt = line.find('>');
    if (lt != std::string::npos && gt != std::string::npos && gt > lt) {
      entry.contact = line.substr(lt + 1, gt - lt - 1);
      entry.name = trim(line.substr(0, lt));
    } else {
      size_t lp = line.find('(');
      size_t rp = line.find(')');
      if (lp != std::string::npos && rp != std::string::npos && rp > lp) {
        entry.contact = line.substr(lp + 1, rp - lp - 1);
        entry.name = trim(line.substr(0, lp));
      } else {
        entry.name = line;
      }
    }

    if (current_title.empty()) {
      if (tc.sections.find("") == tc.sections.end()) {
        tc.title_order.push_back("");
        tc.sections[""] = {};
      }
    }

    tc.sections[current_title].push_back(entry);
  }
}

VORTEX_API std::vector<std::string> vxe::GetTitlesFromTopic(const std::string &topic) {
  auto ctx = vxe::get_current_context();

  auto it = ctx->credits.find(topic);
  if (it == ctx->credits.end())
    return {};

  return it->second.title_order;
}

VORTEX_API std::vector<std::string> vxe::GetNamesFromTopicAndTitle(const std::string &topic, const std::string &title) {
  auto ctx = vxe::get_current_context();

  auto it = ctx->credits.find(topic);
  if (it == ctx->credits.end())
    return {};

  auto &tc = it->second;
  auto sit = tc.sections.find(title);
  if (sit == tc.sections.end())
    return {};

  std::vector<std::string> results;
  for (const auto &entry : sit->second) {
    std::string line = entry.name;
    if (!entry.contact.empty()) {
      bool is_email = entry.contact.find('@') != std::string::npos;
      if (is_email)
        line += " <" + entry.contact + ">";
      else
        line += " (" + entry.contact + ")";
    }
    results.push_back(line);
  }
  return results;
}

VORTEX_API void vxe::AddDocumentation(
    const std::string &topic,
    const std::string &section,
    const std::string &title,
    const std::string &md_file_path) {
  auto ctx = vxe::get_current_context();
  DocumentationFile doc;
  doc.title = title;
  doc.file_path = md_file_path;

  ctx->documentations[topic].sections[section].chapters[title] = doc;
}

VORTEX_API std::vector<std::string> vxe::GetSections(const std::string &topic) {
  auto ctx = vxe::get_current_context();
  std::vector<std::string> section_names;

  if (ctx->documentations.count(topic)) {
    for (auto const &[name, content] : ctx->documentations[topic].sections) {
      section_names.push_back(name);
    }
  }
  return section_names;
}

VORTEX_API std::vector<std::string> vxe::GetChapters(const std::string &topic, const std::string &section) {
  auto ctx = vxe::get_current_context();
  std::vector<std::string> chapter_titles;

  if (ctx->documentations.count(topic)) {
    auto &sections = ctx->documentations[topic].sections;
    if (sections.count(section)) {
      for (auto const &[title, file] : sections[section].chapters) {
        chapter_titles.push_back(title);
      }
    }
  }
  return chapter_titles;
}

VORTEX_API std::string
vxe::GetChapterFilePath(const std::string &topic, const std::string &section, const std::string &title) {
  auto ctx = vxe::get_current_context();

  auto it_topic = ctx->documentations.find(topic);
  if (it_topic == ctx->documentations.end())
    return "";

  auto &sections_map = it_topic->second.sections;
  auto it_section = sections_map.find(section);
  if (it_section == sections_map.end())
    return "";

  auto &chapters_map = it_section->second.chapters;
  auto it_chapter = chapters_map.find(title);
  if (it_chapter == chapters_map.end())
    return "";

  return it_chapter->second.file_path;
}

VORTEX_API void vxe::AddVortexDocumentation() {
  vxe::AddDocumentation(
      "vx", "Introduction", "Introduction", Cherry::GetPath("docs/get_started/contents/introduction/introduction.md"));
  vxe::AddDocumentation(
      "vx", "Introduction", "What is Vortex ?", Cherry::GetPath("docs/get_started/contents/introduction/what_is_vortex.md"));
  vxe::AddDocumentation(
      "vx", "Introduction", "Install Vortex", Cherry::GetPath("docs/get_started/contents/introduction/install_vortex.md"));
  vxe::AddDocumentation(
      "vx", "Introduction", "Get started", Cherry::GetPath("docs/get_started/contents/introduction/get_started.md"));

  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand Vortex Launcher",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "understand_vortexlauncher.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Discover interface",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "discover_interface.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Update the launcher",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "update_launcher.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Manage projects",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_projects.md"));

  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand logical contents:Brief",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "understand_logical_contents.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand logical contents:Manage system modules",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_modules.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand logical contents:Manage system plugins",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_plugins.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand static contents:Brief",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "understand_static_contents.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand static contents:Manage system templates",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_templates.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand static contents:Manage system contents",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_contents.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Manage Vortex Editors",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_vortex_editor_versions.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Subscribe to beta",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "subscribe_to_beta.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Launcher",
      "Uninstallation",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "uninstall.md"));

  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Understand Vortex Editor",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "understand_vortexeditor.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Discover interface",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "discover_interface.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Content Browser",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "content_browser.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Project settings",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "project_settings.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Console logs",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "console_logs.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Handle modules",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "handle_modules.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Handle plugins",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "handle_plugins.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Import contents",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "import_contents.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Create with templates",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "create_with_templates.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Brief",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "share_contents.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Export project/tool",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "export_project.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Share a content",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "share_a_content.md"));
  vxe::AddDocumentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Share a content",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "create_a_template.md"));
}

void vxe::PushEditMenuItem(
    const std::string &title,
    const std::function<void()> &action,
    const std::string &logo,
    const std::string &section) {
  auto ctx = vxe::get_current_context();
  ctx->editMenuItems.push_back({ title, action, logo, section });
}

void vxe::PopEditMenuItem(const int &count) {
  if (count <= 0)
    return;

  auto ctx = vxe::get_current_context();
  const int remove = (std::min)(count, static_cast<int>(ctx->editMenuItems.size()));
  ctx->editMenuItems.erase(ctx->editMenuItems.end() - remove, ctx->editMenuItems.end());
}

void vxe::ClearEditMenuItem() {
  auto ctx = vxe::get_current_context();
  ctx->editMenuItems.clear();
}

void vxe::PushCustomMenu(const std::string &title, const std::function<void()> &render) {
  auto ctx = vxe::get_current_context();
  ctx->customMenus.push_back({ title, render });
}

void vxe::PopCustomMenu(const int &count) {
  if (count <= 0)
    return;

  auto ctx = vxe::get_current_context();
  const int remove = (std::min)(count, static_cast<int>(ctx->customMenus.size()));
  ctx->customMenus.erase(ctx->customMenus.end() - remove, ctx->customMenus.end());
}

void vxe::ClearCustomMenus() {
  auto ctx = vxe::get_current_context();
  ctx->customMenus.clear();
}
#endif  // VORTEX_DISABLE