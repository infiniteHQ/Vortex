#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"
#include "../../include/modules/load.h"


namespace VortexMaker{
    

/**
 * @brief Load modules from the specified directory.
 * 
 * This function searches for plugin files in the specified directory and loads them dynamically.
 * It loads the plugin shared objects (SO files) and creates instances of modules using their factory functions.
 * 
 * @param directory The directory where plugin files are located.
 * @param modules_handlers A vector to store handles of loaded modules.
 * @param modules A vector to store instances of loaded modules.
 */
void LoadEditorModules(const std::string &directory, std::vector<void *> &modules_handlers, std::vector<std::shared_ptr<ModuleInterface>> &modules)
{
    // Search for plugin files recursively in the directory
    auto plugin_files = VortexMaker::SearchFiles(directory, "plugin.json");
    
    // Iterate through each found plugin file
    for (const auto &file : plugin_files)
    {
        std::cout << "File found at: " << file << std::endl;
        try
        {
            // Load JSON data from the plugin configuration file
            auto json_data = VortexMaker::DumpJSON(file);
            
            // Get the directory path containing the plugin file
            std::string path = file.substr(0, file.find_last_of("/"));
            std::cout << "Plugin path: " << path << std::endl;
            
            // Search for shared object (SO) files in the same directory as the plugin configuration file
            auto so_files = VortexMaker::SearchFiles(path, "libplugin.so");

            // Iterate through each found SO file
            for (const auto &so_file : so_files)
            {
                // Load the shared object
                void *handle = dlopen(so_file.c_str(), RTLD_LAZY | RTLD_GLOBAL);
                if (!handle)
                {
                    // Print error if failed to load the shared object
                    std::cerr << "Failed to load plugin: " << dlerror() << std::endl;
                    continue;
                }
                
                // Reset errors
                dlerror();
                
                // Get the symbol for creating the plugin instance
                auto create_editor_module = reinterpret_cast<ModuleInterface *(*)()>(dlsym(handle, "create_editor_module"));
                const char *dlsym_error = dlerror();
                if (dlsym_error)
                {
                    // Print error if failed to load symbol
                    std::cerr << "Failed to load symbol: " << dlsym_error << std::endl;
                    dlclose(handle);
                    continue;
                }
                
                // Create an instance of the plugin
                std::shared_ptr<ModuleInterface> plugin(create_editor_module());
                if (!plugin)
                {
                    // Print error if failed to create plugin instance
                    std::cerr << "Failed to create plugin instance" << std::endl;
                    dlclose(handle);
                    continue;
                }
                
                // Store the plugin instance and handle
                modules.push_back(plugin);
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