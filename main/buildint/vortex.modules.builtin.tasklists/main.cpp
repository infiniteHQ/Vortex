#include "../../include/vortex.h"
#include "./src/instances/tasklistInstance/TasklistRenderInstance.h"
#include "./src/module.h"

#ifndef CTasklistsModule
ModuleCTX *CTasklistsModule = NULL;
#endif



// Module context ptr, including variables of functions, call at any moments wit convention : HelloTest.someParam or HelloTest.return
static std::shared_ptr<hArgs> arguments;


void CreateTasklistContext(){
  ModuleCTX *ctx = VX_NEW(ModuleCTX);
  CTasklistsModule = ctx;
}

class TasklistsModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
    */
    void execute() override
    {
        CreateTasklistContext();
        std::cout <<CTasklistsModule << std::endl;
        std::cout << "tt" << std::endl;
        CTasklistsModule->m_interface = std::make_shared<ModuleInterface>(*this);
        
        std::cout << "tt" << std::endl;
        // Add main args
        this->AddArg<const char*>("chainsModule.name", "TasklistsModule");

        // Add logo
        this->AddLogo(icons::_i,icons::_i_size);
        
        // Adding functions
        this->AddFunction(RegisterTasklist, "RegisterTasklists");
        
        // Adding events
        //this->AddInputEvent(TestInputEvent, "test");
        //this->AddOutputEvent(TestOutputEvent, "test");

        // Render instance
        //this->AddModuleRenderInstance(ToolchainRenderInstance(nullptr, nullptr));

        arguments = this->m_args;

        this->ExecFunction("RegisterTasklists");
        // Adding events

    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
        
		ImGui::Begin("");
            std::cout << CTasklistsModule->m_tasklists.size() << std::endl;
        for(auto tasklist : CTasklistsModule->m_tasklists){
            ImGui::Text(tasklist->label.c_str());
            ImGui::SameLine();
            std::string label = "Open ###" + tasklist->label;
            if(ImGui::Button(label.c_str())){
                VxContext* ctx = VortexMaker::GetCurrentContext();
                std::shared_ptr<TasklistRenderInstance> instance = std::make_shared<TasklistRenderInstance>(ctx, tasklist);
				//factory->SpawnInstance(instance);	
                instance->name = tasklist->label;
                this->AddModuleRenderInstance(instance);

            }
        }
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
    return new TasklistsModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
