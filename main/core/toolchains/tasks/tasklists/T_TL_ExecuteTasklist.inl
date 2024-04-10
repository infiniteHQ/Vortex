// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/

#include "../../../../include/vortex.h"
#include "../../../../include/vortex_internals.h"

struct Task;

struct T_TL_ExecuteTasklist : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<T_TL_ExecuteTasklist>(*this);
  }

  void init() override
  {
    this->tasktype = "T_TL_ExecuteTasklist";

    // Props used by task execution
    // this->neededProps.push_back("package");
    this->neededProps.push_back("toolchain");
    this->neededProps.push_back("tasklist");

    this->addIdleCheck("finished");
  };

  // Récupérer un ancien report
  void retry() override{};

  void exec() override
  {
    this->start();
    VxContext *ctx = VortexMaker::GetCurrentContext();

    if (!this->ifProps(this->neededProps))
    {
      this->finish("fatal", nullptr);
    }

    std::vector<std::pair<std::string, std::string>> env_props = this->props->get<std::vector<std::pair<std::string, std::string>>>("env_props", {{"null", "null"}});

    std::shared_ptr<VxToolchain> toolchain = this->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr);
    std::shared_ptr<TaskList> tasklist = this->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr);

    // Import env_props // Lower priority than props
    // Component can be attached in a dedicated component space for more simplicity


    // std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);

    // Get All env prop and load it

    this->state = "processing";
    std::vector<std::string> taskids;

    toolchain->taskProcessor->stop = true;
    for (auto task : tasklist->list)
    {
      for (auto runtime_tasks : toolchain->tasks)
      {
        if (runtime_tasks->tasktype == task->tasktype)
        {
          std::shared_ptr<Task> _task = runtime_tasks->clone();
          //_task = task;
          std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
          props->add("toolchain", toolchain);
          props->add("tasklist", tasklist);
          _task->env_props = env_props;
          // props->add("package", package);
          // props->add("package", toolchain->packages[row]);

          _task->id = runtime_tasks->tasktype + "-" + VortexMaker::gen_random(8);
          taskids.push_back(_task->id);

          _task->tasktype = runtime_tasks->tasktype;
          _task->component = task->component;
          _task->priority = task->priority;

          bool packagePropAdded = false;
          bool toolchainPropAdded = false;

            spdlog::critical(env_props.size());
          for (auto env_prop : env_props)
          {
            spdlog::critical(env_prop.first);
          }

          for (auto env_prop : env_props)
          {
            std::cout << "Processing " << env_prop.first << " " << env_prop.second << std::endl;
            if (env_prop.first == "package")
            {
              for (auto package : toolchain->packages)
              {
                std::cout << "Package name " << package->name << " " << env_prop.second << std::endl;
                if (package->name == env_prop.second)
                {
                  spdlog::info("Add package");
                  props->add("package", package);
                  spdlog::info("Add package");
                  packagePropAdded = true;
                }
              }
            }
            else if (env_prop.first == "toolchain")
            {
              for (auto toolchain : ctx->IO.toolchains)
              {
                if (toolchain->name == env_prop.second)
                {
                  // props->remove("toolchain");
                  spdlog::info("Add toolchain");
                  props->add("toolchain", toolchain);
                  spdlog::info("Add toolchain");
                  toolchainPropAdded = true;
                }
              }
            }
          }


          _task->props = props;
          _task->state = "not_started";

          // Ajout de la tâche aux listes appropriées
          if (toolchain->taskProcessor)
          {
            // toolchain->taskProcessor->tasksToProcess.push_back(_task);
            // toolchain->taskProcessor->tasksToProcess.push_back(_task);
            toolchain->currentLoadedSystem.executedTasks.push_back(_task);
            // toolchain->packages[row]->latestTask = _task;
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

    toolchain->taskProcessor->stop = false;


    this->addCheckVerdict("finished", "success", "none", "none");
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
