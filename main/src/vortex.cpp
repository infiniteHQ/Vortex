// The VortexMaker Project
// [Main sources]

// Index of this file:

//-------------------------------------------------------------------------
// [INCLUDES]
//-------------------------------------------------------------------------

#include "../include/vortex.h"
#ifndef VORTEX_DISABLE
#include "../include/vortex_internals.h"

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------
// Current runtime pointer.
#ifndef CVortexMaker
VxContext *CVortexMaker = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// - You probably don't want to modify that mid-program, and if you use
// global/static e.g. ImVector<> instances you may need to keep them accessible
// during program destruction.
// - DLL users: read comments above.

#ifndef DISABLE_DEFAULT_ALLOCATORS

/**
 * @brief Wrapper function for malloc.
 *
 * This function is a wrapper for the malloc function.
 * It allocates memory of the specified size using the standard malloc function.
 *
 * @param size The size of memory to allocate.
 * @param user_data Unused user data pointer (required by Vortex allocator
 * signature).
 * @return A pointer to the allocated memory, or nullptr if allocation fails.
 */
static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(user_data); // Unused parameter
  return malloc(size);  // Call standard malloc
}

/**
 * @brief Wrapper function for free.
 *
 * This function is a wrapper for the free function.
 * It deallocates memory pointed to by ptr using the standard free function.
 *
 * @param ptr Pointer to the memory block to deallocate.
 * @param user_data Unused user data pointer (required by Vortex allocator
 * signature).
 */
static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(user_data); // Unused parameter
  free(ptr);            // Call standard free
}

#else // DISABLE_DEFAULT_ALLOCATORS

/**
 * @brief Dummy MallocWrapper function.
 *
 * This function is a dummy wrapper for malloc used when default allocators are
 * disabled. It should not be called and will trigger an assertion.
 *
 * @param size Unused size parameter.
 * @param user_data Unused user data parameter.
 * @return Always returns nullptr, triggering an assertion.
 */
static void *MallocWrapper(size_t size, void *user_data) {
  VX_UNUSED(size);      // Unused parameter
  VX_UNUSED(user_data); // Unused parameter
  VX_ASSERT(0);         // Trigger assertion, should not be called
  return nullptr;       // Return nullptr
}

/**
 * @brief Dummy FreeWrapper function.
 *
 * This function is a dummy wrapper for free used when default allocators are
 * disabled. It should not be called and will trigger an assertion.
 *
 * @param ptr Unused pointer parameter.
 * @param user_data Unused user data parameter.
 */
static void FreeWrapper(void *ptr, void *user_data) {
  VX_UNUSED(ptr);       // Unused parameter
  VX_UNUSED(user_data); // Unused parameter
  VX_ASSERT(0);         // Trigger assertion, should not be called
}

#endif // DISABLE_DEFAULT_ALLOCATORS

static VortexMakerMemAllocFunc CVxAllocatorAllocFunc = MallocWrapper;
static VortexMakerMemFreeFunc CVxAllocatorFreeFunc = FreeWrapper;
static void *CVxAllocatorUserData = NULL;

/**
 * @brief CreateContext creates a new Vortex context.
 *
 * This function creates a new Vortex context and initializes it. It ensures
 * memory safety by properly managing memory allocation and context switching.
 *
 * @return A pointer to the newly created Vortex context.
 */
VORTEX_API VxContext *VortexMaker::CreateContext() {
  // Save the previous context before creating a new one
  VxContext *prev_ctx = GetCurrentContext();

  // Allocate memory for the new context
  VxContext *ctx = VX_NEW(VxContext);

  // Set the current context to the newly created context
  SetCurrentContext(ctx);

  // Initialize the new context
  Initialize();

  // Restore the previous context if it exists
  if (prev_ctx != nullptr)
    SetCurrentContext(prev_ctx);

  // Return the pointer to the newly created context
  return ctx;
}

/**
 * @brief SetCurrentContext sets the current Vortex context.
 *
 * This function sets the current Vortex context to the provided context
 * pointer. It offers flexibility by allowing custom thread-based context
 * control if enabled.
 *
 * @param ctx A pointer to the Vortex context to be set as the current context.
 */
void VortexMaker::SetCurrentContext(VxContext *ctx) {
#ifdef USE_CURRENT_CONTEXT_FUNC
  // If custom thread-based control is enabled, call the custom function
  USE_CURRENT_CONTEXT_FUNC(ctx);
#else
  // Otherwise, set the current context directly
  CVortexMaker = ctx;
#endif
}

/**
 * @brief DestroyContext destroys a Vortex context.
 *
 * This function destroys the specified Vortex context. If no context is
 * provided, it destroys the current context. It ensures proper context
 * switching and memory deallocation.
 *
 * @param ctx A pointer to the Vortex context to be destroyed. If nullptr, the
 * current context will be destroyed.
 */
VORTEX_API void VortexMaker::DestroyContext(VxContext *ctx) {
  // Save the previous context before destroying the specified context
  VxContext *prev_ctx = GetCurrentContext();

  // If no context is provided, destroy the current context
  if (ctx == nullptr)
    ctx = prev_ctx;

  // Set the current context to the one to be destroyed
  SetCurrentContext(ctx);

  // If the previous context is different from the one to be destroyed, restore
  // it
  SetCurrentContext((prev_ctx != ctx) ? prev_ctx : nullptr);

  // Deallocate memory for the context
  VX_DELETE(ctx);
}

/**
 * @brief Initialize initializes the VortexMaker.
 *
 * This function initializes the VortexMaker by setting the initialized flag in
 * the context.
 */
VORTEX_API void VortexMaker::Initialize() {
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
void VortexMaker::SetAllocatorFunctions(VortexMakerMemAllocFunc alloc_func,
                                        VortexMakerMemFreeFunc free_func,
                                        void *user_data) {
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
void VortexMaker::GetAllocatorFunctions(VortexMakerMemAllocFunc *p_alloc_func,
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
VORTEX_API VxContext *VortexMaker::GetCurrentContext() {
  return CVortexMaker; // Return the current Vortex context pointer
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
void *VortexMaker::MemAlloc(size_t size) {
  // Call the Vortex memory allocator function pointer to allocate memory
  void *ptr = (*CVxAllocatorAllocFunc)(size, CVxAllocatorUserData);

#ifndef VX_DISABLE_DEBUG_TOOLS
  // Check if IMGUI debug tools are enabled and CVortexMaker is valid
  if (VxContext *ctx = CVortexMaker) {
    // Call the debug allocation hook to handle debug information
    DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
  }
#endif

  return ptr; // Return the allocated memory pointer
}

// Vx_FREE() == VortexMaker::MemFree()
void VortexMaker::MemFree(void *ptr) {
  if (ptr)
    if (VxContext *ctx = CVortexMaker)
      ctx->IO.MetricsActiveAllocations--;
  return (*CVxAllocatorFreeFunc)(ptr, CVxAllocatorUserData);
}

// We record the number of allocation in recent frames, as a way to
// audit/sanitize our guiding principles of "no allocations on idle/repeating
// frames"
void VortexMaker::DebugAllocHook(VortexMakerDebugAllocInfo *info, void *ptr,
                                 size_t size) {
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

char hString::EmptyString[1] = {0};

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
bool VortexMaker::DebugCheckVersionAndDataLayout(const char *version) {
  bool error = false;

  // Check if the provided version string matches the defined VORTEX_VERSION
  if (strcmp(version, VORTEX_VERSION) != 0) {
    error = true;
    // Assertion for debugging purposes, will only trigger in debug builds
    VX_ASSERT(strcmp(version, VORTEX_VERSION) == 0 &&
              "Mismatched version string!");
  }

  return !error; // Return true if no error occurred
}

VORTEX_API void VortexMaker::CallOutputEvent(const std::string &event_name,
                                             ArgumentValues &args,
                                             ReturnValues &ret,
                                             const std::string &origin) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Iterate through each EventManager in the Vortex context
  for (auto em : ctx.IO.em) {
    // Iterate through each input event in the EventManager
    for (auto output_event : em->m_output_events) {
      // Check if the input event name matches the provided event name
      if (output_event->m_name == event_name) {
        // Check if the event function pointer is valid
        if (output_event->m_function) {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event", HappeningState::INFO,
              "Calling module output event \"" + output_event->m_name +
                  "\" of module \"" + em->m_name + "\" from \"" + origin +
                  "\"");

          // Call the corresponding event function with the provided arguments
          output_event->m_function(args, ret);

          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event", HappeningState::INFO,
              "Output event \"" + output_event->m_name + "\" of module \"" +
                  em->m_name + "\" called succefully from \"" + origin +
                  "\" !");
        } else {
          // Trigger a information trigger in the input event
          output_event->trigger_happening(
              origin + ":call_output_event", HappeningState::INFO,
              "Trying to call \"" + output_event->m_name + "\" of module \"" +
                  em->m_name + "\" from \"" + origin + "\" but it not exist !");
        }
      }
    }
  }
}

VORTEX_API void VortexMaker::CallInputEvent(const std::string &module_name,
                                            const std::string &event_name,
                                            ArgumentValues &args,
                                            ReturnValues &ret,
                                            const std::string &origin) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Iterate through each EventManager in the Vortex context
  for (auto em : ctx.IO.em) {
    // Check if the EventManager corresponds to the specified module
    if (em->m_name == module_name) {
      // Iterate through each input event in the EventManager
      for (auto input_event : em->m_input_events) {
        // Check if the input event name matches the provided event name
        if (input_event->m_name == event_name) {
          // Check if the event function pointer is valid
          if (input_event->m_function) {
            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event", HappeningState::INFO,
                "Calling module input event \"" + input_event->m_name +
                    "\" of module \"" + em->m_name + "\" from \"" + origin +
                    "\"");

            // Call the corresponding event function with the provided arguments
            input_event->m_function(args, ret);

            // Trigger a information trigger in the input event
            input_event->trigger_happening(
                origin + ":call_input_event", HappeningState::INFO,
                "Input event \"" + input_event->m_name + "\" of module \"" +
                    em->m_name + "\" called succefully from \"" + origin +
                    "\" !");
          } else {
            input_event->trigger_happening(
                origin + ":call_input_event", HappeningState::INFO,
                "Trying to call \"" + input_event->m_name + "\" of module \"" +
                    em->m_name + "\" from \"" + origin +
                    "\" but it not exist !");
          }
        }
      }
    }
  }
}

VORTEX_API void VortexMaker::InstallModuleToSystem(const std::string &path) {
  std::string modules_path = "~/.vx/modules";
  std::string json_file = path + "/module.json";

  try {
    auto json_data = VortexMaker::DumpJSON(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    modules_path += "/" + name + "." + version;

    VortexMaker::LogInfo("Core", "Installing the module " + name + "...");

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

VORTEX_API void
VortexMaker::AddModuleToProject(const std::string &module_name) {}

VORTEX_API std::string VortexMaker::replacePlaceholders(
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

VORTEX_API std::string VortexMaker::getHomeDirectory() {
  if (VortexMaker::IsLinux() || VortexMaker::IsMacOs()) {
    const char *homePath = std::getenv("HOME");
    if (homePath == nullptr) {
      throw std::runtime_error("HOME environment variable not set");
    }
    return std::string(homePath);
  } else if (VortexMaker::IsWindows()) {
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

  throw std::runtime_error(
      "Unknown platform: Unable to determine home directory");
}

VORTEX_API void
VortexMaker::InstallContentOnSystem(const std::string &directory) {
  fs::path dir_path(directory);

  if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
    VortexMaker::LogError("Core", "So such file or directory.");
    return;
  }

  bool module_found = fs::exists(dir_path / "module.json");
  bool template_found = fs::exists(dir_path / "template.json");

  if (module_found) {
    VortexMaker::InstallModuleToSystem(directory);
  } else if (template_found) {
    VortexMaker::InstallTemplateOnSystem(directory);
  }
}

VORTEX_API void VortexMaker::AddContentBrowserItem(
    const std::shared_ptr<ContenBrowserItem> &item) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  ctx.IO.contentbrowser_items.push_back(item);
}

VORTEX_API void VortexMaker::AddGeneralUtility(
    const std::shared_ptr<ModuleInterfaceUtility> &utility) {}

VORTEX_API void VortexMaker::PostCustomFolderToJson() {
  VxContext &ctx = *CVortexMaker;
  std::string path = ctx.projectPath.string() + "/.vx/configs/content_browser/";
  VortexMaker::createFolderIfNotExists(path);

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

  VortexMaker::PopulateJSON(json_data, file_path);
}

VORTEX_API void VortexMaker::PublishPool(const std::string &absolute_pool_path,
                                         const std::string &name) {
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

  ctx.IO.contentbrowser_pools.push_back({absolute_pool_path, name});

  VortexMaker::PostPoolsToJson();
}

VORTEX_API void VortexMaker::PublishContentBrowserCustomFolder(
    const std::string &path, const std::string &hex_color, const bool &isFav) {
  VxContext &ctx = *CVortexMaker;

  for (auto folder : ctx.IO.contentbrowser_customfolders) {
    if (folder->path == path) {
      folder->m_Color = hex_color;
      folder->m_IsFav = isFav;
      folder->path = path;
      VortexMaker::PostCustomFolderToJson();
      return;
    }
  }

  std::shared_ptr<ContentBrowserCustomFolder> new_folder =
      std::make_shared<ContentBrowserCustomFolder>();
  new_folder->m_Color = hex_color;
  new_folder->m_IsFav = isFav;
  new_folder->path = path;

  ctx.IO.contentbrowser_customfolders.push_back(new_folder);

  VortexMaker::PostCustomFolderToJson();
}

VORTEX_API bool
VortexMaker::ContentBrowserFolderIsFav(const std::string &path) {
  VxContext &ctx = *CVortexMaker;

  for (auto customized_folder : ctx.IO.contentbrowser_customfolders) {
    if (customized_folder->path == path) {
      return customized_folder->m_IsFav;
    }
  }
  return false;
}

VORTEX_API bool
VortexMaker::GetContentBrowserFolderColor(const std::string &path,
                                          ImU32 *color) {
  VxContext &ctx = *CVortexMaker;

  for (auto customized_folder : ctx.IO.contentbrowser_customfolders) {
    if (customized_folder->path == path) {
      *color = HexToImU32(customized_folder->m_Color);
      return true;
    }
  }
  return false;
}

VORTEX_API ImU32 VortexMaker::HexToImU32(const std::string &hex) {
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

  return IM_COL32(r, g, b, 255); // Alpha is set to 255 (opaque)
}

VORTEX_API std::string VortexMaker::ImU32ToHex(ImU32 color) {
  unsigned char r = (color >> IM_COL32_R_SHIFT) & 0xFF;
  unsigned char g = (color >> IM_COL32_G_SHIFT) & 0xFF;
  unsigned char b = (color >> IM_COL32_B_SHIFT) & 0xFF;

  std::stringstream ss;
  ss << "#" << std::setfill('0') << std::setw(2) << std::hex
     << static_cast<int>(r) << std::setfill('0') << std::setw(2) << std::hex
     << static_cast<int>(g) << std::setfill('0') << std::setw(2) << std::hex
     << static_cast<int>(b);

  return ss.str();
}

VORTEX_API void VortexMaker::FetchPools() {
  VxContext &ctx = *CVortexMaker;
  std::string path = ctx.projectPath.string() + "/.vx/configs/content_browser/";
  VortexMaker::createFolderIfNotExists(path);

  std::string file_path = path + "/pools.json";

  nlohmann::json json_data;
  json_data["main_pool"] = ctx.projectPath.string();
  json_data["pools"] = nlohmann::json::array();

  VortexMaker::createJsonFileIfNotExists(file_path, json_data);

  std::ifstream file(file_path);

  file >> json_data;

  // From master pool relative path (from project path)
  ctx.IO.contentbrowser_mainpool = json_data["main_pool"].get<std::string>();

  std::string projectPath = ctx.projectPath.string();
  ctx.IO.contentbrowser_absolute_mainpool =
      projectPath + "/" + ctx.IO.contentbrowser_mainpool;

  // From other pool absolute paths
  ctx.IO.contentbrowser_pools.clear();
  for (auto directory : json_data["pools"]) {
    std::string path = directory["path"].get<std::string>();
    std::string name = directory["name"].get<std::string>();
    ctx.IO.contentbrowser_pools.push_back({path, name});
  }
}

VORTEX_API void VortexMaker::PostPoolsToJson() {
  VxContext &ctx = *CVortexMaker;
  std::string path = ctx.projectPath.string() + "/.vx/configs/content_browser/";
  VortexMaker::createFolderIfNotExists(path);

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

  VortexMaker::PopulateJSON(json_data, file_path);
}

VORTEX_API void VortexMaker::OpenURL(const std::string &url) {
#if defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
  std::string command = "open " + url;
  system(command.c_str());
#elif defined(__linux__)
  std::string command = "xdg-open " + url;
  system(command.c_str());
#else
  std::cerr << "Unsupported platform: unable to open URL." << std::endl;
#endif
}

VORTEX_API void VortexMaker::FetchCustomFolders() {
  VxContext &ctx = *CVortexMaker;
  std::string path = (ctx.projectPath / ".vx/configs/content_browser").string();
  VortexMaker::createFolderIfNotExists(path);

  std::string file_path = path + "/customized_folders.json";

  nlohmann::json json_data = {{"custom_folders", nlohmann::json::array()}

  };

  VortexMaker::createJsonFileIfNotExists(file_path, json_data);

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
      std::shared_ptr<ContentBrowserCustomFolder> new_folder =
          std::make_shared<ContentBrowserCustomFolder>();
      new_folder->m_Color = directory["color"].get<std::string>();
      new_folder->m_IsFav = directory["isFav"].get<bool>();
      new_folder->path = directory["path"].get<std::string>();

      VortexMaker::createFolderIfNotExists(new_folder->path);

      ctx.IO.contentbrowser_customfolders.push_back(new_folder);
    }
  } catch (const std::exception &e) {
    std::cerr << "Erreur lors de la lecture ou du traitement du fichier JSON : "
              << e.what() << std::endl;
  }
}

VORTEX_API void VortexMaker::Copy(std::vector<std::string> selection,
                                  bool in_addition) {
  VxContext &ctx = *CVortexMaker;

  if (!in_addition) {
    ctx.IO.copy_selection.clear();
  }

  for (auto selected : selection) {
    ctx.IO.copy_selection.push_back(selected);
  }
}

VORTEX_API void VortexMaker::SubmitRename(const std::string &oldPathStr,
                                          const std::string &newName) {
  fs::path oldPath(oldPathStr);
  fs::path parentDir = oldPath.parent_path();

  fs::path newPath = parentDir / newName;

  if (oldPath == newPath)
    return;

  if (fs::exists(newPath)) {
    std::string baseName = newPath.stem().string();
    std::string extension =
        newPath.has_extension() ? newPath.extension().string() : "";

    std::regex suffixPattern(R"( \((\d+)\)$)");
    std::smatch match;

    int counter = 1;

    if (std::regex_search(baseName, match, suffixPattern)) {
      counter = std::stoi(match[1].str()) + 1;
      baseName = baseName.substr(0, baseName.size() - match[0].str().size());
    }

    fs::path candidate;
    do {
      candidate = parentDir /
                  (baseName + " (" + std::to_string(counter) + ")" + extension);
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

VORTEX_API void VortexMaker::Cut(std::vector<std::string> selection,
                                 bool in_addition) {
  VxContext &ctx = *CVortexMaker;

  if (!in_addition) {
    ctx.IO.cut_selection.clear();
  }

  for (auto selected : selection) {
    ctx.IO.cut_selection.push_back(selected);
  }
}

VORTEX_API void VortexMaker::ClearCutSelection() {
  VxContext &ctx = *CVortexMaker;

  ctx.IO.cut_selection.clear();
}

VORTEX_API void VortexMaker::ClearCopySelection() {
  VxContext &ctx = *CVortexMaker;

  ctx.IO.copy_selection.clear();
}

static bool isStrictSubPath(const fs::path &potentialSub,
                            const fs::path &base) {
  auto rel = fs::weakly_canonical(potentialSub)
                 .lexically_relative(fs::weakly_canonical(base));
  return !rel.empty() && rel.native()[0] != '.';
}

void CopyDirectoryRecursively(const fs::path &src, const fs::path &dest,
                              const fs::path &destRoot) {
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

    if (isStrictSubPath(fromCanonical, destRootCanonical) ||
        fromCanonical == destRootCanonical) {
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

VORTEX_API void
VortexMaker::PasteAllSelections(const std::string &target_path_str) {
  VxContext &ctx = *CVortexMaker;
  fs::path targetPath(target_path_str);

  auto generateNonConflictingPath =
      [](const fs::path &targetDir, const fs::path &originalName) -> fs::path {
    fs::path newPath = targetDir / originalName;
    if (!fs::exists(newPath))
      return newPath;

    std::string stem = originalName.stem().string();
    std::string extension =
        originalName.has_extension() ? originalName.extension().string() : "";
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

    fs::path destPath =
        generateNonConflictingPath(targetPath, srcPath.filename());

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
      std::cerr << "Erreur de copie : " << srcPath << " → " << destPath << " : "
                << e.what() << "\n";
    }
  }

  for (const auto &srcStr : ctx.IO.cut_selection) {
    fs::path srcPath(srcStr);
    if (!fs::exists(srcPath)) {
      std::cerr << "Source path n'existe pas (cut) : " << srcStr << "\n";
      continue;
    }

    fs::path destPath =
        generateNonConflictingPath(targetPath, srcPath.filename());

    // 🚫 Prévention contre le déplacement récursif
    if (fs::is_directory(srcPath) && isStrictSubPath(destPath, srcPath)) {
      std::cerr << "Déplacement récursif détecté, opération ignorée : "
                << srcPath << " → " << destPath << "\n";
      continue;
    }

    try {
      fs::rename(srcPath, destPath);
    } catch (const std::exception &e) {
      std::cerr << "Erreur de déplacement : " << srcPath << " → " << destPath
                << " : " << e.what() << "\n";
    }
  }
}

VORTEX_API void VortexMaker::RenameFolder(const std::string &target_path,
                                          const std::string &new_name) {
  namespace fs = std::filesystem;

  try {
    fs::path target(target_path);
    if (!fs::exists(target) || !fs::is_directory(target)) {
      throw std::runtime_error(
          "Target path does not exist or is not a directory.");
    }

    fs::path new_path = target.parent_path() / new_name;
    fs::rename(target, new_path);
  } catch (const fs::filesystem_error &e) {
    throw std::runtime_error(std::string("Filesystem error: ") + e.what());
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error: ") + e.what());
  }
}

VORTEX_API void VortexMaker::RefreshProjectThemes() {
  VxContext &ctx = *CVortexMaker;

  std::string home = ctx.projectPath.string();
  std::string themes_path = home + "/.vx/configs/themes/data";
  std::string config_path = home + "/.vx/configs/themes";
  std::string json_file = config_path + "/themes.json";

  VortexMaker::createFolderIfNotExists(themes_path);
  VortexMaker::createFolderIfNotExists(config_path);

  nlohmann::json defaultData = {{"used_theme", "dark"},
                                {"override_themes", nlohmann::json::array()}};
  VortexMaker::createJsonFileIfNotExists(json_file, defaultData);

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
        VortexMaker::LogError("Core", "Failed to parse theme file: " +
                                          entry.path().string());
        VortexMaker::LogError("Core", e.what());
      }
    }
  }

  std::cout << ctx.IO.themes.size() << std::endl;

  try {
    std::ifstream configFile(json_file);
    nlohmann::json configJson;
    configFile >> configJson;

    ctx.IO.used_theme = configJson.value("used_theme", "dark");
    ctx.IO.override_themes =
        configJson.value("override_themes", std::vector<std::string>());
  } catch (const std::exception &e) {
    VortexMaker::LogError("Core", "Failed to load theme config: " +
                                      std::string(e.what()));
  }
}

VORTEX_API void UpdateProjectTheme(const std::shared_ptr<Theme> &theme,
                                   const std::string &title) {
  std::string themes_path =
      VortexMaker::getHomeDirectory() + "/.vx/configs/themes/data";
  VortexMaker::createFolderIfNotExists(themes_path);

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
    VortexMaker::LogInfo("Core", "Theme '" + theme->label + "' updated.");
  } catch (const std::exception &e) {
    VortexMaker::LogError("Core", "Failed to update theme file: " + filepath);
    VortexMaker::LogError("Core", e.what());
  }
}

VORTEX_API std::shared_ptr<Theme>
VortexMaker::GetTheme(const std::string &label) {

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
VORTEX_API std::shared_ptr<Theme> VortexMaker::GetSelectedTheme() {

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

  VortexMaker::LogError("Core", "No theme matched used_theme: '" + used + "'");
  return nullptr;
}

VORTEX_API void
VortexMaker::CreateNewTheme(const std::shared_ptr<Theme> &base_theme,
                            const std::string &title) {
  if (!base_theme) {
    VortexMaker::LogError("Theme",
                          "Base theme is null. Cannot create new theme.");
    return;
  }

  std::string themes_path =
      VortexMaker::GetCurrentContext()->projectPath.string() +
      "/.vx/configs/themes/data";
  VortexMaker::createFolderIfNotExists(themes_path);

  std::string base_filename = title;
  std::transform(
      base_filename.begin(), base_filename.end(), base_filename.begin(),
      [](unsigned char c) { return std::isspace(c) ? '_' : std::tolower(c); });

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
    VortexMaker::LogInfo("Theme", "New theme created: " + final_filename);
  } catch (const std::exception &e) {
    VortexMaker::LogError("Theme", "Failed to write new theme file: " +
                                       std::string(e.what()));
  }
}
VORTEX_API void VortexMaker::VerifyAndPouplateThemes() {
  std::string themes_path =
      VortexMaker::GetCurrentContext()->projectPath.string() +
      "/.vx/configs/themes/data";

  VortexMaker::createFolderIfNotExists(themes_path);

  const std::vector<std::string> required_themes = {"dark.json", "clear.json"};

  for (const auto &theme_file : required_themes) {
    std::string path = themes_path + "/" + theme_file;
    bool needs_creation = false;

    if (!fs::exists(path)) {
      VortexMaker::LogInfo("Theme", theme_file + " is missing. Recreating.");
      needs_creation = true;
    } else {
      try {
        std::ifstream in(path);
        nlohmann::json theme_json;
        in >> theme_json;

        if (!theme_json.contains("label") || !theme_json.contains("name") ||
            !theme_json.contains("description") ||
            !theme_json.contains("authors") || !theme_json.contains("theme")) {
          VortexMaker::LogError("Theme",
                                theme_file + " is invalid. Recreating.");
          needs_creation = true;
        }
      } catch (...) {
        VortexMaker::LogError("Theme",
                              theme_file + " is corrupted. Recreating.");
        needs_creation = true;
      }
    }

    if (needs_creation && theme_file == "dark.json") {
      std::shared_ptr<Theme> dark_theme = std::make_shared<Theme>();
      dark_theme->description = "Default dark theme";
      dark_theme->authors = {"Vortex DevTeam"};

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
      light_theme->authors = {"Vortex DevTeam"};

      std::map<std::string, std::string> properties = {
          {"color_header", "#EAEAEA"},
          {"color_header_hovered", "#DCDCDC"},
          {"color_header_active", "#D0D0D0"},
          {"color", "#F5F5F5"},
          {"color_hovered", "#EBEBEB"},
          {"color_active", "#DADADA"},
          {"color_framebg", "#FFFFFF"},
          {"color_framebg_hovered", "#F0F0F0"},
          {"color_framebg_active", "#E0E0E0"},
          {"color_text", "#252525FF"},
          {"color_tab", "#F2F2F2"},
          {"color_tab_hovered", "#E6E6E6"},
          {"color_tab_active", "#DDDDDD"},
          {"color_tab_unfocused", "#F2F2F2"},
          {"color_tab_unfocused_active", "#E6E6E6"},
          {"color_titlebg", "#F0F0F0"},
          {"color_titlebg_active", "#E0E0E0"},
          {"color_titlebg_collapsed", "#FAFAFA"},
          {"color_resizegrip", "#AAAAAA40"},
          {"color_resizegrip_hovered", "#888888AA"},
          {"color_resizegrip_active", "#3D7EFF"},
          {"color_scrollbar_bg", "#EEEEEE"},
          {"color_scrollbar_grab", "#C2C2C2"},
          {"color_scrollbar_grab_hovered", "#A8A8A8"},
          {"color_scrollbar_grab_active", "#8F8F8F"},
          {"color_checkmark", "#3D7EFF"},
          {"color_slidergrab", "#BBBBBB"},
          {"color_slidergrab_active", "#3D7EFF"},
          {"color_separator", "#DDDDDD"},
          {"color_separator_active", "#3D7EFF"},
          {"color_separator_hovered", "#CCCCCC"},
          {"color_window_bg", "#FFFFFF"},
          {"color_child_bg", "#F9F9F9"},
          {"color_popup_bg", "#FFFFFF"},
          {"color_border", "#DDDDDD"},
          {"color_table_header_bg", "#EDEDED"},
          {"color_table_border_light", "#F3F3F3"},
          {"color_menubar_bg", "#FAFAFA"},

          // Buttons
          {"button_color_border", "#CCCCCC"},
          {"button_color_border_hovered", "#BBBBBB"},
          {"button_color_border_clicked", "#AAAAAA"},
          {"button_color_border_pressed", "#3D7EFF"},
          {"button_color_bg", "#F0F0F0"},
          {"button_color_bg_hovered", "#E6E6E6"},
          {"button_color_bg_pressed", "#DADADA"},
          {"button_color_bg_clicked", "#DADADA"},
          {"button_color_text", "#333333"},
          {"button_color_text_hovered", "#000000"},
          {"button_color_text_pressed", "#000000"},
          {"button_color_underline", "#CCCCCC"},
          {"button_color_underline_hovered", "#AAAAAA"},
          {"button_color_underline_pressed", "#3D7EFF"},
          {"button_size_x", "0"},
          {"button_size_y", "0"},
          {"button_padding_x", "6"},
          {"button_padding_y", "6"},
          {"button_scale", "0"},

          // Checkbox
          {"checkbox_color_border", "#CCCCCC"},
          {"checkbox_color_border_hovered", "#AAAAAA"},
          {"checkbox_color_border_clicked", "#888888"},
          {"checkbox_color_border_pressed", "#3D7EFF"},
          {"checkbox_color_bg", "#FFFFFF"},
          {"checkbox_color_bg_hovered", "#F5F5F5"},
          {"checkbox_color_bg_pressed", "#E5E5E5"},
          {"checkbox_color_bg_clicked", "#E5E5E5"},
          {"checkbox_size_x", "6"},
          {"checkbox_size_y", "6"},

          // Tooltip
          {"tooltip_color_border", "#CCCCCC"},
          {"tooltip_color_border_hovered", "#AAAAAA"},
          {"tooltip_color_border_clicked", "#888888"},
          {"tooltip_color_bg", "#FAFAFA"},
          {"tooltip_color_bg_hovered", "#F0F0F0"},
          {"tooltip_color_bg_clicked", "#E0E0E0"},

          // Text Area
          {"text_area_color_text", "#222222"},

          // Text
          {"text_color_text", "#222222"},

          // Titles
          {"title_color_text", "#000000"},

          // Notification
          {"notification_color_border", "#AAAAAA"},
          {"notification_color_border_hovered", "#3D7EFF"},
          {"notification_color_bg", "#FFFFFF"},
          {"notification_color_bg_hovered", "#F7F7F7"},

          // Blocks
          {"block_color", "#F5F5F5"},
          {"block_color_hovered", "#E5E5E5"},
          {"block_color_pressed", "#D5D5D5"},
          {"block_border_color", "#CCCCCC"},
          {"block_border_color_hovered", "#BBBBBB"},
          {"block_border_color_pressed", "#3D7EFF"},
          {"block_border_radius", "0.0"},
          {"block_border_size", "1.0"}};

      for (const auto &[k, v] : properties) {
        light_theme->theme[k] = v;
      }

      CreateNewTheme(light_theme, "Clear");
    }
  }
}

VORTEX_API void VortexMaker::UpdateProjectThemesComfig() {
  VxContext &ctx = *CVortexMaker;
  std::string config_path =
      VortexMaker::getHomeDirectory() + "/.vx/configs/themes";
  std::string json_file = config_path + "/themes.json";

  nlohmann::json configJson;
  configJson["used_theme"] = ctx.IO.used_theme;
  configJson["override_themes"] = ctx.IO.override_themes;

  try {
    std::ofstream out(json_file);
    out << std::setw(4) << configJson << std::endl;
    VortexMaker::LogInfo("Core", "Theme configuration updated.");
  } catch (const std::exception &e) {
    VortexMaker::LogError("Core", "Failed to update theme config file: " +
                                      std::string(e.what()));
  }
}

VORTEX_API void VortexMaker::ThemeRebuilded() {
  VxContext &ctx = *CVortexMaker;
  ctx.IO.theme_changed = false;
}

VORTEX_API void VortexMaker::RebuildTheme() {
  VxContext &ctx = *CVortexMaker;
  ctx.IO.theme_changed = true;
}

VORTEX_API bool VortexMaker::IsThemeNeedsRebuild() {
  VxContext &ctx = *CVortexMaker;
  return ctx.IO.theme_changed;
}

VORTEX_API std::string VortexMaker::CreateFile(const std::string &path) {
  fs::path basePath(path);
  std::string baseName = "New file";
  std::string extension = ".txt";
  fs::path fullPath = basePath / (baseName + extension);

  int counter = 2;
  while (fs::exists(fullPath)) {
    fullPath =
        basePath / (baseName + " " + std::to_string(counter) + extension);
    ++counter;
  }

  std::ofstream ofs(fullPath);
  ofs.close();

  return fs::absolute(fullPath).string();
}

VORTEX_API std::string VortexMaker::CreateFolder(const std::string &path) {
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

VORTEX_API void VortexMaker::RenameFile(const std::string &target_path,
                                        const std::string &new_name) {
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

VORTEX_API void VortexMaker::DeleteFile(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path) ||
        !std::filesystem::is_regular_file(target_path)) {
      throw std::invalid_argument(
          "The specified path does not exist or is not a file.");
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

VORTEX_API void VortexMaker::DeleteFolder(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path) ||
        !std::filesystem::is_directory(target_path)) {
      throw std::invalid_argument(
          "The specified path does not exist or is not a directory.");
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

VORTEX_API void VortexMaker::DeletePath(const std::string &target_path) {
  try {
    if (!std::filesystem::exists(target_path)) {
      throw std::invalid_argument("The specified path does not exist.");
    }

    if (std::filesystem::is_directory(target_path)) {
      DeleteFolder(target_path);
    } else if (std::filesystem::is_regular_file(target_path)) {
      DeleteFile(target_path);
    } else {
      throw std::invalid_argument(
          "The specified path is neither a regular file nor a directory.");
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
  } catch (...) {
    std::cerr << "An unknown error occurred while processing the path." << '\n';
  }
}
#endif // VORTEX_DISABLE