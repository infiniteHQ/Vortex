#include "module.h"
#include "instances/tasklistInstance/TasklistRenderInstance.h"

/**
 * @brief Launch tasklist interface
 * @param ["args"] "tasklist" The target tasklist we wan't to open in a interface. (type: std::shared_ptr<Tasklist>)
*/
void TasklistModule::LaunchTasklistInterface(const std::shared_ptr<hArgs>& args)
{
    if (args != NULL)
    {
        std::shared_ptr<TaskList> tasklist = args->get<std::shared_ptr<TaskList>>("tasklist", nullptr);
        std::shared_ptr<Toolchain> toolchain = args->get<std::shared_ptr<Toolchain>>("toolchain", nullptr);


        if (tasklist != nullptr)
        {
            if(toolchain != nullptr)
            {
            VxContext *ctx = VortexMaker::GetCurrentContext();
            std::shared_ptr<TasklistRenderInstance> instance = std::make_shared<TasklistRenderInstance>(ctx, tasklist, toolchain);
            instance->name = tasklist->label;
            CTasklistsModule->m_interface->AddModuleRenderInstance(instance);

            }
        }
        else{
        }
    }
    else{
    }
}

/**
 * @brief Find packages from a registered package interface list
 * @param ["args"] "list" The target package we wan't to open in a interface. (type: std::vector<PackageInterface>)
*/
void TasklistModule::FindTasklists(const std::shared_ptr<hArgs>& args){
    VxContext &ctx = *CVortexMaker;

    if(args == nullptr)
    return;

    std::string path = args->get<std::string>("path", "null");


    std::vector<std::shared_ptr<TaskList>> tasklists_list;
    tasklists_list = args->get<std::vector<std::shared_ptr<TaskList>>>("tasklists", tasklists_list);

    std::vector<std::shared_ptr<TasklistInterface>> interfaces_list;
    interfaces_list = args->get<std::vector<std::shared_ptr<TasklistInterface>>>("list", interfaces_list);

if(path != "null")
{

  // Register all finded local packages
  for (const auto &file : VortexMaker::SearchFiles(path, "tasklist.config"))
  {
    try
    {

      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      // VxPackage newPackage;

      // newPackage.configFilePath = file;

      // Get packages infos

      for (auto registeredTasklist : interfaces_list)
      {
        if (registeredTasklist->label == filecontent["label"].get<std::string>())
        {
          bool already_registered = false;
          for (auto tasklist : tasklists_list)
          {
            if (filecontent["label"].get<std::string>() == tasklist->label)
            {
              already_registered = true;
            }
          }

          if (!already_registered)
          {
            std::shared_ptr<TaskList> newTasklist = std::make_shared<TaskList>();

              newTasklist->configFilePath = file;

            newTasklist->label = filecontent["label"].get<std::string>();

            //this->tasks.clear();
            nlohmann::json tasks = filecontent["tasks"];
            for (auto &t : tasks)
            {
              std::shared_ptr<Task> task = std::make_shared<Task>();
              task->tasktype = t["task"].get<std::string>();
              task->component = t["component"].get<std::string>();

              for(auto env_props : t["env_props"]){
                task->env_props.push_back({env_props["type"].get<std::string>(), env_props["prop"].get<std::string>()});
              }

              task->priority = t["priority"].get<int>();

              newTasklist->list.push_back(task);
            }

            tasklists_list.push_back(newTasklist);
            registeredTasklist->resolved = true;
          }
        }
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}
}



void TaskList::Refresh()
{
  nlohmann::json filecontent = VortexMaker::DumpJSON(this->configFilePath);

  this->label = filecontent["label"].get<std::string>();

  this->list.clear();
  nlohmann::json tasks = filecontent["tasks"];
  for (auto &t : tasks)
  {
    std::shared_ptr<Task> task = std::make_shared<Task>();
    task->tasktype = t["task"].get<std::string>();
    task->component = t["component"].get<std::string>();
    task->priority = t["priority"].get<int>();

    for (auto env_props : t["env_props"])
    {
      task->env_props.push_back({env_props["type"].get<std::string>(), env_props["prop"].get<std::string>()});
    }

    this->list.push_back(task);
  }
}

// TODO : Fix TL interface, finish all tasks systems

void TaskList::PushSave(std::shared_ptr<TaskListSave> save)
{
    nlohmann::json data;

    data["label"] = save->label;
    data["tasks"] = nlohmann::json::array();

    for (auto task : save->list)
    {
        nlohmann::json t;
        t["component"] = task.component;
        t["priority"] = task.priority;
        t["task"] = task.task;
        t["env_props"] = nlohmann::json::array();

        for(auto env_prop : task.env_props){
            nlohmann::json env;
            env["type"] = env_prop.first;
            env["prop"] = env_prop.second;
            t["env_props"].push_back(env);
        }

        data["tasks"].push_back(t);
    }

    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << data << std::endl;

          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " +  this->configFilePath + " for writing!");
    }
}
// MODULE FUNCTION