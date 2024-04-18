#include "../../include/vortex.h"
#include "./src/instances/packageInstance/PackageRenderInstance.h"
#include "./src/module.h"

#ifndef CTasksModule
TasksModuleCTX *CTasksModule = NULL;
#endif



// Module context ptr, including variables of functions, call at any moments wit convention : HelloTest.someParam or HelloTest.return
static std::shared_ptr<hArgs> arguments;


void CreatePackageContext(){
  TasksModuleCTX *ctx = VX_NEW(TasksModuleCTX);
  CTasksModule = ctx;
}

class PackagesModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
    */
    void execute() override
    {
        CreatePackageContext();
        std::cout <<CTasksModule << std::endl;
        CTasksModule->m_interface = std::make_shared<ModuleInterface>(*this);
        
        // Add main args
        this->AddArg<const char*>("chainsModule.name", "PackagesModule");

        // Add logo
        this->AddLogo(icons::_i,icons::_i_size);
        
        // Adding functions
        //this->AddFunction(Register, "RegisterPackages");
        
        // Adding events
        //this->AddInputEvent(TestInputEvent, "test");
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
    return new PackagesModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
