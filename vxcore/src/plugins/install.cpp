#include <plugins/install.h>
#include <plugins/load.h>
#include <plugins/runtime.h>

VORTEX_API void VortexMaker::InstallPlugin(const std::string &plugin_name,
                                           const std::string &version,
                                           bool &restart_plugins) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Get the home directory
  std::string homeDir = VortexMaker::getHomeDirectory();

  // Plugin path on the system
  std::string path = homeDir + "/.vx/plugins";

  // Search plugins registered
  auto plugin_files = VortexMaker::SearchFiles(path, "plugin.json");

  // Finded state
  bool finded = false;

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string plugin_path = file.substr(0, file.find_last_of("/"));

      std::string name = json_data["name"].get<std::string>();
      std::string version = json_data["version"].get<std::string>();

      if (name == plugin_name && version == version) {
        finded = true;

        // Move the plugin into the project
        {
          std::string cmd = "cp -r " + plugin_path + " " +
                            ctx.projectPath.string() + "/.vx/plugins/";
          system(cmd.c_str());
        }

        // If restart_plugins == true, restart all plugins
        if (restart_plugins) {
          // Stop all plugins
          for (auto ep : ctx.IO.ep) {
            ep->Stop();
          }

          // Clear closed plugins
          ctx.IO.ep.clear();

          // Load plugins installed in the current project
          VortexMaker::LoadEditorPlugins(ctx.projectPath.string(),
                                         ctx.IO.ep_handles, ctx.IO.ep);

          // Finally, start all loaded plugins.
          VortexMaker::StartAllPlugins();
        }

        return;
      }
    } catch (std::exception e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  if (!finded) {
    VortexMaker::LogError("Core", "Failed to find the plugin " + plugin_name +
                                      " with version " + version +
                                      ", this plugin is installed ?");
  }

  // Search plugin registered by plugin_name in env
  // Get main informations
  // Create the folder name
  // Copy distant plugin content into the plugin into the projet
  // Restart plugins
}

VORTEX_API std::shared_ptr<PluginInterface>
VortexMaker::FindPluginInDirectory(const std::string &directory) {
  // Search plugins registered
  auto plugin_files = VortexMaker::SearchFiles(directory, "plugin.json");

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string plugin_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<PluginInterface> plugin_interface =
          std::make_shared<PluginInterface>();

      plugin_interface->m_name = json_data["name"].get<std::string>();
      plugin_interface->m_version = json_data["version"].get<std::string>();

      return plugin_interface;

    } catch (std::exception e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  VortexMaker::LogError("Core", "Failed to find the plugin at " + directory +
                                    ", no plugins detected.");
  return nullptr;
}

VORTEX_API std::vector<std::shared_ptr<PluginInterface>>
VortexMaker::FindPluginsInDirectory(const std::string &directory) {
  // Search plugins registered
  auto plugin_files = VortexMaker::SearchFiles(directory, "plugin.json", 3);

  std::vector<std::shared_ptr<PluginInterface>> interfaces;

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = VortexMaker::DumpJSON(file);

      std::string plugin_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<PluginInterface> plugin_interface =
          std::make_shared<PluginInterface>();
      plugin_interface->m_name = json_data["name"].get<std::string>();
      plugin_interface->m_auto_exec = json_data["auto_exec"].get<bool>();
      plugin_interface->m_proper_name =
          json_data["proper_name"].get<std::string>();
      plugin_interface->m_type = json_data["type"].get<std::string>();
      plugin_interface->m_version = json_data["version"].get<std::string>();
      plugin_interface->m_description =
          json_data["description"].get<std::string>();
      plugin_interface->m_picture = json_data["picture"].get<std::string>();
      plugin_interface->m_logo_path =
          plugin_path + "/" + plugin_interface->m_picture;
      plugin_interface->m_path = plugin_path + "/";
      plugin_interface->m_author = json_data["author"].get<std::string>();
      plugin_interface->m_group = json_data["group"].get<std::string>();
      plugin_interface->m_contributors =
          json_data["contributors"].get<std::vector<std::string>>();

      for (auto dep : json_data["deps"]) {
        std::shared_ptr<PluginInterfaceDep> dependence =
            std::make_shared<PluginInterfaceDep>();
        dependence->name = dep["name"].get<std::string>();
        dependence->type = dep["type"].get<std::string>();
        for (auto version : dep["versions"]) {
          dependence->supported_versions.push_back(version);
        }
        plugin_interface->m_dependencies.push_back(dependence);
      }

      plugin_interface->m_supported_versions =
          json_data["compatibility"].get<std::vector<std::string>>();

      interfaces.push_back(plugin_interface);

    } catch (std::exception e) {
      VortexMaker::LogError("Core", e.what());
    }
  }

  return interfaces;
}
