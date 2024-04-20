#include "../../include/vortex.h"
#include "./src/instances/toolchainInstance/ToolchainRenderInstance.h"
#include "./src/toolchain.h"


#ifndef CToolchainModule
ToolchainsModuleCTX *CToolchainModule = NULL;
#endif


PackagesModuleCTX *CPackagesModule = NULL;

// Module context ptr, including variables of functions, call at any moments wit convention : HelloTest.someParam or HelloTest.return
static std::shared_ptr<hArgs> arguments;


void TestOutputEvent(std::shared_ptr<hArgs> args){
    if(args != NULL){
        std::cout << "TestOutputEvent trigerred ! With message : " << args->get<const char*>("message", "default") <<  std::endl;
        args->add("result", "ARRIBA");
    }
}


void TestInputEvent(std::shared_ptr<hArgs> args){
    if(args != NULL){
        std::cout << "TestInputEvent trigerred ! With message : " << args->get<const char*>("message", "default") <<  std::endl;
        args->add("result", "ARRIBA");
    }
}

void HelloTest(){
    if(arguments != NULL){
        std::cout << "Hello " << " et " << arguments->get<const char*>("ToolchainsModule.name", "default") << std::endl;
    }
}


 void CreateModuleContext(){
  ToolchainsModuleCTX *ctx = VX_NEW(ToolchainsModuleCTX);
  CToolchainModule = ctx;
}



// TODO : Documentation des apis & vortex install "modulname" & vortex register (current dir)
// TODO : Toolchains & task reconsitution & VIDEO

class ToolchainsModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
    */
    void execute() override
    {
        CreateModuleContext();
        CToolchainModule->m_interface = ModuleInterface::GetEditorModuleByName(this->m_name);
        
        // Add main args
        this->AddArg<const char*>("chainsModule.name", "ToolchainsModule");
        
        // Adding functions
        this->AddFunction(RegisterToolchains, "RegisterToolchains");
        
        // Adding events
        //this->AddInputEvent(TestInputEvent, "test");
        //this->AddOutputEvent(TestOutputEvent, "test");

        // Render instance
        //this->AddModuleRenderInstance(ToolchainRenderInstance(nullptr, nullptr));

        arguments = this->m_args;


        this->ExecFunction("RegisterToolchains");
        // Adding events

    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
        
		ImGui::Begin("Toolchains");
        for(auto toolchain : CToolchainModule->m_toolchains){
            ImGui::Text(toolchain->name.c_str());
            ImGui::SameLine();
            std::string label = "Open ###" + toolchain->name;
            if(ImGui::Button(label.c_str())){
                VxContext* ctx = VortexMaker::GetCurrentContext();
                std::shared_ptr<ToolchainRenderInstance> instance = std::make_shared<ToolchainRenderInstance>(ctx, toolchain);
				//factory->SpawnInstance(instance);	
                instance->name = toolchain->name;
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
    return new ToolchainsModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
