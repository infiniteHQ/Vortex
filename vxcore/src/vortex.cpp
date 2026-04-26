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