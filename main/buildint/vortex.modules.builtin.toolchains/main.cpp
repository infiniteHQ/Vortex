#include "../../include/vortex.h"
#include "./src/instances/toolchainInstance/ToolchainInstance.h"
#include "./src/toolchain/toolchain.h"

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
        CToolchainModule->m_interface = std::make_shared<ModuleInterface>(*this);
        
        // Add main args
        this->AddArg<const char*>("chainsModule.name", "ToolchainsModule");

        // Add logo
        this->AddLogo(icons::_i,icons::_i_size);
        
        // Adding functions
        this->AddFunction(Register, "RegisterToolchains");
        
        // Adding events
        //this->AddInputEvent(TestInputEvent, "test");
        //this->AddOutputEvent(TestOutputEvent, "test");

        // Render instance
        //this->AddModuleRenderInstance(ToolchainInstance(nullptr, nullptr));

        arguments = this->m_args;

        std::cout << "TEst" << std::endl;
        this->ExecFunction("RegisterToolchains");
        std::cout << "TEst" << std::endl;

        // Adding events

    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
        std::cout << CToolchainModule->m_toolchains.size()<< std::endl;
		ImGui::Begin("Contefgghnt SQDQSD");
        for(auto toolchains : CToolchainModule->m_toolchains){
            ImGui::Text(toolchains->name.c_str());
        }
        ImGui::End();
        // "Launcher" of regitered toolchains
    }
/*
    void openToolchain(){ // Make a function
        // Get instance, open it and create a render instance

        // Ctx (from module), Toolchain
        ToolchainInstance instance(VortexMaker::GetCurrentContext(), nullptr);
        this->AddModuleRenderInstance(instance);
    }*/
};

extern "C" ModuleInterface *create_em()
{
    return new ToolchainsModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
