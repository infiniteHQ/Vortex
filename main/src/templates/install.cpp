#include "../../include/templates/install.h"

VORTEX_API void VortexMaker::InstallTemplateOnSystem(const std::string &directory)
{
    std::string modules_path = "~/.vx/templates";
    std::string json_file = directory + "/template.json";

    // Verify if the module is valid
    try
    {
        // Load JSON data from the module configuration file
        auto json_data = VortexMaker::DumpJSON(json_file);
        std::string name = json_data["name"].get<std::string>();
        std::string proper_name = json_data["proper_name"].get<std::string>();
        std::string type = json_data["type"].get<std::string>();
        std::string version = json_data["version"].get<std::string>();
        std::string description = json_data["description"].get<std::string>();
        std::string author = json_data["author"].get<std::string>();

        //std::string origin_path = path.substr(0, path.find_last_of("/"));
        modules_path += "/" + name + "." + version;

        VortexMaker::LogInfo("Core", "Installing the module " + name + "...");

        // Create directory
        {
            std::string cmd = "mkdir " + modules_path;
            system(cmd.c_str());
        }

        // Move the module in the final system path
        {
            std::string cmd = "cp -r " + directory + "/* " + modules_path;
            system(cmd.c_str());
        }
    }
    catch (const std::exception &e)
    {
        // Print error if an exception occurs
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

VORTEX_API void VortexMaker::InstallTemplate(const std::string &name)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Finded state
    bool finded = false;

    // TODO : Search modules registered in project first (if the project have include_system_templates == false)
    // Search modules registered in project first
    /*...*/

    // Search the template registered in the system environment (if not findeed in the system and if the project can see env template)
    if (!finded && ctx.include_system_templates)
    {
        // Get the home directory
        std::string homeDir = VortexMaker::getHomeDirectory();

        // Module path on the system
        std::string path = homeDir + "/.vx/templates";

        // Search modules registered in system
        auto module_files = VortexMaker::SearchFiles(path, "template.json");

        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                std::string module_path = file.substr(0, file.find_last_of("/"));

                std::string name = json_data["name"].get<std::string>();

                if (name == name)
                {
                    finded = true;

                    VortexMaker::LogInfo("Core", "Installing the template \"" + name + "\" ...");

                    // TODO : Call Deploy function of the template

                    return;
                }
            }
            catch (std::exception e)
            {
                VortexMaker::LogError("Core", e.what());
            }
        }
    }

    if (!finded)
    {
        VortexMaker::LogError("Core", "Failed to find the template " + name + " this template is installed ?");
    }
}