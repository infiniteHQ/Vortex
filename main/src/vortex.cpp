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
 * @param user_data Unused user data pointer (required by Vortex allocator signature).
 * @return A pointer to the allocated memory, or nullptr if allocation fails.
 */
static void *MallocWrapper(size_t size, void *user_data)
{
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
 * @param user_data Unused user data pointer (required by Vortex allocator signature).
 */
static void FreeWrapper(void *ptr, void *user_data)
{
    VX_UNUSED(user_data); // Unused parameter
    free(ptr);            // Call standard free
}

#else // DISABLE_DEFAULT_ALLOCATORS

/**
 * @brief Dummy MallocWrapper function.
 *
 * This function is a dummy wrapper for malloc used when default allocators are disabled.
 * It should not be called and will trigger an assertion.
 *
 * @param size Unused size parameter.
 * @param user_data Unused user data parameter.
 * @return Always returns nullptr, triggering an assertion.
 */
static void *MallocWrapper(size_t size, void *user_data)
{
    VX_UNUSED(size);      // Unused parameter
    VX_UNUSED(user_data); // Unused parameter
    VX_ASSERT(0);         // Trigger assertion, should not be called
    return nullptr;       // Return nullptr
}

/**
 * @brief Dummy FreeWrapper function.
 *
 * This function is a dummy wrapper for free used when default allocators are disabled.
 * It should not be called and will trigger an assertion.
 *
 * @param ptr Unused pointer parameter.
 * @param user_data Unused user data parameter.
 */
static void FreeWrapper(void *ptr, void *user_data)
{
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
 * This function creates a new Vortex context and initializes it. It ensures memory safety
 * by properly managing memory allocation and context switching.
 *
 * @return A pointer to the newly created Vortex context.
 */
VORTEX_API VxContext *VortexMaker::CreateContext()
{
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
 * This function sets the current Vortex context to the provided context pointer.
 * It offers flexibility by allowing custom thread-based context control if enabled.
 *
 * @param ctx A pointer to the Vortex context to be set as the current context.
 */
void VortexMaker::SetCurrentContext(VxContext *ctx)
{
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
 * This function destroys the specified Vortex context. If no context is provided,
 * it destroys the current context. It ensures proper context switching and memory deallocation.
 *
 * @param ctx A pointer to the Vortex context to be destroyed. If nullptr, the current context will be destroyed.
 */
VORTEX_API void VortexMaker::DestroyContext(VxContext *ctx)
{
    // Save the previous context before destroying the specified context
    VxContext *prev_ctx = GetCurrentContext();

    // If no context is provided, destroy the current context
    if (ctx == nullptr)
        ctx = prev_ctx;

    // Set the current context to the one to be destroyed
    SetCurrentContext(ctx);

    // If the previous context is different from the one to be destroyed, restore it
    SetCurrentContext((prev_ctx != ctx) ? prev_ctx : nullptr);

    // Deallocate memory for the context
    VX_DELETE(ctx);
}

/**
 * @brief Initialize initializes the VortexMaker.
 *
 * This function initializes the VortexMaker by setting the initialized flag in the context.
 */
VORTEX_API void VortexMaker::Initialize()
{
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
                                        void *user_data)
{
    // Set the custom allocator functions and user data for VortexMaker
    CVxAllocatorAllocFunc = alloc_func;
    CVxAllocatorFreeFunc = free_func;
    CVxAllocatorUserData = user_data;
}

/**
 * @brief Retrieve allocator functions and user data.
 *
 * This function is provided to facilitate copying allocators from one static/DLL
 * boundary to another. It retrieves the allocator functions and user data
 * used by the VortexMaker.
 *
 * @param p_alloc_func Pointer to store the allocator function.
 * @param p_free_func Pointer to store the free function.
 * @param p_user_data Pointer to store the user data.
 */
void VortexMaker::GetAllocatorFunctions(VortexMakerMemAllocFunc *p_alloc_func,
                                        VortexMakerMemFreeFunc *p_free_func,
                                        void **p_user_data)
{
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
VORTEX_API VxContext *VortexMaker::GetCurrentContext()
{
    return CVortexMaker; // Return the current Vortex context pointer
}

/**
 * @brief Allocate memory using the Vortex memory allocator.
 *
 * This function allocates memory of the specified size using the Vortex memory allocator.
 * It calls the CVxAllocatorAllocFunc function pointer with the provided size and allocator user data.
 * If IMGUI debug tools are enabled, it also calls DebugAllocHook to handle debug allocation information.
 *
 * @param size The size of memory to allocate.
 * @return A pointer to the allocated memory, or nullptr if allocation fails.
 */
void *VortexMaker::MemAlloc(size_t size)
{
    // Call the Vortex memory allocator function pointer to allocate memory
    void *ptr = (*CVxAllocatorAllocFunc)(size, CVxAllocatorUserData);

#ifndef VX_DISABLE_DEBUG_TOOLS
    // Check if IMGUI debug tools are enabled and CVortexMaker is valid
    if (VxContext *ctx = CVortexMaker)
    {
        // Call the debug allocation hook to handle debug information
        DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
    }
#endif

    return ptr; // Return the allocated memory pointer
}

// Vx_FREE() == VortexMaker::MemFree()
void VortexMaker::MemFree(void *ptr)
{
    if (ptr)
        if (VxContext *ctx = CVortexMaker)
            ctx->IO.MetricsActiveAllocations--;
    return (*CVxAllocatorFreeFunc)(ptr, CVxAllocatorUserData);
}

// We record the number of allocation in recent frames, as a way to
// audit/sanitize our guiding principles of "no allocations on idle/repeating
// frames"
void VortexMaker::DebugAllocHook(VortexMakerDebugAllocInfo *info, void *ptr,
                                 size_t size)
{
    // VortexMakerDebugAllocEntry* entry = &info->LastEntriesBuf[info->LastEntriesIdx];
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
 * It calculates the length of the string based on the provided end pointer (str_end) or using strlen if str_end is null.
 * If the buffer capacity is not enough to hold the new string, it resizes the buffer to accommodate the new string.
 * The string is then copied into the buffer, and a zero-terminator is added.
 *
 * @param str Pointer to the beginning of the string to append.
 * @param str_end Pointer to the end of the string to append (optional).
 */
void hString::append(const char *str, const char *str_end)
{
    // Calculate the length of the string
    int len = str_end ? (int)(str_end - str) : (int)strlen(str);

    // Calculate the write offset and needed size
    const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
    const int needed_sz = write_off + len;

    // Check if the buffer capacity is enough, resize if needed
    if (needed_sz >= Buf.Capacity)
    {
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
 * This function compares the provided version string with the defined VORTEX_VERSION.
 * If the strings do not match, it sets an error flag and asserts.
 *
 * @param version The version string to check.
 * @return True if the version string matches, false otherwise.
 */
bool VortexMaker::DebugCheckVersionAndDataLayout(const char *version)
{
    bool error = false;

    // Check if the provided version string matches the defined VORTEX_VERSION
    if (strcmp(version, VORTEX_VERSION) != 0)
    {
        error = true;
        // Assertion for debugging purposes, will only trigger in debug builds
        VX_ASSERT(strcmp(version, VORTEX_VERSION) == 0 && "Mismatched version string!");
    }

    return !error; // Return true if no error occurred
}

/**
 * @brief Deploy an event with the specified arguments.
 *
 * This function deploys an event with the specified event name and arguments.
 * It iterates through each EventManager in the Vortex context, checks if the output event
 * matches the provided event name, and calls the corresponding event function with the arguments.
 *
 * @param args A shared pointer to the arguments for the event.
 * @param event_name The name of the event to deploy.
 */
VORTEX_API void VortexMaker::DeployEvent(const std::shared_ptr<hArgs> &args, const std::string &event_name)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Ensure args is not null before proceeding
    if (!args)
    {
        VortexMaker::LogError("Core", "Null argument provided to DeployEvent.");
        return;
    }

    // Iterate through each EventManager in the Vortex context
    for (auto em : ctx.IO.em)
    {
        // Iterate through each output event in the EventManager
        for (auto output_event : em->m_output_events)
        {
            // Check if the output event name matches the provided event name
            if (output_event->m_name == event_name)
            {
                // Check if the event function pointer is valid
                if (output_event->m_foo)
                {
                    // Call the corresponding event function with the provided arguments
                    output_event->m_foo(args);
                }
                else
                {
                    // Print an error message if the event function is null
                    VortexMaker::LogError("Core", "Event function is null for event " + event_name);
                }
            }
        }
    }
}

/**
 * @brief Deploy an event with the specified arguments and callback.
 *
 * This function deploys an event with the specified event name and arguments.
 * It iterates through each EventManager in the Vortex context, checks if the output event
 * matches the provided event name, and calls the corresponding event function with the arguments.
 * Additionally, it calls the provided callback function after executing the event function.
 *
 * @param args A shared pointer to the arguments for the event.
 * @param event_name The name of the event to deploy.
 * @param callback The callback function to be called after executing the event function.
 */
VORTEX_API void VortexMaker::DeployEvent(const std::shared_ptr<hArgs> &args, const std::string &event_name, void (*callback)(std::shared_ptr<hArgs> args))
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Ensure args is not null before proceeding
    if (!args)
    {
        VortexMaker::LogError("Core", "Null argument provided to DeployEvent.");
        return;
    }

    // Ensure callback is not null before proceeding
    if (!callback)
    {
        VortexMaker::LogError("Core", "Null callback provided to DeployEvent.");
        return;
    }

    // Iterate through each EventManager in the Vortex context
    for (auto em : ctx.IO.em)
    {
        // Iterate through each output event in the EventManager
        for (auto output_event : em->m_output_events)
        {
            // Check if the output event name matches the provided event name
            if (output_event->m_name == event_name)
            {
                // Check if the event function pointer is valid
                if (output_event->m_foo)
                {
                    // Call the corresponding event function with the provided arguments
                    output_event->m_foo(args);
                    // Call the provided callback function with the arguments
                    callback(args);
                }
                else
                {
                    // Print an error message if the event function is null
                    VortexMaker::LogError("Core", "Event function is null for event " + event_name);
                }
            }
        }
    }
}

/**
 * @brief Call an event of a specific module with the specified arguments.
 *
 * This function calls an event with the specified event name and arguments
 * for a specific module in the Vortex context.
 * It iterates through each EventManager in the Vortex context, checks if the module
 * name matches the provided module name, and then checks if the input event name
 * matches the provided event name. If both conditions are met, it calls the
 * corresponding event function with the provided arguments.
 *
 * @param args A shared pointer to the arguments for the event.
 * @param event_name The name of the event to call.
 * @param module_name The name of the module where the event should be called.
 */
VORTEX_API void VortexMaker::CallModuleEvent(const std::shared_ptr<hArgs> &args, const std::string &event_name, const std::string &module_name)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Ensure args is not null before proceeding
    if (!args)
    {
        VortexMaker::LogError("Core", "Null argument provided to DeployEvent.");
        return;
    }

    // Iterate through each EventManager in the Vortex context
    for (auto em : ctx.IO.em)
    {
        // Check if the EventManager corresponds to the specified module
        if (em->m_name == module_name)
        {
            // Iterate through each input event in the EventManager
            for (auto input_event : em->m_input_events)
            {
                // Check if the input event name matches the provided event name
                if (input_event->m_name == event_name)
                {
                    // Check if the event function pointer is valid
                    if (input_event->m_foo)
                    {
                        // Call the corresponding event function with the provided arguments
                        input_event->m_foo(args);
                    }
                    else
                    {
                        // Print an error message if the event function is null
                        VortexMaker::LogError("Core", "Event function is null for event " + event_name);
                    }
                }
            }
        }
    }
}

/**
 * @brief Call an event of a specific module with the specified arguments and callback.
 *
 * This function calls an event with the specified event name and arguments
 * for a specific module in the Vortex context.
 * It iterates through each EventManager in the Vortex context, checks if the module
 * name matches the provided module name, and then checks if the input event name
 * matches the provided event name. If both conditions are met, it calls the
 * corresponding event function with the provided arguments and then calls
 * the provided callback function with the arguments.
 *
 * @param args A shared pointer to the arguments for the event.
 * @param event_name The name of the event to call.
 * @param module_name The name of the module where the event should be called.
 * @param callback The callback function to be called after executing the event function.
 */
VORTEX_API void VortexMaker::CallModuleEvent(const std::shared_ptr<hArgs> &args, const std::string &event_name, const std::string &module_name, void (*callback)(std::shared_ptr<hArgs> args))
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Ensure args is not null before proceeding
    if (!args)
    {
        VortexMaker::LogError("Core", "Null argument provided to CallModuleEvent.");
        return;
    }

    // Ensure callback is not null before proceeding
    if (!callback)
    {
        VortexMaker::LogError("Core", "Null callback provided to CallModuleEvent.");
        return;
    }

    // Iterate through each EventManager in the Vortex context
    for (auto em : ctx.IO.em)
    {
        // Check if the EventManager corresponds to the specified module
        if (em->m_name == module_name)
        {
            // Iterate through each input event in the EventManager
            for (auto input_event : em->m_input_events)
            {
                // Check if the input event name matches the provided event name
                if (input_event->m_name == event_name)
                {
                    // Check if the event function pointer is valid
                    if (input_event->m_foo)
                    {
                        // Call the corresponding event function with the provided arguments
                        input_event->m_foo(args);
                        // Call the provided callback function with the arguments
                        callback(args);
                    }
                    else
                    {
                        // Print an error message if the event function is null
                        VortexMaker::LogError("Core", "Event function is null for event " + event_name + " in module " + module_name);
                    }
                }
            }
        }
    }
}

VORTEX_API void VortexMaker::InstallModuleToSystem(const std::string &path)
{
    std::string modules_path = "~/.vx/modules";
    std::string json_file = path + "/module.json";

    {
        std::string cmd = "mkdir " + modules_path;
        system(cmd.c_str());
    }

    // Verify if the module is valid
    try
    {
        // Load JSON data from the module configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        std::string name = json_data["name"].get<std::string>();
        std::string proper_name = json_data["proper_name"].get<std::string>();
        std::string type = json_data["type"].get<std::string>();
        std::string version = json_data["version"].get<std::string>();
        std::string description = json_data["description"].get<std::string>();
        std::string author = json_data["author"].get<std::string>();

        VortexMaker::LogInfo("Core", "Installing the module " + name + "...");

        // Move the module in the final system path
        {
            std::string cmd = "cp -r " + path + " " + modules_path;
            system(cmd.c_str());
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

VORTEX_API void VortexMaker::AddModuleToProject(const std::string &module_name)
{
}

VORTEX_API std::string VortexMaker::replacePlaceholders(const std::string &command, const std::unordered_map<std::string, std::string> &replacements)
{
    std::string result = command;
    for (const auto &[placeholder, value] : replacements)
    {
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos)
        {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    return result;
}

#endif // VORTEX_DISABLE