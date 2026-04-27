#include "../../include/modules/runtime.h"

#include "../../include/vortex_internals.h"

VORTEX_API void FinalStartModule(
    const std::string &module_name,
    std::shared_ptr<std::vector<std::string>> processed_modules) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  for (auto em : ctx->IO.em) {
    if (em->m_name == module_name) {
      if (em->m_state != "running" || em->m_state != "waiting") {
        vxe::log_info("Modules", "Start \"" + em->m_name + "\"");
        processed_modules->push_back(em->m_name);

        try {
          em->Start();
        } catch (const std::exception &e) {
          // Log the error
          vxe::log_error("Modules", "Error starting module \"" + em->m_name + "\": " + e.what());
          // Continue to the next module
          continue;
        }
      }
    }
  }
}

VORTEX_API void vxe::StartModule(const std::string &module_name) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  for (auto em : ctx->IO.em) {
    if (em->m_name == module_name) {
      if (em->m_state != "running") {
        vxe::log_info("Modules", "Start \"" + em->m_name + "\"");

        std::shared_ptr<std::vector<std::string>> processed_modules = std::make_shared<std::vector<std::string>>();

        // Start dependencies
        for (auto deps : em->m_dependencies) {
          FinalStartModule(deps->name, processed_modules);
        }

        // Finally start the module
        em->Start();
      }
    }
  }
}

VORTEX_API void vxe::StartAllModules() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  // TODO create dist envs
  // TODO move to (StartModule) function
  for (auto em : ctx->IO.em) {
    {
      // Set the module data path
      std::string datapath = ctx->projectPath.string();
      datapath += "/.vx/data/" + em->m_name;
      em->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    StartModule(em->m_name);
  }
}

VORTEX_API void vxe::BootstrappAllModules() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  // TODO move to (StartModule) function
  for (auto em : ctx->IO.em) {
    {
      // Set the module data path
      std::string datapath = ctx->projectPath.string();
      datapath += "/.vx/data/" + em->m_name;
      em->m_datapath = datapath;

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    if (em->m_auto_exec) {
      StartModule(em->m_name);
    }
  }
}