#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"


#ifndef __TASKLISTS_MODULE_H__
#define __TASKLISTS_MODULE_H__

class TasklistInterface;
class TaskList_;

struct ModuleCTX
{
  std::vector<std::shared_ptr<TasklistInterface>>  m_registered_tasklists;    
  std::vector<std::shared_ptr<TaskList_>>  m_tasklists;    
  std::shared_ptr<ModuleInterface>        m_interface;
};

#ifndef TASKLIST_MODULE_API
#define TASKLIST_MODULE_API
#endif

#ifndef CTasklistsModule
extern TASKLIST_MODULE_API ModuleCTX *CTasklistsModule; // Current implicit context pointer
#endif

namespace TasklistModule{
    //TASKLIST_MODULE_API bool RegisterPackage(std::string filepath, std::shared_ptr<Package> newPackage, nlohmann::json filecontent);
}



struct TasklistInterface{
    std::string label;
    std::string emplacement;
    bool resolved;
};

struct TaskSave_{
    char task[128];
    char component[128];

    //        type,     component       example : package, P_GCC-12.0
    std::vector<std::pair<char[128],char[128]>> env_props;


    int priority = - 1;

};


struct TaskList_Save {
    char label[128] = "unknow";

    std::vector<TaskSave_> list;

};


struct TaskList_{
    std::string configFilePath;
    std::string label;
    std::vector<std::shared_ptr<Task>> list;

    std::shared_ptr<hArgs> props;

    void Refresh();
    void PushSave(std::shared_ptr<TaskList_Save> save);
};


/**
 * @brief Register global tasklists
*/
static void RegisterTasklist()
{
  VxContext &ctx = *CVortexMaker;

  // Register all finded local packages
  for (const auto &file : VortexMaker::SearchFiles(CTasklistsModule->m_interface->m_datapath, "tasklist.config"))
  {
    try
    {

      nlohmann::json filecontent = VortexMaker::DumpJSON(file);
      // VxPackage newPackage;

      // newPackage.configFilePath = file;

      // Get packages infos

      for (auto registeredTasklist : CTasklistsModule->m_registered_tasklists)
      {
        if (registeredTasklist->label == filecontent["label"].get<std::string>())
        {
          bool already_registered = false;
          for (auto tasklist : CTasklistsModule->m_tasklists)
          {
            if (filecontent["label"].get<std::string>() == tasklist->label)
            {
              already_registered = true;
            }
          }

          if (!already_registered)
          {
            std::shared_ptr<TaskList_> newTasklist = std::make_shared<TaskList_>();

              newTasklist->configFilePath = file;

            newTasklist->label = filecontent["label"].get<std::string>();

            //this->tasks.clear();
            nlohmann::json tasks = filecontent["tasks"];
            for (auto &t : tasks)
            {
              std::shared_ptr<Task> task = std::make_shared<Task>();
              task->tasktype = t["task"].get<std::string>();
              //task->component = t["component"].get<std::string>();
              task->priority = t["priority"].get<int>();
              newTasklist->list.push_back(task);
            }

            CTasklistsModule->m_tasklists.push_back(newTasklist);
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

 // std::sort(CTasklistsModule->m_tasklists.begin(), CTasklistsModule->m_tasklists.end(), [](const std::shared_ptr<TaskList_> &a, const std::shared_ptr<TaskList_> &b)
 //           { return a->p < b->priority; });

  // Register global packages
  for (auto registeredPackage : CTasklistsModule->m_registered_tasklists)
  {
    if (registeredPackage->emplacement == "global")
    {
        // Recupérer les packages du ctx.
    }
  }
}



#endif // __TASKLISTS_MODULE_H__