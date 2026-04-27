//
//  load.cpp
//  Sources for plugins information loading features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex.h>
#include <vortex_internals.h>

#include <plugins/load.hpp>

namespace vxe {

  VORTEX_API void load_editor_plugins(
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

        new_plugin->name_ = json_data["name"].get<std::string>();
        new_plugin->auto_exec_ = json_data["auto_exec"].get<bool>();
        new_plugin->proper_name_ = json_data["proper_name"].get<std::string>();
        new_plugin->type_ = json_data["type"].get<std::string>();
        new_plugin->version_ = json_data["version"].get<std::string>();
        new_plugin->description_ = json_data["description"].get<std::string>();
        new_plugin->picture_ = json_data["picture"].get<std::string>();
        new_plugin->logo_path_ = (fs::path(path) / new_plugin->picture_).string();
        new_plugin->path_ = (fs::path(path)).string();
        new_plugin->m_mainscript_path = new_plugin->path_ + "/" + json_data["path"].get<std::string>();

        new_plugin->author_ = json_data["author"].get<std::string>();
        new_plugin->group_ = json_data["group"].get<std::string>();
        new_plugin->contributors_ = json_data["contributors"].get<std::vector<std::string>>();

        for (auto dep : json_data["deps"]) {
          std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
          dependence->name = dep["name"].get<std::string>();
          dependence->type = dep["type"].get<std::string>();
          for (auto version : dep["versions"]) {
            dependence->supported_versions.push_back(version);
          }
          new_plugin->dependencies_.push_back(dependence);
        }

        new_plugin->supported_versions_ = json_data["compatibility"].get<std::vector<std::string>>();
        plugins.push_back(new_plugin);
      } catch (const std::exception &e) {
        vxe::log_error("Plugins", "Failed to load plugin metadata: " + std::string(e.what()));
      }
    }
  }

  VORTEX_API void load_system_plugins(std::vector<std::shared_ptr<PluginInterface>> &sys_plugins) {
    // Get the home directory
    std::string homeDir = vxe::get_home_directory();

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
          new_plugin->name_ = json_data["name"].get<std::string>();
          new_plugin->auto_exec_ = json_data["auto_exec"].get<bool>();
          new_plugin->proper_name_ = json_data["proper_name"].get<std::string>();
          new_plugin->type_ = json_data["type"].get<std::string>();
          new_plugin->version_ = json_data["version"].get<std::string>();
          new_plugin->description_ = json_data["description"].get<std::string>();
          new_plugin->picture_ = json_data["picture"].get<std::string>();
          new_plugin->logo_path_ = plugin_path + "/" + new_plugin->picture_;
          new_plugin->path_ = plugin_path + "/";
          new_plugin->m_mainscript_path = new_plugin->path_ + "/" + json_data["path"].get<std::string>();
          new_plugin->author_ = json_data["author"].get<std::string>();
          new_plugin->group_ = json_data["group"].get<std::string>();
          new_plugin->contributors_ = json_data["contributors"].get<std::vector<std::string>>();

          for (auto dep : json_data["deps"]) {
            std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
            dependence->name = dep["name"].get<std::string>();
            dependence->type = dep["type"].get<std::string>();
            for (auto version : dep["versions"]) {
              dependence->supported_versions.push_back(version);
            }
            new_plugin->dependencies_.push_back(dependence);
          }

          new_plugin->supported_versions_ = json_data["compatibility"].get<std::vector<std::string>>();
        } catch (std::exception e) {
          std::cerr << e.what() << std::endl;
        }

        VXINFO("Plugins registered in system ", new_plugin->name_ + " loaded with success !")

        // Store the plugin instance and handle
        sys_plugins.push_back(new_plugin);
      } catch (const std::exception &e) {
        // Print error if an exception occurs
        vxe::log_error("Core", std::string("Error: ") + e.what());
      }
    }
  }

}  // namespace vxe