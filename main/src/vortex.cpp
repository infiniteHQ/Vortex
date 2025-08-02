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
  fs::path baseAbs;
  fs::path subAbs;

  try {
    baseAbs = fs::canonical(base);
    subAbs = fs::canonical(potentialSub);
  } catch (const fs::filesystem_error &) {
    baseAbs = fs::weakly_canonical(base);
    subAbs = fs::weakly_canonical(potentialSub);
  }

  if (baseAbs == subAbs) {
    return false;
  }

  return subAbs.string().rfind(baseAbs.string(), 0) == 0;
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

    if (isStrictSubPath(from, destRoot) || from == destRoot) {
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