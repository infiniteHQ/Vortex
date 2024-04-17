#include "../../../include/vortex.h"
#include "task.h"
#include <queue>

// Comparateur pour trier les tâches par priorité
struct CompareTaskPriority {
    bool operator()(const std::shared_ptr<Task>& task1, const std::shared_ptr<Task>& task2) const {
        return task1->priority > task2->priority; // Priorité plus élevée a un indice plus bas
    }
};

// Déclaration anticipée de la classe TaskModuleProcessor
class TaskModuleProcessor {
public:
    TaskModuleProcessor();
    ~TaskModuleProcessor();
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