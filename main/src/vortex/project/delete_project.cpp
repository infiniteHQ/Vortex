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
VORTEX_API void VortexMaker::DeleteProject(const std::string &path)
{
        // Search for module files recursively in the directory
        auto module_files = VortexMaker::SearchSystemFiles(path, "module.json");


        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                // Handle data form project if needed

                {
                    std::string cmd = "rm -rf " + path;
                    std::vector<std::string> ff;
                    std::cout << cmd << std::endl;
                    std::cout << ff[100] << std::endl;
                }

            }
            catch(std::exception e)
            {
                std::cout << e.what() << std::endl;
            }
            
        }

    VortexMaker::LogError("Core", "No project to delete at \"" + path + "\"");

}