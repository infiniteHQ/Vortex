//
//  runtime.cpp
//  Sources for plugins runtime (loading and ignition)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <plugins/runtime.h>

VORTEX_API void vxe::final_start_plugin(
    const std::string &plugin_name,
    std::shared_ptr<std::vector<std::string>> processed_plugins) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all plugins & create data environments
  for (auto ep : ctx->IO.ep) {
    if (ep->m_name == plugin_name) {
      if (ep->m_state != "running" || ep->m_state != "waiting") {
        vxe::log_info("Plugins", "Start \"" + ep->m_name + "\"");
        processed_plugins->push_back(ep->m_name);

        try {
          ep->start();
        } catch (const std::exception &e) {
          // Log the error
          vxe::log_error("Plugins", "Error starting plugin \"" + ep->m_name + "\": " + e.what());
          // Continue to the next plugin
          continue;
        }
      }
    }
  }
}

VORTEX_API void vxe::start_plugin(const std::string &plugin_name) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all plugins & create data environments
  for (auto ep : ctx->IO.ep) {
    if (ep->m_name == plugin_name) {
      if (ep->m_state != "running") {
        vxe::log_info("Plugins", "Start \"" + ep->m_name + "\"");

        std::shared_ptr<std::vector<std::string>> processed_plugins = std::make_shared<std::vector<std::string>>();

        // Start dependencies
        for (auto deps : ep->m_dependencies) {
          final_start_plugin(deps->name, processed_plugins);
        }

        // Finally start the plugin
        ep->start();
      }
    }
  }
}

VORTEX_API void vxe::start_all_plugins() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all plugins & create data environments
  // TODO create dist envs
  // TODO move to (start_plugin) function
  for (auto ep : ctx->IO.ep) {
    {
      // Set the plugin data path
      std::string datapath = ctx->projectPath.string();
      datapath += "/.vx/data/" + ep->m_name;
      ep->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    start_plugin(ep->m_name);
  }
}

VORTEX_API void vxe::bootstrapp_all_plugins() {
  auto ctx = vxe::get_current_context();

  for (auto ep : ctx->IO.ep) {
    {
      std::string datapath = ctx->projectPath.string();
      datapath += "/.vx/data/" + ep->m_name;
      ep->m_datapath = datapath;

      // TODO : Rework that with API
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    if (ep->m_auto_exec) {
      start_plugin(ep->m_name);
    }
  }
}