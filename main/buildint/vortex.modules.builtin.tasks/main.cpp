#include "../../include/vortex.h"
#include "./assets/logo.h"
#include "./src/module.h"

#ifndef CTasksModule
TasksModuleCTX *CTasksModule = NULL;
#endif



// Module context ptr, including variables of functions, call at any moments wit convention : HelloTest.someParam or HelloTest.return
static std::shared_ptr<hArgs> arguments;

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
        
        // Add main args
        this->AddArg<const char*>("chainsModule.name", "TasksModule");

        // Add logo
        
        // Adding functions
        //this->AddFunction(Register, "RegisterPackages");
        
        // Adding events
        this->AddInputEvent(TaskModule::CreateTaskPool, "CreateTaskPool");
        this->AddInputEvent(TaskModule::CreateTaskProcessor, "CreateTaskProcessor");
        this->AddInputEvent(TaskModule::AddTaskToPool, "AddTaskToPool");
        this->AddInputEvent(TaskModule::GetTaskPool, "GetTaskPool");
        this->AddInputEvent(TaskModule::StartTaskProcessor, "StartTaskProcessor");
        this->AddInputEvent(TaskModule::StopTaskProcessor, "StopTaskProcessor");
        this->AddInputEvent(TaskModule::AddTaskToProcess, "AddTaskToProcess");
        //this->AddOutputEvent(TestOutputEvent, "test");

        // Render instance
        //this->AddModuleRenderInstance(ToolchainRenderInstance(nullptr, nullptr));

        arguments = this->m_args;


        this->ExecFunction("RegisterPackages");
        // Adding events

    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
        
		ImGui::Begin("Tasks");
        ImGui::End();
        // "Launcher" of regitered toolchains
    }
/*
    void openToolchain(){ // Make a function
        // Get instance, open it and create a render instance

        // Ctx (from module), Toolchain
        ToolchainRenderInstance instance(VortexMaker::GetCurrentContext(), nullptr);
        this->AddModuleRenderInstance(instance);
    }*/
};

extern "C" ModuleInterface *create_em()
{
    return new TasksModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
