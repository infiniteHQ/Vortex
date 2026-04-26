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