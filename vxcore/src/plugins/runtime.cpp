#include <plugins/runtime.h>

VORTEX_API void
FinalStartPlugin(const std::string &plugin_name,
                 std::shared_ptr<std::vector<std::string>> processed_plugins) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  for (auto ep : ctx.IO.ep) {
    if (ep->m_name == plugin_name) {
      if (ep->m_state != "running" || ep->m_state != "waiting") {
        VortexMaker::LogInfo("Plugins", "Start \"" + ep->m_name + "\"");
        processed_plugins->push_back(ep->m_name);

        try {
          ep->Start();
        } catch (const std::exception &e) {
          // Log the error
          VortexMaker::LogError("Plugins", "Error starting plugin \"" +
                                               ep->m_name + "\": " + e.what());
          // Continue to the next plugin
          continue;
        }
      }
    }
  }
}

VORTEX_API void VortexMaker::StartPlugin(const std::string &plugin_name) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  for (auto ep : ctx.IO.ep) {
    if (ep->m_name == plugin_name) {
      if (ep->m_state != "running") {
        VortexMaker::LogInfo("Plugins", "Start \"" + ep->m_name + "\"");

        std::shared_ptr<std::vector<std::string>> processed_plugins =
            std::make_shared<std::vector<std::string>>();

        // Start dependencies
        for (auto deps : ep->m_dependencies) {
          FinalStartPlugin(deps->name, processed_plugins);
        }

        // Finally start the plugin
        ep->Start();
      }
    }
  }
}

VORTEX_API void VortexMaker::StartAllPlugins() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  // TODO create dist envs
  // TODO move to (StartPlugin) function
  for (auto ep : ctx.IO.ep) {
    {
      // Set the plugin data path
      std::string datapath = ctx.projectPath.string();
      datapath += "/.vx/data/" + ep->m_name;
      ep->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    StartPlugin(ep->m_name);
  }
}

VORTEX_API void VortexMaker::BootstrappAllPlugins() {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  // Start all plugins & create data environments
  // TODO move to (StartPlugin) function
  for (auto ep : ctx.IO.ep) {
    {
      // Set the plugin data path
      std::string datapath = ctx.projectPath.string();
      datapath += "/.vx/data/" + ep->m_name;
      ep->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    if (ep->m_auto_exec) {
      StartPlugin(ep->m_name);
    }
  }
}