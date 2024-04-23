#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/modules/load.h"

namespace VortexMaker
{

    /**
     * @brief Load modules from the specified directory.
     *
     * This function searches for module files in the specified directory and loads them dynamically.
     * It loads the module shared objects (SO files) and creates instances of modules using their factory functions.
     *
     * @param directory The directory where module files are located.
     * @param modules_handlers A vector to store handles of loaded modules.
     * @param modules A vector to store instances of loaded modules.
     */
    void LoadEditorModules(const std::string &directory, std::vector<void *> &modules_handlers, std::vector<std::shared_ptr<ModuleInterface>> &modules)
    {
        // Search for module files recursively in the directory
        auto module_files = VortexMaker::SearchFiles(directory, "module.json");

        // Iterate through each found module file
        for (const auto &file : module_files)
        {
            try
            {
                // Load JSON data from the module configuration file
                auto json_data = VortexMaker::DumpJSON(file);

                // Get the directory path containing the module file
                std::string path = file.substr(0, file.find_last_of("/"));

                // Search for shared object (SO) files in the same directory as the module configuration file
                auto so_files = VortexMaker::SearchFiles(path, "libmodule.so");

                // Iterate through each found SO file
                for (const auto &so_file : so_files)
                {
                    // Load the shared object
                    void *handle = dlopen(so_file.c_str(), RTLD_LAZY | RTLD_GLOBAL);
                    if (!handle)
                    {
                        // Print error if failed to load the shared object
                        std::cerr << "Failed to load module: " << dlerror() << std::endl;
                        continue;
                    }

                    // Reset errors
                    dlerror();

                    // Get the symbol for creating the module instance
                    auto create_em = reinterpret_cast<ModuleInterface *(*)()>(dlsym(handle, "create_em"));
                    const char *dlsym_error = dlerror();
                    if (dlsym_error)
                    {
                        // Print error if failed to load symbol
                        std::cerr << "Failed to load symbol: " << dlsym_error << std::endl;
                        dlclose(handle);
                        continue;
                    }

                    // Create an instance of the module
                    std::shared_ptr<ModuleInterface> new_module(create_em());

                    // Try to fetch module informations from module.json
                    try
                    {
                        new_module->m_name = json_data["name"].get<std::string>();
                        new_module->m_auto_exec = json_data["auto_exec"].get<bool>();
                        new_module->m_proper_name = json_data["proper_name"].get<std::string>();
                        new_module->m_type = json_data["type"].get<std::string>();
                        new_module->m_version = json_data["version"].get<std::string>();
                        new_module->m_description = json_data["description"].get<std::string>();
                        new_module->m_picture = json_data["picture"].get<std::string>();
                        new_module->m_logo_path = path + "/" + new_module->m_picture;
                        new_module->m_author = json_data["author"].get<std::string>();
                        new_module->m_group = json_data["group"].get<std::string>();
                        new_module->m_contributors = json_data["contributors"].get<std::vector<std::string>>();
                    }
                    catch (std::exception e)
                    {
                        std::cerr << e.what() << std::endl;
                    }

                    if (!new_module)
                    {
                        // Print error if failed to create module instance
                        std::cerr << "Failed to create module instance" << std::endl;
                        dlclose(handle);
                        continue;
                    }

                    VXINFO("Modules", new_module->m_name + " module loaded with success !")

                    // Store the module instance and handle
                    modules.push_back(new_module);
                    modules_handlers.push_back(handle);
                }
            }
            catch (const std::exception &e)
            {
                // Print error if an exception occurs
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

} // namespace VortexMaker