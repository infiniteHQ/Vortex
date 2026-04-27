#include <templates/install.h>

VORTEX_API void vxe::InstallTemplateOnSystem(const std::string &directory) {
  std::string templates_path = "~/.vx/templates";
  std::string json_file = directory + "/template.json";

  // Verify if the module is valid
  try {
    // Load JSON data from the module configuration file
    auto json_data = vxe::dump_json(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    templates_path += "/" + name + "." + version;

    vxe::log_info("Core", "Installing the template " + name + "...");

    // Create directory
    {
      std::string cmd = "mkdir " + templates_path;
      system(cmd.c_str());
    }

    // Move the module in the final system path
    {
      std::string cmd = "cp -r " + directory + "/* " + templates_path;
      system(cmd.c_str());
    }

    vxe::log_info("Core", "The template " + name + "is now installed on system !");
    return;
  } catch (const std::exception &e) {
    // Print error if an exception occurs
    vxe::log_error("Core", e.what());
  }

  vxe::log_error("Core", "Cannot find template registered at" + directory + " !");
}

VORTEX_API void vxe::InstallTemplate(const std::string &name, const std::string &path) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Finded state
  bool finded = false;

  // TODO : Search modules registered in project first (if the project have
  // include_system_templates == false) Search modules registered in project
  // first
  /*...*/

  for (auto tem : ctx->IO.sys_templates) {
    if (tem->m_name == name) {
      vxe::log_info("Core", "Installing the template \"" + name + "\" ...");

      std::string cmd = "tar -xvf " + tem->m_path + tem->m_tarball + " --strip-components=1 " + " -C " + path;
      std::cout << cmd << std::endl;
      system(cmd.c_str());

      // Then take vortex.config and override
    }
  }

  /*// Search the template registered in the system environment (if not findeed
  in the system and if the project can see env template) if (!finded &&
  ctx->include_system_templates)
  {
      // Get the home directory
      std::string homeDir = vxe::get_home_directory();

      // Module path on the system
      std::string path = homeDir + "/.vx/templates";

      // Search modules registered in system
      auto module_files = vxe::search_files(path, "template.json");

      // Iterate through each found module file
      for (const auto &file : module_files)
      {
          try
          {
              // Load JSON data from the module configuration file
              auto json_data = vxe::dump_json(file);

              std::string module_path = file.substr(0, file.find_last_of("/"));

              std::string name = json_data["name"].get<std::string>();

              if (name == name)
              {
                  finded = true;

                  vxe::log_info("Core", "Installing the template \"" +
  name + "\" ...");

                  // TODO : Call Deploy function of the template

                  std::string cmd = "tar -xvf " + module_path +

                  // Get the target directory of installation (path)
                  // Take the tarball, and uncompress it into destination

                  return;
              }
          }
          catch (std::exception e)
          {
              vxe::log_error("Core", e.what());
          }
      }
  }

  if (!finded)
  {
      vxe::log_error("Core", "Failed to find the template " + name + "
  this template is installed ?");
  }*/
}

VORTEX_API std::vector<std::shared_ptr<TemplateInterface>> vxe::FindTemplatesInDirectory(const std::string &directory) {
  // Search modules registered
  auto template_files = vxe::search_files(directory, "template.json", 3);

  std::vector<std::shared_ptr<TemplateInterface>> interfaces;

  // Iterate through each found module file
  for (const auto &file : template_files) {
    try {
      // Load JSON data from the module configuration file
      auto json_data = vxe::dump_json(file);

      std::string module_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<TemplateInterface> new_template = std::make_shared<TemplateInterface>();

      new_template->m_name = json_data["name"].get<std::string>();
      new_template->m_proper_name = json_data["proper_name"].get<std::string>();
      new_template->m_type = json_data["type"].get<std::string>();
      new_template->m_description = json_data["description"].get<std::string>();
      new_template->m_picture = json_data["picture"].get<std::string>();
      new_template->m_logo_path = module_path + "/" + new_template->m_picture;
      new_template->m_path = module_path + "/";
      new_template->m_author = json_data["author"].get<std::string>();
      new_template->m_group = json_data["group"].get<std::string>();
      new_template->m_tarball = json_data["tarball"].get<std::string>();
      new_template->m_contributors = json_data["contributors"].get<std::vector<std::string>>();

      for (auto dep : json_data["deps"]) {
        std::shared_ptr<TemplateDep> dependence = std::make_shared<TemplateDep>();
        dependence->name = dep["name"].get<std::string>();
        dependence->type = dep["type"].get<std::string>();
        for (auto version : dep["versions"]) {
          dependence->supported_versions.push_back(version);
        }
        new_template->m_dependencies.push_back(dependence);
      }

      interfaces.push_back(new_template);
    } catch (std::exception e) {
      vxe::log_error("Core", e.what());
    }
  }

  return interfaces;
}
