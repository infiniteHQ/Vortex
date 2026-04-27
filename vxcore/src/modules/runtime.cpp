//
//  runtime.cpp
//  Sources for modules runtime functions (ignition and starting)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/modules/runtime.hpp"

#include "../../include/vortex_internals.h"

VORTEX_API void vxe::final_start_modules(
    const std::string &module_name,
    std::shared_ptr<std::vector<std::string>> processed_modules) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  for (auto em : ctx->IO.em) {
    if (em->name() == module_name) {
      if (em->state() != "running" || em->state() != "waiting") {
        vxe::log_info("Modules", "Start \"" + em->name() + "\"");
        processed_modules->push_back(em->name());

        try {
          em->start();
        } catch (const std::exception &e) {
          // Log the error
          vxe::log_error("Modules", "Error starting module \"" + em->name() + "\": " + e.what());
          // Continue to the next module
          continue;
        }
      }
    }
  }
}

VORTEX_API void vxe::start_module(const std::string &module_name) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  for (auto em : ctx->IO.em) {
    if (em->name() == module_name) {
      if (em->state() != "running") {
        vxe::log_info("Modules", "Start \"" + em->name() + "\"");

        std::shared_ptr<std::vector<std::string>> processed_modules = std::make_shared<std::vector<std::string>>();

        // Start dependencies
        for (auto deps : em->dependencies()) {
          final_start_modules(deps->name, processed_modules);
        }

        // Finally start the module
        em->start();
      }
    }
  }
}

VORTEX_API void vxe::start_all_modules() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  // TODO create dist envs
  // TODO move to (start_module) function
  for (auto em : ctx->IO.em) {
    {
      // Set the module data path
      std::string datapath = ctx->projectPath.string();
      datapath += "/.vx/data/" + em->name();
      em->datapath(datapath);

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    start_module(em->name());
  }
}

VORTEX_API void vxe::bootstrapp_all_modules() {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Start all modules & create data environments
  // TODO move to (start_module) function
  for (auto em : ctx->IO.em) {
    {
      // Set the module data path
      std::string datapath = ctx->projectPath.string();
      datapath += "/.vx/data/" + em->name();
      em->datapath(datapath);

      // Try to create the datapath folder (if not exist yet)
      std::string cmd = "sudo mkdir ";
      cmd += datapath.c_str();
      system(cmd.c_str());
    }

    if (em->auto_exec()) {
      start_module(em->name());
    }
  }
}