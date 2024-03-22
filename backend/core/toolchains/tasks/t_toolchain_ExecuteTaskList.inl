// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"

struct Task;

struct ExecuteTaskList : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<ExecuteTaskList>(*this);
  }

  void init() override
  {
    this->tasktype = "ExecuteTaskList";

    

    // Props used by task execution
    this->neededProps.push_back("package");
    this->neededProps.push_back("toolchain");
    this->neededProps.push_back("tasklist");



    this->addIdleCheck("create_build_folder");
    this->addIdleCheck("configure_dist_package");
    this->addIdleCheck("compile_dist_package");
    this->addIdleCheck("install_dist_package");
  };


  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();


    if(!this->ifProps(this->neededProps)){this->finish("fatal", nullptr);}

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
    std::shared_ptr<TaskList> tasklist = this->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr);
    std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);


            for (auto task : tasklist->list)
{
    for(auto runtime_tasks : toolchain->tasks)
    {
        if(runtime_tasks->tasktype == task->tasktype)
        {
            std::shared_ptr<Task> _task = runtime_tasks->clone();
            //_task = task;
            std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
            props->add("toolchain", toolchain);
            props->add("tasklist", tasklist);
            props->add("package", package);
            //props->add("package", toolchain->packages[row]);

            _task->id = runtime_tasks->tasktype + "-" + VortexMaker::gen_random(8);
            _task->tasktype = runtime_tasks->tasktype;
            _task->component = task->component;
            _task->priority = task->priority;
            _task->props = props;
            _task->state = "not_started";

            // Ajout de la tâche aux listes appropriées
            if (toolchain->taskProcessor)
            {
                //toolchain->taskProcessor->tasksToProcess.push_back(_task);
                //toolchain->taskProcessor->tasksToProcess.push_back(_task);
                toolchain->currentLoadedSystem.executedTasks.push_back(_task);
                //toolchain->packages[row]->latestTask = _task;
                toolchain->currentLoadedSystem.Save(toolchain);

                {
                    std::lock_guard<std::mutex> lock(toolchain->taskProcessor->mutex);
                    toolchain->taskProcessor->tasksToProcess.push_back(_task);
                }
            }
            else
            {
                std::cout << "Failed while accessing taskToProcess" << std::endl;
            }
        }
    }
}








    this->finish("finish", nullptr);
  }

  void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) override
  {
    // Get time
    // Get timer
    // Get all...
    this->stop();
    this->result_props = result_properties;

    this->state = "unknow";
    if (this->successCounter > 0)
    {
      this->state = "success";
    }

    if (this->warningCounter > 0)
    {
      this->state = "warning";
    }

    if (this->failCounter > 0)
    {
      this->state = "failed";
    }

    if (this->props)
    {
      std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
