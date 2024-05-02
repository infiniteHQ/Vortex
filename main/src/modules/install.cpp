#include "../../include/modules/install.h"
#include "../../include/modules/load.h"
#include "../../include/modules/runtime.h"

VORTEX_API void VortexMaker::InstallModule(const std::string &module_name, const std::string &version, bool &restart_modules)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Get the home directory
    std::string homeDir = VortexMaker::getHomeDirectory();

    // Module path on the system
    std::string path = homeDir + "/.vx/modules";

    // Search modules registered
    auto module_files = VortexMaker::SearchFiles(path, "module.json");

    // Finded state
    bool finded = false;

    // Iterate through each found module file
    for (const auto &file : module_files)
    {
        try
        {
            // Load JSON data from the module configuration file
            auto json_data = VortexMaker::DumpJSON(file);

            std::string module_path = file.substr(0, file.find_last_of("/"));

            std::string name = json_data["name"].get<std::string>();
            std::string version = json_data["version"].get<std::string>();

            if (name == module_name && version == version)
            {
                finded = true;

                // Move the module into the project
                {
                    std::string cmd = "cp -r " + module_path + " " + ctx.projectPath.c_str() + "/.vx/modules/";
                    system(cmd.c_str());
                }

                // If restart_modules == true, restart all modules
                if (restart_modules)
                {
                    // Stop all modules
                    for (auto em : ctx.IO.em)
                    {
                        em->Stop();
                    }

                    // Clear closed modules
                    ctx.IO.em.clear();

                    // Load modules installed in the current project
                    VortexMaker::LoadEditorModules(ctx.projectPath, ctx.IO.em_handles, ctx.IO.em);

                    // Finally, start all loaded modules.
                    VortexMaker::StartAllModules();
                }

                return;
            }
        }
        catch (std::exception e)
        {
            VortexMaker::LogError("Core", e.what());
        }
    }

    if (!finded)
    {
        VortexMaker::LogError("Core", "Failed to find the module " + module_name + " with version " + version + ", this module is installed ?");
    }

    // Search module registered by module_name in env
    // Get main informations
    // Create the folder name
    // Copy distant module content into the module into the projet
    // Restart modules
}