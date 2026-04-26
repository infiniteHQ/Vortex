#pragma once
#include <vortex.h>

#ifndef VORTEX_PROJECT_HPP
#define VORTEX_PROJECT_HPP

namespace vxe {
  VORTEX_API void init_project(const nlohmann::json &main_config);
  VORTEX_API void finish_process();
  VORTEX_API void create_project(const std::string &name, const std::string &path);
  VORTEX_API void create_project(
      const std::string &name,
      const std::string &author,
      const std::string &version,
      const std::string &description,
      const std::string &path,
      const std::string &logo_path,
      const std::string &template_name);
  VORTEX_API void delete_project(const std::string &path, const std::string &project_name);
  VORTEX_API void import_project(const std::string &path);
  VORTEX_API std::vector<std::shared_ptr<EnvProject>> find_project_in_folder(const std::string &path);
  VORTEX_API void refresh_project_informations();
  VORTEX_API void remove_system_project_entry(const std::string &project_name);

  VORTEX_API void update_project_data(const std::string &old_name, const std::string &path);
  VORTEX_API void update_project_name(const std::string &name);
  VORTEX_API void update_project_version(const std::string &version);
  VORTEX_API void update_project_author(const std::string &author);
  VORTEX_API void update_project_description(const std::string &description);
  VORTEX_API void update_project_type(const std::string &type);
  VORTEX_API void update_project_logo_path(const std::string &path);
  VORTEX_API void update_project_banner_path(const std::string &path);
  VORTEX_API void update_project_tags(const std::string &tags);
  VORTEX_API void update_project_website(const std::string &website);
  VORTEX_API void update_project_support_contact(const std::string &contact);
  VORTEX_API void update_project_copyright_flle(const std::string &path);
  VORTEX_API void update_project_license_flle(const std::string &path);
  VORTEX_API void update_project_readme_flle(const std::string &path);
  VORTEX_API void update_project_requirements_flle(const std::string &path);
  VORTEX_API void update_project_code_of_conduct_flle(const std::string &path);
  VORTEX_API void update_project_security_flle(const std::string &path);
  VORTEX_API void update_project_root_content_path(const std::string &root_path);
  VORTEX_API void update_project_startup_script(const std::string &startup_script);

}  // namespace vxe

#endif  // VORTEX_PROJECT_HPP