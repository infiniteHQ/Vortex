#include "../../include/vortex.h"
#include "./src/instances/toolchainInstance/ToolchainInstance.h"
#include "./src/functions/OpenToolchain.hpp"

// Module context ptr, including variables of functions, call at any moments wit convention : HelloTest.someParam or HelloTest.return
static std::shared_ptr<hArgs> arguments;


void TestOutputEvent(std::shared_ptr<hArgs> args){
    if(args != NULL){
        std::cout << "TestOutputEvent trigerred ! With message : " << args->get<const char*>("message", "default") <<  std::endl;
    }
}


void TestInputEvent(std::shared_ptr<hArgs> args){
    if(args != NULL){
        std::cout << "TestInputEvent trigerred ! With message : " << args->get<const char*>("message", "default") <<  std::endl;
    }
}

void HelloTest(){
    if(arguments != NULL){
        std::cout << "Hello " << " et " << arguments->get<const char*>("ToolchainsModule.name", "default") << std::endl;
    }
}

// TODO : Events, Load static lib, Finish Toolchain system

class ToolchainsModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
    */
    void execute() override
    {
        // Add main args
        this->AddArg<const char*>("chainsModule.name", "ToolchainsModule");

        // Add logo
        this->AddLogo(icons::_i,icons::_i_size);
        
        // Adding functions
        this->AddFunction(HelloTest, "HelloTest");
        
        // Adding events
        this->AddInputEvent(TestInputEvent, "test");
        this->AddOutputEvent(TestOutputEvent, "test");

        // Render instance
        //this->AddModuleRenderInstance(ToolchainInstance(nullptr, nullptr));

        arguments = this->m_args;

        this->ExecFunction("HelloTest");

        // Adding events

    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
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
