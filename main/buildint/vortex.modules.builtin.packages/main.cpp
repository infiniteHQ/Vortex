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
     * @brief Executes the module's main functionality.
     *
     * This function is called to execute the main functionality of the module.
     * It creates the context pointer for the module, retrieves the interface pointer,
     * adds functions and events, and executes the main function.
     */
    void execute() override
    {
        // Create the context pointer of this module
        PackageModule::CreatePackageContext();

        // Get the interface pointer (for GUI launcher, from other modules)
        CPackagesModule->m_interface = ModuleInterface::GetEditorModuleByName(this->m_name);

        // Adding functions
        this->AddFunction(RegisterPackages, "RegisterPackages");

        // Adding events
        this->AddInputEvent(PackageModule::LaunchPackageInterface, "LaunchPackageInterface");
        this->AddInputEvent(PackageModule::FindPackages, "FindPackages");

        // Execute main functions
        this->ExecFunction("RegisterPackages");
    }

    /**
     * @brief Renders the GUI for the Packages.
     *
     * This function is responsible for rendering the GUI for the Packages.
     * It displays each package's name along with an "Open" button.
     * When the button is clicked, it creates and adds a render instance for the package.
     */
    void render() override
    {
        ImGui::Begin("Packages");

        // Iterate through each package
        for (auto package : CPackagesModule->m_packages)
        {
            // Display the package's name
            ImGui::Text(package->name.c_str());

            // Display "Open" button for the package
            ImGui::SameLine();
            std::string label = "Open ###" + package->name;
            if (ImGui::Button(label.c_str()))
            {
                // Create a render instance for the package and add it
                VxContext *ctx = VortexMaker::GetCurrentContext();
                std::shared_ptr<PackageRenderInstance> instance = std::make_shared<PackageRenderInstance>(ctx, package);
                instance->name = package->name;
                this->AddModuleRenderInstance(instance);
            }
        }

        ImGui::End();
    }
};

extern "C" ModuleInterface *create_em()
{
    return new PackagesModule();
}
