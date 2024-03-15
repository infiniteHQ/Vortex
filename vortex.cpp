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
  ctx.label = main_configs["project"]["label"].get<std::string>();
  ctx.name = main_configs["project"]["name"].get<std::string>();
  ctx.type = main_configs["project"]["type"].get<std::string>();
  ctx.version = main_configs["project"]["version"].get<std::string>();

  ctx.packagesPath = main_configs["data"]["packages"].get<std::string>();
  ctx.toolchainsPath = main_configs["data"]["toolchains"].get<std::string>();
  ctx.gposPath = main_configs["data"]["gpos"].get<std::string>();
  ctx.hostsPath = main_configs["data"]["hosts"].get<std::string>();

  ctx.paths.toolchainDistFolder = main_configs["dists"]["toolchains"].get<std::string>();
  ctx.paths.hostDistFolder = main_configs["dists"]["hosts"].get<std::string>();

  ctx.projectPath = fs::current_path();

  // Packages
  /*
  for (const auto &file : SearchFiles(ctx.packagesPath, "package.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      RegisterPackage(filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }*/

  // Toolchains
  for (const auto &file : SearchFiles(ctx.toolchainsPath, "toolchain.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxToolchain> toolchain = std::make_shared<VxToolchain>();

      toolchain->configFilePath = file;
      toolchain->path = file;

      size_t position = toolchain->path.find("/toolchain.config");
      if (position != std::string::npos)
      {
        toolchain->path.erase(position, 17);
      }


      RegisterToolchain(toolchain, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }

  // Hosts
  for (const auto &file : SearchFiles(ctx.hostsPath, "host.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxHost> host = std::make_shared<VxHost>();

      host->configFilePath = file;

      RegisterHost(host, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }

  // Hosts
  for (const auto &file : SearchFiles(ctx.gposPath, "gpos.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);
      std::shared_ptr<VxGPOSystem> gpos = std::make_shared<VxGPOSystem>();

      gpos->configFilePath = file;

      RegisterGPOS(gpos, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }

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
  for (auto action : this->actions)
  {
    if (sequence == action->executionSequence)
    {
      if (action->type == "command")
      {

        std::string cmd = "";
        cmd += "cd " + package->distPath + "/" + package->path + "/build && " + action->command;

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