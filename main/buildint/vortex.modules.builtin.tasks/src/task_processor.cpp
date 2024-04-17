#include "task_processor.h"

// Constructeur de TaskModuleProcessor
TaskModuleProcessor::TaskModuleProcessor() : stop(false)
{
}

// Constructeur de TaskModuleProcessor
TaskModuleProcessor::~TaskModuleProcessor()
{
  
}

void TaskModuleProcessor::startWorker(){
  std::thread Thread([&]()
                     { this->processTasks(); });
  worker.swap(Thread);
}

void TaskModuleProcessor::stopWorker(){
  this->stop = true;
}


// Ajout d'une tâche à TaskModuleProcessor

// Marque une tâche comme terminée
void TaskModuleProcessor::markTaskCompleted(std::shared_ptr<Task> task)
{
  this->tasksToProcess.erase(std::remove_if(this->tasksToProcess.begin(), this->tasksToProcess.end(), [task](const auto& t) { return t == task; }), this->tasksToProcess.end());
  std::unique_lock<std::mutex> lock(mutex);
}