#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief CreateProject creates a new project with the specified name and path.
 * 
 * This function creates a new project directory structure and initializes necessary files
 * such as 'vortex.config'.
 * 
 * @param name The name of the project.
 * @param path The path where the project will be created.
 */
void VortexMaker::CreateProject(const std::string& name, const std::string& path)
{
    std::string projectPath;

    // Creating main project folder
    {
        projectPath = path + "/" + name + "/";
        std::string cmd = "mkdir "  +projectPath;
        system(cmd.c_str());
    }

    // Creating subdirectories and files
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx";
        system(cmd.c_str());
    }
    {
        std::string cmd = "touch "  + projectPath + "/vortex.config";
        system(cmd.c_str());
    }
    // Creating and populating JSON data for vortex.config
    {
        nlohmann::json j;
        j["project"]["author"] = "unknown";
        j["project"]["description"] = "This is a toolchain";
        j["project"]["name"] = name;
        j["project"]["type"] = "???";
        j["project"]["version"] = "1.0.0";
        j["project"]["compatibleWith"] = VORTEX_VERSION;
        j["project"]["include_system_templates"] = true;

        j["data"]["toolchains"] = "./.vx/data/toolchains/";
        j["data"]["hosts"] = "./.vx/data/hosts/";
        j["data"]["scripts"] = "./.vx/data/scripts/";
        j["data"]["gpos"] = "./.vx/data/gpos/";
        j["data"]["packages"] = "./.vx/data/packages/";

        j["dist"]["toolchains"] = "./.vx/dist/toolchains/";
        j["dist"]["gpos"] = "./.vx/dist/gpos/";
        j["dist"]["packages"] = "./.vx/dist/packages/";
        j["dist"]["hosts"] = "./.vx/dist/hosts/";

        // Store JSON into vortex.config file
        std::ofstream o(projectPath + "/vortex.config");
        o << std::setw(4) << j << std::endl;
        o.close();
    }

    // Creating data directory
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx/data";
        system(cmd.c_str());
    }
    // Creating subdirectories in data directory
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx/data/hosts";
        system(cmd.c_str());
    }
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx/data/kernels";
        system(cmd.c_str());
    }
    // (Additional subdirectories omitted for brevity)

    // Creating dist directory
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx/dist";
        system(cmd.c_str());
    }
    // Creating subdirectories in dist directory
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx/dist/hosts";
        system(cmd.c_str());
    }
    // (Additional subdirectories omitted for brevity)

    // Creating temporary directory
    {
        std::string cmd = "mkdir "  + projectPath + "/.vx/temp";
        system(cmd.c_str());
    }
}
