// The VortexMaker Project, version 1.0
// [Main sources & docs]

// Index of this file:

//  [Includes]
//  [Context Utility: RegisterProvider] => Register a Provider into the context

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
static void *MallocWrapper(size_t size, void *user_data)
{
  VX_UNUSED(user_data);
  return malloc(size);
}
static void FreeWrapper(void *ptr, void *user_data)
{
  VX_UNUSED(user_data);
  free(ptr);
}
#else
static void *MallocWrapper(size_t size, void *user_data)
{
  VX_UNUSED(user_data);
  VX_UNUSED(size);
  VX_ASSERT(0);
  return NULL;
}
static void FreeWrapper(void *ptr, void *user_data)
{
  VX_UNUSED(user_data);
  VX_UNUSED(ptr);
  VX_ASSERT(0);
}
#endif
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



//-----------------------------------------------------------------------------

void VortexMaker::SetAllocatorFunctions(VortexMakerMemAllocFunc alloc_func,
                                        VortexMakerMemFreeFunc free_func,
                                        void *user_data)
{
  CVxAllocatorAllocFunc = alloc_func;
  CVxAllocatorFreeFunc = free_func;
  CVxAllocatorUserData = user_data;
}

// This is provided to facilitate copying allocators from one static/DLL
// boundary to another (e.g. retrieve default allocator of your executable
// address space)
void VortexMaker::GetAllocatorFunctions(VortexMakerMemAllocFunc *p_alloc_func,
                                        VortexMakerMemFreeFunc *p_free_func,
                                        void **p_user_data)
{
  *p_alloc_func = CVxAllocatorAllocFunc;
  *p_free_func = CVxAllocatorFreeFunc;
  *p_user_data = CVxAllocatorUserData;
}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VORTEX_API VxContext *VortexMaker::GetCurrentContext() { return CVortexMaker; }
//-----------------------------------------------------------------------------

// IM_ALLOC() == ImGui::MemAlloc()
void *VortexMaker::MemAlloc(size_t size)
{
  void *ptr = (*CVxAllocatorAllocFunc)(size, CVxAllocatorUserData);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
  if (VxContext *ctx = CVortexMaker)
    DebugAllocHook(&ctx->debugAllocInfo, ptr, size);
#endif
  return ptr;
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

void hString::append(const char *str, const char *str_end)
{
  int len = str_end ? (int)(str_end - str) : (int)strlen(str);

  // Add zero-terminator the first time
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;
  if (write_off + len >= Buf.Capacity)
  {
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  Buf.resize(needed_sz);
  memcpy(&Buf[write_off - 1], str, (size_t)len);
  Buf[write_off - 1 + len] = 0;
}

bool VortexMaker::DebugCheckVersionAndDataLayout(const char *version)
{
  bool error = false;
  if (strcmp(version, VORTEX_VERSION) != 0)
  {
    error = true;
    VX_ASSERT(strcmp(version, VORTEX_VERSION) == 0 && "Mismatched version string!");
  }
  return !error;
}


// Correction de la fonction CreateTask
/*
std::shared_ptr<Task> VortexMaker::CreateTask(std::string tasktype, std::string component, std::string uniqueID, int priority, std::shared_ptr<hArgs> props)
{
  VxContext &ctx = *CVortexMaker;

  // Utilisation de make_shared pour créer la tâche
  std::shared_ptr<Task> task = TaskFactory::getInstance().createInstance(tasktype);

  if (task)
  { // Vérification si la tâche a été créée avec succès
    task->id = uniqueID;
    task->tasktype = tasktype;
    task->component = component;
    task->priority = priority;
    task->props = props;
    task->state = "not_started";

    // Ajout de la tâche aux listes appropriées
    //ctx.IO.tasksToProcess.push_back(task);
    ctx.IO.tasks.push_back(task);
  }
  else
  {
    std::cerr << "Failed to create task of type: " << tasktype << std::endl;
  }

  return task;
}
*/

/*
void VxPackage::ExecuteActions(std::string sequence, std::shared_ptr<VxPackage> package)
{
  for (auto action : package->actions)
  {
    if (sequence == action->executionSequence)
    {
      if (action->type == "command")
      {

        std::string cmd = "";
        cmd += "cd " + package->distPath + "/build && " + action->command;

        std::string label = "action-" + action->type + "-" + action->executionSequence + "-";
        label += action->priority;

        package->SetDiagCode(label, system(cmd.c_str()));
      }
    }
  }
}*/

// Demain : Finir l'edition complete des packages et de la toolchains (y compris les configuration de target de la toolchain, etc...)
// Preparer la beta !!

VORTEX_API void VortexMaker::CreateNewTask(std::shared_ptr<Task> task, std::string tasktype, std::string uniqueID, int priority, std::shared_ptr<hArgs> props)
{
}

/*
void VortexMaker::DeleteHost(const std::shared_ptr<VxHost>& host){
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string host_path = ctx->projectPath;
    host_path += "/.vx/data/hosts/" + host->name;

    std::string host_distpath = ctx->projectPath;
    host_distpath += "/.vx/dist/hosts/" + host->name;

    
    // Verify if the toolchain_path is not empty
    if (host_path != "") {
        std::string cmd = "rm -rf " + host_path + "";
        system(cmd.c_str());
    }


    if (host_distpath != "") {
        std::string cmd = "rm -rf " + host_distpath + "";
        system(cmd.c_str());
    }

    ctx->IO.hosts.erase(std::remove(ctx->IO.hosts.begin(), ctx->IO.hosts.end(), host), ctx->IO.hosts.end());

  VortexMaker::RefreshToolchains();

}*/

/*

void VortexMaker::DeleteGpos(const std::shared_ptr<VxGPOSystem>& gpos){
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string gpos_path = ctx->projectPath;
    gpos_path += "/.vx/data/gpos/" + gpos->name;

    std::string gpos_distpath = ctx->projectPath;
    gpos_distpath += "/.vx/dist/gpos/" + gpos->name;


    // Create package.config into the baseDir folder
    
    // Verify if the toolchain_path is not empty
    if (gpos_path != "") {
        std::string cmd = "rm -rf " + gpos_path + "";
        system(cmd.c_str());
    }


    if (gpos_distpath != "") {
        std::string cmd = "rm -rf " + gpos_distpath + "";
        system(cmd.c_str());
    }

    ctx->IO.gpoSystems.erase(std::remove(ctx->IO.gpoSystems.begin(), ctx->IO.gpoSystems.end(), gpos), ctx->IO.gpoSystems.end());

  VortexMaker::RefreshGpos();

}
*/

/*

void VortexMaker::CreateGpos(const std::string& name, const std::string& author){
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::string new_gpos_path = ctx->projectPath;
    new_gpos_path += "/.vx/data/gpos/" + name;

    std::string new_gpos_distpath = ctx->projectPath;
    new_gpos_distpath += "/.vx/dist/gpos/" + name;



    // Create package.config into the baseDir folder
    {
        std::string cmd = "mkdir " + new_gpos_path + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_gpos_path + "/gpos.config";
        system(cmd.c_str());
    }

    {
        std::string cmd = "mkdir " + new_gpos_path + "/data";
        system(cmd.c_str());
    }

    {
        std::string cmd = "mkdir " + new_gpos_path + "/data/packages";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_gpos_path + "/data/patchs";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_gpos_path + "/data/scripts";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_gpos_path + "/data/tasklists";
        system(cmd.c_str());
    }


    {
        std::string cmd = "mkdir " + new_gpos_distpath + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_gpos_distpath + "/gpos.dist.config";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_gpos_distpath + "/data";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_gpos_distpath + "/snapshots";
        system(cmd.c_str());
    }

      // Create json object and store it into  "touch " + envPath + "/toolchain.config"
      nlohmann::json j;
      j["gpos"]["author"] = author;
      j["gpos"]["description"] = "This is a toolchain";
      j["gpos"]["name"] = name;
      j["gpos"]["platform"] = "???";
      j["gpos"]["state"] = "???";
      j["gpos"]["type"] = "toolchain";
      j["gpos"]["vendor"] = "???";
      j["gpos"]["version"] = "1.0.0";

      j["data"]["packages"] = "./data/packages/";
      j["data"]["patchs"] = "./data/patchs/";
      j["data"]["scripts"] = "./data/scripts/";

      j["content"]["packages"] = nlohmann::json::array();
      j["content"]["patchs"] = nlohmann::json::array();
      j["content"]["tasklists"] = nlohmann::json::array();
      
      j["build"]["use_toolchain"] = "???";

      j["configs"]["builder_arch"] = "???";
      j["configs"]["host_arch"] = "???";
      j["configs"]["target_arch"] = "???";
      j["configs"]["compression"] = "???";

      // Store this into toolchain.config
      std::ofstream o(new_gpos_path + "/gpos.config");
      o << std::setw(4) << j << std::endl;
      o.close();

      VortexMaker::RefreshGpos();

}
*/

/*

void VortexMaker::DeleteToolchain(const std::shared_ptr<VxToolchain>& toolchain){
    VxContext *ctx = VortexMaker::GetCurrentContext();
    std::string toolchain_path = ctx->projectPath;
    toolchain_path += "/.vx/data/toolchains/" + toolchain->name;

    std::string toolchain_distpath = ctx->projectPath;
    toolchain_distpath += "/.vx/dist/toolchains/" + toolchain->name;


    // Create package.config into the baseDir folder
    
    // Verify if the toolchain_path is not empty
    if (toolchain_path != "") {
        std::string cmd = "rm -rf " + toolchain_path + "";
        system(cmd.c_str());
    }


    if (toolchain_distpath != "") {
        std::string cmd = "rm -rf " + toolchain_distpath + "";
        system(cmd.c_str());
    }

    ctx->IO.toolchains.erase(std::remove(ctx->IO.toolchains.begin(), ctx->IO.toolchains.end(), toolchain), ctx->IO.toolchains.end());

  VortexMaker::RefreshToolchains();

}

*/

/*
void VortexMaker::CreateHost(const std::string& name, const std::string& author){
    VxContext *ctx = VortexMaker::GetCurrentContext();

    std::string new_host_path = ctx->projectPath;
    new_host_path += "/.vx/data/hosts/" + name;

    std::string new_toolchain_distpath = ctx->projectPath;
    new_toolchain_distpath += "/.vx/dist/hosts/" + name;



    // Create package.config into the baseDir folder
    {
        std::string cmd = "mkdir " + new_host_path + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_host_path + "/host.config";
        system(cmd.c_str());
    }

    {
        std::string cmd = "mkdir " + new_host_path + "/data";
        system(cmd.c_str());
    }

    {
        std::string cmd = "mkdir " + new_host_path + "/data/packages";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_host_path + "/data/patchs";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_host_path + "/data/scripts";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_host_path + "/data/tasklists";
        system(cmd.c_str());
    }


    {
        std::string cmd = "mkdir " + new_toolchain_distpath + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_toolchain_distpath + "/host.dist.config";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_distpath + "/data";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_distpath + "/snapshots";
        system(cmd.c_str());
    }




      // Create json object and store it into  "touch " + envPath + "/toolchain.config"
      nlohmann::json j;
      j["host"]["author"] = author;
      j["host"]["description"] = "This is a toolchain";
      j["host"]["name"] = name;
      j["host"]["platform"] = "???";
      j["host"]["state"] = "???";
      j["host"]["type"] = "toolchain";
      j["host"]["vendor"] = "???";
      j["host"]["version"] = "1.0.0";

      j["data"]["packages"] = "./data/packages/";
      j["data"]["patchs"] = "./data/patchs/";
      j["data"]["scripts"] = "./data/scripts/";

      j["content"]["packages"] = nlohmann::json::array();
      j["content"]["patchs"] = nlohmann::json::array();
      j["content"]["tasklists"] = nlohmann::json::array();
      
      j["build"]["use_toolchain"] = "???";

      j["configs"]["builder_arch"] = "???";
      j["configs"]["host_arch"] = "???";
      j["configs"]["target_arch"] = "???";
      j["configs"]["compression"] = "???";

      // Store this into toolchain.config
      std::ofstream o(new_host_path + "/host.config");
      o << std::setw(4) << j << std::endl;
      o.close();

      VortexMaker::RefreshHosts();

}
*/



/*
void CreateCreate(std::string name, std::string pathOfTarball){
    VxContext *ctx = VortexMaker::GetCurrentContext();


    std::string new_package_path = ctx->projectPath;
    new_package_path += "/.vx/data/packages/" + name;

    {
        std::string cmd = "mkdir " + new_package_path + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_package_path + "/package.config";
        system(cmd.c_str());
    }

 nlohmann::json j;
      j["package"]["name"] = name;
      j["package"]["author"] = "???";
      j["package"]["description"] = "???";
      j["package"]["label"] = "???";
      j["package"]["clear_after_process"] = false;
      j["package"]["priority"] = 1;
      j["package"]["compressed"] = "???";
      j["package"]["filename"] = "???";

      j["data"]["packages"] = "./data/packages/";
      j["data"]["patchs"] = "./data/patchs/";
      j["data"]["scripts"] = "./data/scripts/";

      j["assets"] = nlohmann::json::array();
      j["actions"] = nlohmann::json::array();

      j["parameters"]["customOutput"] = "???";
      j["parameters"]["useOnlyCustomConfigurationProcess"] = "not specified";
      j["parameters"]["useOnlyCustomCompilationProcess"] = "not specified";
      j["parameters"]["useOnlyCustomInstallationProcess"] = "not specified";
      j["parameters"]["useCompilationOptimization"] = true;


      j["compilation"]["configurationSuffixes"] = nlohmann::json::array();
      j["compilation"]["configurationPrefixes"] = nlohmann::json::array();
      j["compilation"]["compilationSuffixes"] = nlohmann::json::array();
      j["compilation"]["compilationPrefixes"] = nlohmann::json::array();
      j["compilation"]["installationSuffixes"] = nlohmann::json::array();
      j["compilation"]["installationPrefixes"] = nlohmann::json::array();
      
      j["configs"]["builder_arch"] = "???";
      j["configs"]["host_arch"] = "???";
      j["configs"]["target_arch"] = "???";
      j["configs"]["compression"] = "???";

      // Store this into toolchain.config
      std::ofstream o(new_package_path + "/package.config");
      o << std::setw(4) << j << std::endl;
      o.close();
  VortexMaker::RefreshPackages();


}
*/

/*

void VortexMaker::CreateScript(const std::string& name, const std::string& author){
    VxContext *ctx = VortexMaker::GetCurrentContext();


    std::string new_script_path = ctx->projectPath;
    new_script_path += "/.vx/data/scripts/" + name;


    {
        std::string cmd = "mkdir " + new_script_path + "/"; 
        system(cmd.c_str());
    }


    // Create package.config into the baseDir folder
    {
        std::string cmd = "mkdir " + new_script_path + "/pool"; // for all files (to open with a file browser/editor)
        system(cmd.c_str());
    }

      // Create json object and store it into  "touch " + envPath + "/toolchain.config"
      nlohmann::json j;
      j["script"]["author"] = author;
      j["script"]["description"] = "This is a script";
      j["script"]["name"] = name;

      // Store this into toolchain.config
      std::ofstream o(new_script_path + "/script.config");
      o << std::setw(4) << j << std::endl;
      o.close();

      std::ofstream mainScript(new_script_path + "/pool/main.sh");
      mainScript << std::setw(4) << "#Auto generated by VortexMaker. This is the bootsrapp of the script asset. \n #!/bin/sh" << std::endl;
      mainScript.close();

      VortexMaker::RefreshScripts();
      

    // Refresh current host
    //this->
}
*/

/*
void VortexMaker::CreateToolchain(const std::string& name, const std::string& author){
    VxContext *ctx = VortexMaker::GetCurrentContext();


    std::string new_toolchain_path = ctx->projectPath;
    new_toolchain_path += "/.vx/data/toolchains/" + name;

    std::string new_toolchain_distpath = ctx->projectPath;
    new_toolchain_distpath += "/.vx/dist/toolchains/" + name;


    // Create package.config into the baseDir folder
    {
        std::string cmd = "mkdir " + new_toolchain_path + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_toolchain_path + "/toolchain.config";
        system(cmd.c_str());
    }

    {
        std::string cmd = "mkdir " + new_toolchain_path + "/data";
        system(cmd.c_str());
    }

    {
        std::string cmd = "mkdir " + new_toolchain_path + "/data/packages";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_path + "/data/patchs";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_path + "/data/scripts";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_path + "/data/tasklists";
        system(cmd.c_str());
    }


    {
        std::string cmd = "mkdir " + new_toolchain_distpath + "/";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch " + new_toolchain_distpath + "/toolchain.dist.config";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_distpath + "/data";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir " + new_toolchain_distpath + "/snapshots";
        system(cmd.c_str());
    }

      // Create json object and store it into  "touch " + envPath + "/toolchain.config"
      nlohmann::json j;
      j["toolchain"]["author"] = author;
      j["toolchain"]["description"] = "This is a toolchain";
      j["toolchain"]["name"] = name;
      j["toolchain"]["platform"] = "???";
      j["toolchain"]["state"] = "???";
      j["toolchain"]["type"] = "toolchain";
      j["toolchain"]["vendor"] = "???";
      j["toolchain"]["version"] = "1.0.0";

      j["data"]["packages"] = "./data/packages/";
      j["data"]["patchs"] = "./data/patchs/";
      j["data"]["scripts"] = "./data/scripts/";

      j["content"]["packages"] = nlohmann::json::array();
      j["content"]["patchs"] = nlohmann::json::array();
      j["content"]["tasklists"] = nlohmann::json::array();
      
      j["configs"]["builder_arch"] = "???";
      j["configs"]["builder_platform"] = "???";
      j["configs"]["builder_vendor"] = "???";
      j["configs"]["builder_cpu"] = "???";
      j["configs"]["builder_fpu"] = "???";
      
      j["configs"]["host_arch"] = "???";
      j["configs"]["host_platform"] = "???";
      j["configs"]["host_vendor"] = "???";
      j["configs"]["host_cpu"] = "???";
      j["configs"]["host_fpu"] = "???";

      j["configs"]["target_arch"] = "???";
      j["configs"]["target_platform"] = "???";
      j["configs"]["target_vendor"] = "???";
      j["configs"]["target_cpu"] = "???";
      j["configs"]["target_fpu"] = "???";

      j["configs"]["toolchain_type"] = "???";
       
      j["configs"]["compression"] = "???";

      // Store this into toolchain.config
      std::ofstream o(new_toolchain_path + "/toolchain.config");
      o << std::setw(4) << j << std::endl;
      o.close();


      nlohmann::json dist;
      dist["configs"]["AR"] = "???";
      dist["configs"]["AS"] = "???";
      dist["configs"]["CC"] = "???";
      dist["configs"]["CXX"] = "???";
      dist["configs"]["LD"] = "???";
      dist["configs"]["RANLIB"] = "???";
      dist["configs"]["STRIP"] = "???";
    // Store this into toolchain.config
    std::ofstream disto(new_toolchain_distpath + "/toolchain.dist.config");
    disto << std::setw(4) << j << std::endl;
    disto.close();



  VortexMaker::RefreshToolchains();
      

    // Refresh current host
    //this->
}
*/



VORTEX_API void VortexMaker::DeployEvent(const std::shared_ptr<hArgs>& args, const std::string& event_name){
    VxContext &ctx = *CVortexMaker;
    for(auto em : ctx.IO.em){
      for(auto output_event : em->m_output_events){
        if(output_event->m_name == event_name){
          output_event->m_foo(args);
        }
      }
    }
  
}



VORTEX_API void VortexMaker::DeployEvent(const std::shared_ptr<hArgs>& args, const std::string& event_name, void(*callback)(std::shared_ptr<hArgs> args)){
    VxContext &ctx = *CVortexMaker;
    for(auto em : ctx.IO.em){
      for(auto output_event : em->m_output_events){
        if(output_event->m_name == event_name){
          output_event->m_foo(args);
          callback(args);
        }
      }
    }
}



VORTEX_API void VortexMaker::CallModuleEvent(const std::shared_ptr<hArgs>& args, const std::string& event_name, const std::string& module_name){
    VxContext &ctx = *CVortexMaker;
    for(auto em : ctx.IO.em){

      if(em->m_name == module_name){
      for(auto input_events : em->m_input_events){

        if(input_events->m_name == event_name){
            input_events->m_foo(args);
        }

      }
      }

    }
}


VORTEX_API void VortexMaker::CallModuleEvent(const std::shared_ptr<hArgs>& args, const std::string& event_name, const std::string& module_name, void(*callback)(std::shared_ptr<hArgs> args)){
    VxContext &ctx = *CVortexMaker;
    for(auto em : ctx.IO.em){

      if(em->m_name == module_name){
      for(auto input_events : em->m_input_events){

        if(input_events->m_name == event_name){
            input_events->m_foo(args);
            callback(args);
        }

      }
      }

    }
}



// TODO after : Module loading system, plugins loader. 


#endif // VORTEX_DISABLE