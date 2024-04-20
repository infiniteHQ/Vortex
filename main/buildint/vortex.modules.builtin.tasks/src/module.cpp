#include "module.h"



TASKS_MODULE_API void TaskModule::RegisterTask(const std::shared_ptr<hArgs>& args){
    if(args != NULL){
        std::vector<std::shared_ptr<Task>>* task_array = args->get<std::vector<std::shared_ptr<Task>>*>("taskarray", nullptr);
        std::shared_ptr<Task> task = args->get<std::shared_ptr<Task>>("task", nullptr);

        std::cout << "TASKARRAY" << task_array << std::endl;

        if(task != NULL && task_array != NULL){
            task_array->push_back(task);
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


// MODULE FUNCTION