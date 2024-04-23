// Toolchain task API
// Task : buildPackage
// Date : 03/11/2024
// Author : Diego Moreno

/*
    Description :
*/


/// TODO : Finish toolchain, Libs, Deps handling, install of modules, advances logs. VIDEOS

static std::vector<std::shared_ptr<Task>> taskpool;
static void refreshTaskList(const char *poolname)
{
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("pool_name", poolname);
    VortexMaker::CallModuleEvent(args, "GetTaskPool", "vortex.modules.builtin.tasks");
    std::shared_ptr<TaskPool> list = args->get<std::shared_ptr<TaskPool>>("taskpool", nullptr);

    if (list != nullptr)
    {
        taskpool = list->m_list;
    }
}


#include "../toolchain.h"

struct Task;

struct ExecuteTasklist : public Task
{
  std::shared_ptr<Task> clone() const override
  {
    return std::make_shared<ExecuteTasklist>(*this);
  }

  void init() override
  {
    this->tasktype = "ExecuteTasklist";

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

    std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
    std::shared_ptr<TaskList> tasklist = this->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr);



    if (toolchain == nullptr)
    {
      this->finish("fatal", nullptr);
    }
    // Import env_props // Lower priority than props
    // Component can be attached in a dedicated component space for more simplicity

    // std::shared_ptr<VxPackage> package = this->props->get<std::shared_ptr<VxPackage>>("package", nullptr);

    // Get All env prop and load it

    this->state = "processing";
    std::vector<std::string> taskids;


    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("processor_name", toolchain->pool_name);
    VortexMaker::CallModuleEvent(args, "StopProcessor", "vortex.modules.builtin.tasks");

    refreshTaskList(toolchain->pool_name);

      std::shared_ptr<TaskList> prop_tasklist = this->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr);
    for (auto runtime_tasks : prop_tasklist->list) // !
    {

      for (auto task : taskpool)
      {
        if (runtime_tasks->tasktype == task->tasktype)
        {
          {
            std::shared_ptr<hArgs> arguments = std::make_shared<hArgs>();
            arguments->add("pool_name", toolchain->pool_name);
            arguments->add("processor_name", toolchain->pool_name);
            arguments->add("task_name", runtime_tasks->tasktype.c_str());
            arguments->add("task_priority", runtime_tasks->priority);
            arguments->add("task_props", this->props);

            std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
            props->add("toolchain", toolchain);
            props->add("tasklist", tasklist);
            arguments->add("arguments", props);
            // props->add("package", package);
            // props->add("package", toolchain->packages[row]);

            /*_task->id = runtime_tasks->tasktype + "-" + VortexMaker::gen_random(8);
            taskids.push_back(_task->id);

            _task->tasktype = runtime_tasks->tasktype;
            _task->component = task->component;
            _task->priority = task->priority;*/

            bool packagePropAdded = false;
            bool toolchainPropAdded = false;

            spdlog::critical(env_props.size());
            for (auto env_prop : env_props)
            {
              spdlog::critical(env_prop.first);
            }

            for (auto env_prop : env_props)
            {
              if (env_prop.first == "package")
              {
                for (auto package : toolchain->packages)
                {
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
                for (auto toolchain : CToolchainModule->m_toolchains)
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

            //props->add<std::shared_ptr<Toolchain>>("toolchain", toolchain);
            arguments->add("arguments", props);

            VortexMaker::CallModuleEvent(arguments, "AddTaskToProcess", "vortex.modules.builtin.tasks");

            std::shared_ptr<Task> newtask = arguments->get<std::shared_ptr<Task>>("task", nullptr);
            if (newtask != nullptr)
            {
              toolchain->currentLoadedSystem.executedTasks.push_back(newtask);
              toolchain->currentLoadedSystem.parent = toolchain;
            }
            else
            {
            }
          }
        }
      }
    }

              // this->toolchain->packages[row]->latestTask = _task;
    toolchain->currentLoadedSystem.Save(toolchain);
    VortexMaker::CallModuleEvent(args, "StartProcessor", "vortex.modules.builtin.tasks");

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
      std::shared_ptr<Toolchain> toolchain = this->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);
      toolchain->currentLoadedSystem.Save(toolchain);
    }
    else
    {
      std::cout << "Error: Props is null" << std::endl;
    }
  }
};
