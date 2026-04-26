#include <plugins/load.h>
#include <vortex.h>
#include <vortex_internals.h>

namespace vxe {

  VORTEX_API void LoadEditorPlugins(
      const std::string &directory,
      std::vector<void *> &plugins_handlers,
      std::vector<std::shared_ptr<PluginInterface>> &plugins) {
    plugins.clear();
    plugins_handlers.clear();

    auto plugin_files = vxe::search_files(directory, "plugin.json");

    // Load scripts etc instead of dll/so (only for plugins)

    for (const auto &file : plugin_files) {
      try {
        auto json_data = vxe::dump_json(file);

        fs::path plugin_path(file);
        fs::path path = plugin_path.parent_path();

        std::shared_ptr<PluginInterface> new_plugin = std::make_shared<PluginInterface>();

        new_plugin->m_name = json_data["name"].get<std::string>();
        new_plugin->m_auto_exec = json_data["auto_exec"].get<bool>();
        new_plugin->m_proper_name = json_data["proper_name"].get<std::string>();
        new_plugin->m_type = json_data["type"].get<std::string>();
        new_plugin->m_version = json_data["version"].get<std::string>();
        new_plugin->m_description = json_data["description"].get<std::string>();
        new_plugin->m_picture = json_data["picture"].get<std::string>();
        new_plugin->m_logo_path = (fs::path(path) / new_plugin->m_picture).string();
        new_plugin->m_path = (fs::path(path)).string();
        new_plugin->m_mainscript_path = new_plugin->m_path + "/" + json_data["path"].get<std::string>();

        new_plugin->m_author = json_data["author"].get<std::string>();
        new_plugin->m_group = json_data["group"].get<std::string>();
        new_plugin->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

        for (auto dep : json_data["deps"]) {
          std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
          dependence->name = dep["name"].get<std::string>();
          dependence->type = dep["type"].get<std::string>();
          for (auto version : dep["versions"]) {
            dependence->supported_versions.push_back(version);
          }
          new_plugin->m_dependencies.push_back(dependence);
        }

        new_plugin->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();
        plugins.push_back(new_plugin);
      } catch (const std::exception &e) {
        vxe::log_error("Plugins", "Failed to load plugin metadata: " + std::string(e.what()));
      }
    }
  }

  VORTEX_API void LoadSystemPlugins(std::vector<std::shared_ptr<PluginInterface>> &sys_plugins) {
    // Get the home directory
    std::string homeDir = vxe::getHomeDirectory();

    // Plugin path on the system
    std::string path = homeDir + "/.vx/plugins";

    // Search for plugin files recursively in the directory
    auto plugin_files = vxe::search_system_files(path, "plugin.json");

    // Clear system plugins vector
    sys_plugins.clear();

    // Iterate through each found plugin file
    for (const auto &file : plugin_files) {
      try {
        // Load JSON data from the plugin configuration file
        auto json_data = vxe::dump_json(file);

        std::string plugin_path = file.substr(0, file.find_last_of("/"));

        std::shared_ptr<PluginInterface> new_plugin = std::make_shared<PluginInterface>();

        // Try to fetch plugin informations from plugin.json
        try {
          new_plugin->m_name = json_data["name"].get<std::string>();
          new_plugin->m_auto_exec = json_data["auto_exec"].get<bool>();
          new_plugin->m_proper_name = json_data["proper_name"].get<std::string>();
          new_plugin->m_type = json_data["type"].get<std::string>();
          new_plugin->m_version = json_data["version"].get<std::string>();
          new_plugin->m_description = json_data["description"].get<std::string>();
          new_plugin->m_picture = json_data["picture"].get<std::string>();
          new_plugin->m_logo_path = plugin_path + "/" + new_plugin->m_picture;
          new_plugin->m_path = plugin_path + "/";
          new_plugin->m_mainscript_path = new_plugin->m_path + "/" + json_data["path"].get<std::string>();
          new_plugin->m_author = json_data["author"].get<std::string>();
          new_plugin->m_group = json_data["group"].get<std::string>();
          new_plugin->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

          for (auto dep : json_data["deps"]) {
            std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
            dependence->name = dep["name"].get<std::string>();
            dependence->type = dep["type"].get<std::string>();
            for (auto version : dep["versions"]) {
              dependence->supported_versions.push_back(version);
            }
            new_plugin->m_dependencies.push_back(dependence);
          }

          new_plugin->m_supported_versions = json_data["compatibility"].get<std::vector<std::string>>();
        } catch (std::exception e) {
          std::cerr << e.what() << std::endl;
        }

        VXINFO("Plugins registered in system ", new_plugin->m_name + " loaded with success !")

        // Store the plugin instance and handle
        sys_plugins.push_back(new_plugin);
      } catch (const std::exception &e) {
        // Print error if an exception occurs
        vxe::log_error("Core", std::string("Error: ") + e.what());
      }
    }
  }

}  // namespace vxe