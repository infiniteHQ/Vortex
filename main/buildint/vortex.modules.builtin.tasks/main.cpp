#include "../../include/vortex.h"
#include "./assets/logo.h"
#include "./src/module.h"

#ifndef CTasksModule
TasksModuleCTX *CTasksModule = NULL;
#endif

class TasksModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
    */
    void execute() override
    {
        TaskModule::CreateTasksContext();
        CTasksModule->m_interface = ModuleInterface::GetEditorModuleByName(this->m_name);

        // Adding events
        this->AddInputEvent(TaskModule::CreateTaskPool, "CreateTaskPool");
        this->AddInputEvent(TaskModule::CreateTaskProcessor, "CreateTaskProcessor");
        this->AddInputEvent(TaskModule::AddTaskToPool, "AddTaskToPool");
        this->AddInputEvent(TaskModule::GetTaskPool, "GetTaskPool");
        this->AddInputEvent(TaskModule::StartTaskProcessor, "StartTaskProcessor");
        this->AddInputEvent(TaskModule::StopTaskProcessor, "StopTaskProcessor");
        this->AddInputEvent(TaskModule::AddTaskToProcess, "AddTaskToProcess");
        this->AddInputEvent(TaskModule::OpenTaskReportInstance, "OpenTaskReportInstance");
        this->AddInputEvent(TaskModule::StartProcessor, "StartProcessor");
        this->AddInputEvent(TaskModule::StopProcessor, "StopProcessor");
    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
        
		ImGui::Begin("Tasks");
        ImGui::End();
    }
};

extern "C" ModuleInterface *create_em()
{
    return new TasksModule();
}

