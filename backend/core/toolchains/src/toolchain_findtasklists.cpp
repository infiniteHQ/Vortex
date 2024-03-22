#include "../../../../vortex.h"
#include "../../../../vortex_internals.h"


void VxToolchain::RegisterTasklist(const std::string label)
{
  std::shared_ptr<VxTasklistInterface> newTasklistInterface = std::make_shared<VxTasklistInterface>();
  newTasklistInterface->label = label;
  registeredTasklists.push_back(newTasklistInterface);
}

void VxToolchain::FindTasklists()
{
  VxContext &ctx = *CVortexMaker;

  // Register all finded local packages
  for (const auto &file : VortexMaker::SearchFiles(ctx.toolchainsPath, "tasklist.config"))
  {
    try
    {

      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      // VxPackage newPackage;

      // newPackage.configFilePath = file;

      // Get packages infos

      for (auto registeredTasklist : this->registeredTasklists)
      {
        if (registeredTasklist->label == filecontent["label"].get<std::string>())
        {
          bool already_registered = false;
          for (auto tasklist : this->tasklists)
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

            this->tasklists.push_back(newTasklist);
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

  std::sort(this->packages.begin(), this->packages.end(), [](const std::shared_ptr<VxPackage> &a, const std::shared_ptr<VxPackage> &b)
            { return a->priority < b->priority; });

  // Register global packages
  for (auto registeredPackage : this->registeredPackages)
  {
    if (registeredPackage->emplacement == "global")
    {
      // Recupérer les packages du ctx.
    }
  }
}
