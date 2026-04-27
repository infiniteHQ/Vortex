//
//  load.cpp
//  Sources for template loading features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/templates/load.hpp"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

namespace vxe {
  VORTEX_API void load_system_templates(std::vector<std::shared_ptr<TemplateInterface>> &sys_templates) {
    // Get the home directory
    std::string homeDir = vxe::get_home_directory();

    // Module path on the system
    std::string path = homeDir + "/.vx/templates";

    // Verify if the path exist, if not, create it
    if (!fs::exists(path)) {
      fs::create_directories(path);
    }

    // Clear actual templates array
    sys_templates.clear();

    // Search for module files recursively in the directory
    auto module_files = vxe::search_system_files(path, "template.json");

    // Iterate through each found module file
    for (const auto &file : module_files) {
      try {
        // Load JSON data from the module configuration file
        auto json_data = vxe::dump_json(file);

        std::string module_path = file.substr(0, file.find_last_of("/"));

        std::shared_ptr<TemplateInterface> new_template = std::make_shared<TemplateInterface>();

        // Try to fetch module informations from module.json
        try {
          new_template->name_ = json_data["name"].get<std::string>();
          new_template->proper_name_ = json_data["proper_name"].get<std::string>();
          new_template->type_ = json_data["type"].get<std::string>();
          new_template->description_ = json_data["description"].get<std::string>();
          new_template->picture_ = json_data["picture"].get<std::string>();
          new_template->logo_path_ = module_path + "/" + new_template->picture_;
          new_template->path_ = module_path + "/";
          new_template->author_ = json_data["author"].get<std::string>();
          new_template->group_ = json_data["group"].get<std::string>();
          new_template->m_tarball = json_data["tarball"].get<std::string>();
          new_template->contributors_ = json_data["contributors"].get<std::vector<std::string>>();

          for (auto dep : json_data["deps"]) {
            std::shared_ptr<TemplateDep> dependence = std::make_shared<TemplateDep>();
            dependence->name = dep["name"].get<std::string>();
            dependence->type = dep["type"].get<std::string>();
            for (auto version : dep["versions"]) {
              dependence->supported_versions.push_back(version);
            }
            new_template->dependencies_.push_back(dependence);
          }
        } catch (std::exception e) {
          vxe::log_error("Core", e.what());
        }

        VXINFO("Template registered in system ", new_template->name_ + " loaded with success !")

        // Store the module instance and handle
        sys_templates.push_back(new_template);
      } catch (const std::exception &e) {
        // Print error if an exception occurs
        std::cerr << "Error: " << e.what() << std::endl;
      }
    }
  }

}  // namespace vxe