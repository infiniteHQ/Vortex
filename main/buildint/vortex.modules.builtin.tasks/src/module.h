#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"
#include <queue>


#ifndef __TASKS__MODULE_H__
#define __TASKS__MODULE_H__

class TaskProcessor;
class TaskPool;
class Task;

struct TasksModuleCTX
{
  std::shared_ptr<ModuleInterface> m_interface;
  std::unordered_map<const char*, std::shared_ptr<TaskProcessor>>   m_processors;
  std::vector<std::shared_ptr<TaskPool>> m_taskpools;
};

#ifndef TASKS_MODULE_API
#define TASKS_MODULE_API
#endif

#ifndef CTasksModule
extern TASKS_MODULE_API TasksModuleCTX *CTasksModule; // Current implicit context pointer
#endif

namespace TaskModule
{
  TASKS_MODULE_API void CreateTasksContext();
  TASKS_MODULE_API void CreateTaskProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void StopProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void StartProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void CreateTaskPool(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void OpenTaskReportInstance(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void AddTaskToPool(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void AddTaskToProcess(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void StartTaskProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void StopTaskProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void GetTaskPool(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void StartTaskProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void StopTaskProcessor(const std::shared_ptr<hArgs>& args);
  TASKS_MODULE_API void FindPackages(const std::shared_ptr<hArgs>& args);
}

struct TaskPool
{  
  std::vector<std::shared_ptr<Task>> m_list;
  const char* m_pool_name;
};

struct Check{
    std::string checkID;
    std::string checkResult = "unknow";
    std::string checkLog;
    std::string checkDirective;
};


struct Task
{

  virtual std::shared_ptr<Task> clone() const
  {
    return std::make_shared<Task>(*this);
  }

  std::string id; // to find this task from everywhere
  std::string tasktype = "unknow";

  std::vector<std::string> neededVariables; // Only data view

  std::vector<std::string> neededProps;                       // Only data view
  std::vector<std::pair<std::string, std::string>> env_props; // To resolve and store in " std::shared_ptr<hArgs> props"

  std::string component;
  std::string state; // state of this task
  std::vector<std::tuple<std::string, std::string, std::string>> used_variables;
  std::vector<std::tuple<std::string, std::string, std::string>> created_variables;

  int priority;

  int warningCounter = 0;
  int failCounter = 0;
  int unknowCounter = 0;
  int successCounter = 0;

  std::vector<std::shared_ptr<Check>> checkList;
  void addIdleCheck(std::string id);
  void updateState();

  /*
      Result types :
      - success : The check is a success
      - warning : The check is a warning
      - failed : The check is a failure
      - unknow : The check is not yet processed
      - fatal : The check is a fatal error
  */

  void addCheckVerdict(std::string id, std::string result, std::string log, std::string directive);

  // type // state
  std::vector<std::pair<std::string, std::string>> depsChecks;
  // type // name
  std::vector<std::pair<std::string, std::string>> depsChecksSpec;

  /// Permettre de voir les name des packages,

  // Pour la prochaine fois
  /*
      refAIRE UNE TOOLCHAINE FONCTIONNELLE
      Reprendre toutes les tasklists de Newtoolchains
      Regler le core dumped juste apres l'execution de la task qui se produit surement dans ifProps (apres modification d'un props de plus bas niveau)
      Faire une save de la toolchain au cas ou okyus
      Finir la toolchain et video !!


  */

  bool ifProps(const std::vector<std::string> &propsname);
  bool ifProp(std::string propname);

  std::chrono::time_point<std::chrono::system_clock> m_StartTime;
  std::chrono::time_point<std::chrono::system_clock> m_EndTime;
  double m_TotalTime;
  bool m_bRunning = false;

  double elapsedMilliseconds();
  double elapsedSeconds();

  std::string startTime();
  void start();
  void stop();

  // Custom args
  std::shared_ptr<hArgs> props;

  // Result
  std::shared_ptr<hArgs> result_props;

  // std::shared_ptr<VxHost> parent;

  virtual void init() { VortexMaker::LogError("Core", "Core task not implemented !"); };
  virtual void retry() { VortexMaker::LogError("Core", "Core task not implemented !"); };
  virtual void exec() { VortexMaker::LogError("Core", "Core task not implemented !"); };
  virtual void finish(std::string finish_state, std::shared_ptr<hArgs> result_properties) { VortexMaker::LogError("Core", "Core task not implemented !"); };
};


// Comparateur pour trier les tâches par priorité
struct CompareTaskPriority {
    bool operator()(const std::shared_ptr<Task>& task1, const std::shared_ptr<Task>& task2) const {
        return task1->priority > task2->priority; // Priorité plus élevée a un indice plus bas
    }
};

// Déclaration anticipée de la classe TaskProcessor
class TaskProcessor {
public:
    TaskProcessor();
    ~TaskProcessor();
    void processTasks();
    void markTaskCompleted(std::shared_ptr<Task> task);
    std::vector<std::shared_ptr<Task>> tasksToProcess; // compy references of tasks to process (from "task")

    void startWorker();
    void stopWorker();
    std::atomic<bool> stop;
    std::atomic<bool> running;
    std::mutex mutex;

    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, CompareTaskPriority> tasks;
    
    std::map<int, std::deque<std::shared_ptr<Task>>> tasksByPriority;
    std::condition_variable condition;
    std::vector<std::thread> workers;
    std::thread worker;
};

#endif // __TASKS__MODULE_H__