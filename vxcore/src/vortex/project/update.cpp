#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

#define UPDATE_JSON_FIELD(FIELD, VALUE)                                                           \
  if (json_data.contains("project")) {                                                            \
    json_data["project"][FIELD] = VALUE;                                                          \
  } else {                                                                                        \
    vxe::log_error("Core", "Cannot update the project " FIELD " : no 'project' key in the file"); \
    return;                                                                                       \
  }

#define UPDATE_PROJECT_FIELD(FUNC_NAME, JSON_FIELD)                                                     \
  VORTEX_API void vxe::FUNC_NAME(const std::string &value) {                                            \
    VxContext &ctx = *CVortexMaker;                                                                     \
                                                                                                        \
    const std::string project_file = (ctx.projectPath / "vortex.config").string();                      \
                                                                                                        \
    std::ifstream input_file(project_file);                                                             \
    if (!input_file.is_open()) {                                                                        \
      vxe::log_error("Core", "Cannot update the project " #JSON_FIELD " : bad file");                   \
      return;                                                                                           \
    }                                                                                                   \
                                                                                                        \
    nlohmann::json json_data;                                                                           \
    input_file >> json_data;                                                                            \
    input_file.close();                                                                                 \
                                                                                                        \
    UPDATE_JSON_FIELD(#JSON_FIELD, value);                                                              \
                                                                                                        \
    std::ofstream output_file(project_file);                                                            \
    if (!output_file.is_open()) {                                                                       \
      vxe::log_error("Core", "Cannot update the project " #JSON_FIELD " : cannot write into the file"); \
      return;                                                                                           \
    }                                                                                                   \
    output_file << json_data.dump(4);                                                                   \
    output_file.close();                                                                                \
  }

UPDATE_PROJECT_FIELD(update_project_tags, tags)
UPDATE_PROJECT_FIELD(update_project_website, website)
UPDATE_PROJECT_FIELD(update_project_support_contact, support_contact)
UPDATE_PROJECT_FIELD(update_project_copyright_flle, copyright_file)
UPDATE_PROJECT_FIELD(update_project_license_flle, license_file)
UPDATE_PROJECT_FIELD(update_project_readme_flle, readme_file)
UPDATE_PROJECT_FIELD(update_project_requirements_flle, requirements_file)
UPDATE_PROJECT_FIELD(update_project_code_of_conduct_flle, code_of_conduct_file)
UPDATE_PROJECT_FIELD(update_project_security_flle, security_file)
UPDATE_PROJECT_FIELD(update_project_name, name)
UPDATE_PROJECT_FIELD(update_project_version, version)
UPDATE_PROJECT_FIELD(update_project_author, author)
UPDATE_PROJECT_FIELD(update_project_description, description)
UPDATE_PROJECT_FIELD(update_project_type, type)
UPDATE_PROJECT_FIELD(update_project_logo_path, logoPath)
UPDATE_PROJECT_FIELD(update_project_banner_path, bannerPath)
UPDATE_PROJECT_FIELD(update_project_root_content_path, root_content_path)
UPDATE_PROJECT_FIELD(update_project_startup_script, startup_script)
