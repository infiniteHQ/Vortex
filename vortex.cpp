// The VortexMaker Project, version 1.0
// [Main sources & docs]

// Index of this file:

//  [Includes]
//  [Context Utility: RegisterProvider] => Register a Provider into the context

//-------------------------------------------------------------------------
// [INCLUDES]
//-------------------------------------------------------------------------

#include "vortex.h"
#ifndef VORTEX_DISABLE
#include "vortex_internals.h"

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

//-----------------------------------------------------------------------------
// [Context Function: CreateContext] => Create a context for a VortexMaker use
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

VORTEX_API VxContext *VortexMaker::CreateContext()
{
  /*
  VxContext *context = new VxContext();
  // Vx_NEW
  // malloc(sizeof(context));
  SetCurrentContext(context);
  return context;
  */

  VxContext *prev_ctx = GetCurrentContext();
  VxContext *ctx = VX_NEW(VxContext);
  SetCurrentContext(ctx);
  Initialize();
  if (prev_ctx != NULL)
    SetCurrentContext(
        prev_ctx); // Restore previous context if any, else keep new one.

  return ctx;
}

//-----------------------------------------------------------------------------
// [Context Function: SetCurrentContext] => Set the main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void VortexMaker::SetCurrentContext(VxContext *ctx)
{
#ifdef USE_CURRENT_CONTEXT_FUNC
  USE_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-based hackery you may want
                                 // to have control over this.
#else
  CVortexMaker = ctx;
#endif
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
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VORTEX_API void VortexMaker::DestroyContext(VxContext *ctx)
{
  VxContext *prev_ctx = GetCurrentContext();
  if (ctx == NULL) //-V1051
    ctx = prev_ctx;
  SetCurrentContext(ctx);
  SetCurrentContext((prev_ctx != ctx) ? prev_ctx : NULL);
  VX_DELETE(ctx);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [Context Function: GetCurrentContext] => Get the current main context
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
VORTEX_API VxContext *VortexMaker::GetCurrentContext() { return CVortexMaker; }
//-----------------------------------------------------------------------------

VORTEX_API void VortexMaker::Initialize()
{
  VxContext &ctx = *CVortexMaker;

  ctx.taskFactory = &TaskFactory::getInstance();

  // Set initialized flags
  ctx.initialized = true;
}

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

std::string SearchFilesRecursive(const fs::path &chemin, const std::string &filename, std::vector<std::string> &file)
{
  for (const auto &entry : fs::directory_iterator(chemin))
  {
    if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos)
    {
      file.push_back(entry.path().string());
      return entry.path().string();
    }
    else if (entry.is_directory())
    {
      SearchFilesRecursive(entry.path(), filename, file);
    }
  }
  return "null";
}

VORTEX_API std::vector<std::string> VortexMaker::SearchFiles(const std::string &path, const std::string &filename)
{
  std::vector<std::string> fichiersTest;
  SearchFilesRecursive(fs::current_path() / path, filename, fichiersTest);
  return fichiersTest;
}

VORTEX_API std::string VortexMaker::gen_random(int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
    {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

VORTEX_API nlohmann::json VortexMaker::DumpJSON(const std::string &file)
{
  std::ifstream fichier(file);

  if (!fichier.is_open())
  {
    throw std::runtime_error("Error while opening file " + file);
  }

  nlohmann::json json_data;
  fichier >> json_data;

  return json_data;
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


 void VortexMaker::CreateProject(std::string name, std::string path){
  
  std::string projectPath;

  // Creating main project folder
  {
    projectPath = path + "/" + name + "/";
    std::string cmd = "mkdir "  +projectPath;
    system(cmd.c_str());
  }

  {
    std::string cmd = "mkdir "  + projectPath + "/.vx";
    system(cmd.c_str());
  }
  {
    std::string cmd = "touch "  + projectPath + "/vortex.config";
    system(cmd.c_str());
  }
   nlohmann::json j;
      j["project"]["author"] = "unknow";
      j["project"]["description"] = "This is a toolchain";
      j["project"]["name"] = name;
      j["project"]["type"] = "???";
      j["project"]["version"] = "1.0.0";

      j["data"]["toolchains"] = "./.vx/data/toolchains/";
      j["data"]["hosts"] = "./.vx/data/hosts/";
      j["data"]["gpos"] = "./.vx/data/gpos/";
      j["data"]["packages"] = "./.vx/data/packages/";

      j["dist"]["toolchains"] = "./.vx/dist/toolchains/";
      j["dist"]["gpos"] = "./.vx/dist/gpos/";
      j["dist"]["packages"] = "./.vx/dist/packages/";
      j["dist"]["hosts"] = "./.vx/dist/hosts/";

      // Store this into toolchain.config
      std::ofstream o(projectPath + "/vortex.config");
      o << std::setw(4) << j << std::endl;
      o.close();

  // Data
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/hosts";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/kernels";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/libs";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/gpos";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/packages";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/skeletons";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/data/toolchains";
    system(cmd.c_str());
  }


  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/config";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/hosts";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/gpos";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/kernels";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/libs";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/packages";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/skeletons";
    system(cmd.c_str());
  }
  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/dist/toolchains";
    system(cmd.c_str());
  }

  {
    std::string cmd = "mkdir "  + projectPath + "/.vx/temp";
    system(cmd.c_str());
  }


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

VORTEX_API void VortexMaker::InitProject(nlohmann::json main_configs)
{
  VxContext &ctx = *CVortexMaker;

  ctx.author = main_configs["project"]["author"].get<std::string>();
  ctx.description = main_configs["project"]["description"].get<std::string>();
  ctx.label = main_configs["project"]["name"].get<std::string>();
  ctx.name = main_configs["project"]["name"].get<std::string>();
  ctx.type = main_configs["project"]["type"].get<std::string>();
  ctx.version = main_configs["project"]["version"].get<std::string>();

  ctx.packagesPath = main_configs["data"]["packages"].get<std::string>();
  ctx.toolchainsPath = main_configs["data"]["toolchains"].get<std::string>();
  ctx.gposPath = main_configs["data"]["gpos"].get<std::string>();
  ctx.hostsPath = main_configs["data"]["hosts"].get<std::string>();

  ctx.paths.toolchainDistFolder = main_configs["dist"]["toolchains"].get<std::string>();
  ctx.paths.hostDistFolder = main_configs["dist"]["hosts"].get<std::string>();

  ctx.projectPath = fs::current_path();

  VortexMaker::RefreshPackages();
  VortexMaker::RefreshGpos();
  VortexMaker::RefreshHosts();
  VortexMaker::RefreshToolchains();
  VortexMaker::RefreshDistToolchains();
  VortexMaker::RefreshDistHosts();
}

// Correction de la fonction CreateTask
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
}

VORTEX_API void VortexMaker::CreateNewTask(std::shared_ptr<Task> task, std::string tasktype, std::string uniqueID, int priority, std::shared_ptr<hArgs> props)
{
}

// Constructeur de TaskProcessor
TaskProcessor::TaskProcessor() : stop(false)
{
}

// Constructeur de TaskProcessor
TaskProcessor::~TaskProcessor()
{
  
}

void TaskProcessor::startWorker(){
  std::thread Thread([&]()
                     { this->processTasks(); });
  worker.swap(Thread);
}

void TaskProcessor::stopWorker(){
  this->stop = true;
}


// Ajout d'une tâche à TaskProcessor

// Marque une tâche comme terminée
void TaskProcessor::markTaskCompleted(std::shared_ptr<Task> task)
{
  VxContext &ctx = *CVortexMaker;
  this->tasksToProcess.erase(std::remove_if(this->tasksToProcess.begin(), this->tasksToProcess.end(), [task](const auto& t) { return t == task; }), this->tasksToProcess.end());
  std::unique_lock<std::mutex> lock(mutex);
}

void VortexMaker::DeleteHost(std::shared_ptr<VxHost> host){
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

}



void VortexMaker::DeleteGpos(std::shared_ptr<VxGPOSystem> gpos){
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




void VortexMaker::CreateGpos(std::string name, std::string author){
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


void VortexMaker::DeleteToolchain(std::shared_ptr<VxToolchain> toolchain){
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


void VortexMaker::CreateHost(std::string name, std::string author){
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

void VortexMaker::CreateToolchain(std::string name, std::string author){
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
      j["configs"]["host_arch"] = "???";
      j["configs"]["target_arch"] = "???";
      j["configs"]["compression"] = "???";

      // Store this into toolchain.config
      std::ofstream o(new_toolchain_path + "/toolchain.config");
      o << std::setw(4) << j << std::endl;
      o.close();
  VortexMaker::RefreshToolchains();
      

    // Refresh current host
    //this->
}




#include <deque>
#include <mutex>

void TaskProcessor::processTasks() {
    VxContext &ctx = *CVortexMaker;

    while (!stop) {
        std::vector<std::future<void>> futures;
        std::vector<std::shared_ptr<Task>> tasks;

        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks = tasksToProcess;
            tasksToProcess.clear();  // Clear processed tasks
        }

        // Group tasks by priority
        std::map<int, std::vector<std::shared_ptr<Task>>> tasksByPriority;
        for (const auto &task : tasks) {
            tasksByPriority[task->priority].push_back(task);
        }

        // Iterate over priorities, from lowest to highest
        for (const auto &[priority, tasksWithPriority] : tasksByPriority) {
            // Execute tasks with the same priority simultaneously
            std::vector<std::future<void>> priorityFutures;
            for (const auto &task : tasksWithPriority) {
                priorityFutures.emplace_back(std::async(std::launch::async, [this, task]() {
                    if (task->state == "not_started" || task->state == "retry") {
                        task->state = "process";
                        task->exec();
                        markTaskCompleted(task);
                    }
                }));
            }

            // Wait for all tasks of the same priority to complete
            for (auto &future : priorityFutures) {
                future.get();
            }
        }

        // Sleep or perform other operations if needed before next iteration
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


#endif // VORTEX_DISABLE