#include "../../include/vortex.h"
#include "./src/module.h"
#include "./src/instances/packageInstance/PackageRenderInstance.h"

#ifndef CPackagesModule
PackagesModuleCTX *CPackagesModule = NULL;
#endif


class PackagesModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
     */
    void execute() override
    {
        // Create the context ptr of this module
        PackageModule::CreatePackageContext();

        // Get the interface ptr (for GUI launcher, from others modules)
        CPackagesModule->m_interface = ModuleInterface::GetEditorModuleByName(this->m_name);

        // Add main args
        this->AddArg<const char *>("chainsModule.name", "PackagesModule");

        // Add logo
        //this->AddLogo(icons::package_icon, icons::_i_size);

        // Adding functions
        this->AddFunction(RegisterPackages, "RegisterPackages");

        // Adding events
        this->AddInputEvent(PackageModule::LaunchPackageInterface, "LaunchPackageInterface");
        this->AddInputEvent(PackageModule::FindPackages,           "FindPackages");
        //this->AddInputEvent(PackageModule::RegisterPackage,        "RegisterPackage");

        // Execute main functions 
        this->ExecFunction("RegisterPackages");
    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
     */
    void render() override
    {

        ImGui::Begin("Packages");
        for (auto package : CPackagesModule->m_packages)
        {
            ImGui::Text(package->name.c_str());
            ImGui::SameLine();
            std::string label = "Open ###" + package->name;
            if (ImGui::Button(label.c_str()))
            {
                VxContext *ctx = VortexMaker::GetCurrentContext();
                std::shared_ptr<PackageRenderInstance> instance = std::make_shared<PackageRenderInstance>(ctx, package);
                // factory->SpawnInstance(instance);
                instance->name = package->name;
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
    return new PackagesModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
