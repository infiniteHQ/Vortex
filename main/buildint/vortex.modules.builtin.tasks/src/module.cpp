#include "module.h"
#include "./instances/Report/ReportRenderInstance.h"



// TODO Task processor creation
// TODO Task launching in processor with params
// TODO Link to toolchbain


TASKS_MODULE_API void TaskModule::StopProcessor(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){
        
        const char* processor_name = args->get<const char*>("processor_name", nullptr);

        if(processor_name != nullptr){
            for(auto processor : CTasksModule->m_processors){
                if(processor_name == processor.first){
                    processor.second->stop = true;
                    return;
                }
            }
        }

    }
}


TASKS_MODULE_API void TaskModule::StartProcessor(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){
        
        const char* processor_name = args->get<const char*>("processor_name", nullptr);

        if(processor_name != nullptr){
            for(auto processor : CTasksModule->m_processors){
                if(processor_name == processor.first){
                    processor.second->stop = false;
                    return;
                }
            }
        }

    }
}

TASKS_MODULE_API void TaskModule::OpenTaskReportInstance(const std::shared_ptr<hArgs>& args){
        // Check input args
    if(args != NULL){

        VxContext* ctx = VortexMaker::GetCurrentContext();

        // Try to get pool name
        std::shared_ptr<Task> task = args->get<std::shared_ptr<Task>>("task", nullptr);

        if(task != nullptr){

            std::shared_ptr<ReportRenderInstance> instance = std::make_shared<ReportRenderInstance>(ctx, task);
            instance->name = task->id;
            CTasksModule->m_interface->AddModuleRenderInstance(instance);
        }

    }
}

/**
 * @brief Create a new task execution processor
 * @param args All arguments
 * 
 * @arg <const char*> "processor_name" : Identification of the task processor 
*/
TASKS_MODULE_API void TaskModule::CreateTaskProcessor(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){

        // Try to get pool name
        const char* pool_name = args->get<const char*>("processor_name", nullptr);

        if(pool_name != nullptr){
            std::shared_ptr<TaskProcessor> processor = std::make_shared<TaskProcessor>();
            CTasksModule->m_processors.insert({pool_name, processor});
        }
    }
}

/**
 * @brief Create a new task execution processor
 * @param args All arguments
 * @param args <const char*>("pool_name") : Name of the task pool who tasks will be stored
*/
TASKS_MODULE_API void TaskModule::CreateTaskPool(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){

        // Try to get pool name
        const char* pool_name = args->get<const char*>("pool_name", nullptr);

        // Verify is "pool_name" exist
        if(pool_name != nullptr){

            // Create a new task pool and store it in the current context.
            std::shared_ptr<TaskPool> pool = std::make_shared<TaskPool>();
            pool->m_pool_name = pool_name;
            CTasksModule->m_taskpools.push_back(pool);
        }
        else{
            // Handle error

        }
    }
}


TASKS_MODULE_API void TaskModule::GetTaskPool(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){
        const char* pool_name = args->get<const char*>("pool_name", nullptr);
        if(pool_name != nullptr){ 
            // Find pool
            for(auto pool : CTasksModule->m_taskpools){
                if(pool->m_pool_name == pool_name){
                    args->add<std::shared_ptr<TaskPool>>("taskpool", pool);
                    return;
                }
            }
        }
    }
}

/**
 * @brief Create a new task execution processor
 * @param args All arguments
*/
TASKS_MODULE_API void TaskModule::AddTaskToPool(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){

        // Get arguments
        const char* pool_name = args->get<const char*>("pool_name", nullptr);
        std::shared_ptr<Task> task = args->get<std::shared_ptr<Task>>("task", nullptr);

        // Verify arguments
        if(pool_name != nullptr && task != nullptr){

            // Find pool
            for(auto pool : CTasksModule->m_taskpools){
                if(pool->m_pool_name == pool_name){

                    // If exist, increment the finded pool with the new task
                    pool->m_list.push_back(task);
                    return;
                }
            }

            // Handler error... (pool not found)

        }
        else{
            //Handle error... (args are not valid)
        }
    }
}

/**
 * @brief Add task to process
 * @param args All arguments
*/
TASKS_MODULE_API void TaskModule::AddTaskToProcess(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){

        const char* pool_name = args->get<const char*>("pool_name", nullptr);
        const char* processor_name = args->get<const char*>("processor_name", nullptr);
        const char* task_name = args->get<const char*>("task_name", nullptr);
        std::shared_ptr<hArgs> task_props = args->get<std::shared_ptr<hArgs>>("task_props", nullptr);
        int task_priority = args->get<int>("task_priority", 0);
        std::shared_ptr<hArgs> arguments = args->get<std::shared_ptr<hArgs>>("arguments", nullptr);

        if(task_props != nullptr && processor_name != nullptr && task_name != nullptr && pool_name != nullptr && arguments != nullptr){ 
            // Find processor

            for(auto processor : CTasksModule->m_processors){

                if(processor.first == processor_name){

                // Find pool
                for(auto pool : CTasksModule->m_taskpools){

                    if(pool->m_pool_name == pool_name){

                        // Find task
                        for(auto task : pool->m_list){
                            if(task->tasktype == task_name){
                                std::shared_ptr<Task> newtask = task->clone();
                                newtask->id = newtask->tasktype + "-" + VortexMaker::gen_random(6);
                                newtask->state = "not_started";
                                newtask->props = task_props;
                                newtask->priority = task_priority;

                                {
                                    std::lock_guard<std::mutex> lock(processor.second->mutex);
                                    processor.second->tasksToProcess.push_back(newtask);
                                }
                                
                                args->add<std::shared_ptr<Task>>("task", newtask);

                                // Get task file and store it
                                //this->toolchain->currentLoadedSystem.executedTasks.push_back(selectedTask);
                                return;

                            }
                        }
                    }
                    
                }
                }
            }
        }
    }

}

/**
 * @brief Start a task processor
 * @param args All arguments
*/
TASKS_MODULE_API void TaskModule::StartTaskProcessor(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){
        const char* processor_name = args->get<const char*>("processor_name", nullptr);
        if(processor_name != nullptr){ 
            // Find pool
            for(auto processor : CTasksModule->m_processors){
                if(processor.first == processor_name){
                    processor.second->startWorker();
                    return;
                }
            }
        }
    }
}


/**
 * @brief Stop a task processor
 * @param args All arguments
*/
TASKS_MODULE_API void TaskModule::StopTaskProcessor(const std::shared_ptr<hArgs>& args){
    // Check input args
    if(args != NULL){
        const char* processor_name = args->get<const char*>("processor_name", nullptr);
        if(processor_name != nullptr){ 
            // Find pool
            for(auto processor : CTasksModule->m_processors){
                if(processor.first == processor_name){
                    processor.second->stopWorker();
                    return;
                }
            }
        }
    }
}

// TODO : systeme de taches et faire l'api public, puis faire les tasks list et retrouver tout
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

void TaskModule::CreateTasksContext(){
  TasksModuleCTX *ctx = VX_NEW(TasksModuleCTX);
  CTasksModule = ctx;
}

// Ajout d'une tâche à TaskProcessor

// Marque une tâche comme terminée
void TaskProcessor::markTaskCompleted(std::shared_ptr<Task> task)
{
  VxContext &ctx = *CVortexMaker;
  this->tasksToProcess.erase(std::remove_if(this->tasksToProcess.begin(), this->tasksToProcess.end(), [task](const auto& t) { return t == task; }), this->tasksToProcess.end());
  std::unique_lock<std::mutex> lock(mutex);
}


void TaskProcessor::processTasks() {
    VxContext &ctx = *CVortexMaker;
    this->running = true;

    while(running){

    while (!stop) {
        std::vector<std::future<void>> futures;
        std::vector<std::shared_ptr<Task>> tasks;

        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks = tasksToProcess;
            //tasksToProcess.clear();  // Clear processed tasks
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

            break;
        }
    
        // Sleep or perform other operations if needed before next iteration
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    }
}

  bool Task::ifProps(const std::vector<std::string> &propsname)
  {
    int satisfied = 0;
    int total = propsname.size();

    this->depsChecks.clear();

    for (const auto &prop : propsname)
    {
      bool propFound = false;

      std::string propStr = prop;

      for (const auto &registeredArg : this->props->registered_arguments)
      {
        std::string registeredArgStr = registeredArg.c_str();

        if (registeredArgStr == propStr)
        {
          satisfied++;
          this->depsChecks.push_back({propStr, "satisfied"});
          VortexMaker::LogInfo("Core", "New prop found, state: " + std::to_string(satisfied) + "/" + std::to_string(total) + " props found.");
          propFound = true;
          break;
        }
      }

      if (!propFound)
      {
        this->depsChecks.push_back({propStr, "missing"});
      }
    }

    VortexMaker::LogInfo("Core", "Added " + std::to_string(total) + " props to check.");

    // Vérifier si toutes les propriétés sont satisfaites
    return (satisfied == total);
  }

  bool Task::ifProp(std::string propname)
  {
    for (auto prop : this->props->registered_arguments)
    {
      if (prop.c_str() == propname)
      {
        return true;
      }
    }
    return false;
  }


 double Task::elapsedMilliseconds()
  {
    std::chrono::time_point<std::chrono::system_clock> endTime;
    if (m_bRunning)
    {
      endTime = std::chrono::system_clock::now();
    }
    else
    {
      endTime = m_EndTime;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
  }

  double Task::elapsedSeconds() { return elapsedMilliseconds() / 1000.0; }



  std::string Task::startTime()
  {
    std::time_t t = std::chrono::system_clock::to_time_t(this->m_StartTime);
    std::string ts = std::ctime(&t);
    ts.resize(ts.size() - 1);
    return ts;
  }

  void Task::start()
  {
    this->m_StartTime = std::chrono::system_clock::now();
    this->state = "process";
    this->m_bRunning = true;
  }

  void Task::stop()
  {
    this->m_EndTime = std::chrono::system_clock::now();
    this->m_bRunning = false;
    this->m_TotalTime = elapsedSeconds();
  }



  void Task::addCheckVerdict(std::string id, std::string result, std::string log, std::string directive)
  {
    for (auto check : this->checkList)
    {
      if (check->checkID == id)
      {
        check->checkLog = log;
        check->checkResult = result;
        check->checkDirective = directive;

        if (check->checkResult == "failed")
        {
          this->failCounter++;
          this->unknowCounter--;
        }
        else if (check->checkResult == "success")
        {
          this->successCounter++;
          this->unknowCounter--;
        }
        else if (check->checkResult == "warning")
        {
          this->warningCounter++;
          this->unknowCounter--;
        }
        else if (check->checkResult == "unknow")
        {
        }
        this->updateState();
      }
    }
  }


  void Task::addIdleCheck(std::string id)
  {
    std::shared_ptr<Check> newCheck = std::make_shared<Check>();
    newCheck->checkID = id;
    this->checkList.push_back(newCheck);
    this->unknowCounter++;
  }

  void Task::updateState()
  {
    std::string _state = "unknow";
    if (this->successCounter > 0)
    {
      _state = "success";
    }
    else if (this->warningCounter > 0)
    {
      _state = "warning";
    }
    else if (this->failCounter > 0)
    {
      _state = "failed";
    }
    this->state = _state;
  }

// MODULE FUNCTION