#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

#define UPDATE_JSON_FIELD(FIELD, VALUE)                                        \
  if (json_data.contains("project")) {                                         \
    json_data["project"][FIELD] = VALUE;                                       \
  } else {                                                                     \
    VortexMaker::LogError("Core", "Cannot update the project " FIELD           \
                                  " : no 'project' key in the file");          \
    return;                                                                    \
  }

#define UPDATE_PROJECT_FIELD(FUNC_NAME, JSON_FIELD)                            \
  VORTEX_API void VortexMaker::FUNC_NAME(const std::string &value) {           \
    VxContext &ctx = *CVortexMaker;                                            \
                                                                               \
    const std::string project_file =                                           \
        (ctx.projectPath / "vortex.config").string();                          \
                                                                               \
    std::ifstream input_file(project_file);                                    \
    if (!input_file.is_open()) {                                               \
      VortexMaker::LogError("Core", "Cannot update the project " #JSON_FIELD   \
                                    " : bad file");                            \
      return;                                                                  \
    }                                                                          \
                                                                               \
    nlohmann::json json_data;                                                  \
    input_file >> json_data;                                                   \
    input_file.close();                                                        \
                                                                               \
    UPDATE_JSON_FIELD(#JSON_FIELD, value);                                     \
                                                                               \
    std::ofstream output_file(project_file);                                   \
    if (!output_file.is_open()) {                                              \
      VortexMaker::LogError("Core", "Cannot update the project " #JSON_FIELD   \
                                    " : cannot write into the file");          \
      return;                                                                  \
    }                                                                          \
    output_file << json_data.dump(4);                                          \
    output_file.close();                                                       \
  }

UPDATE_PROJECT_FIELD(UpdateProjectTags, tags)
UPDATE_PROJECT_FIELD(UpdateProjectWebsite, website)
UPDATE_PROJECT_FIELD(UpdateProjectSupportContact, support_contact)
UPDATE_PROJECT_FIELD(UpdateProjectCopyrightFile, copyright_file)
UPDATE_PROJECT_FIELD(UpdateProjectLicenseFile, license_file)
UPDATE_PROJECT_FIELD(UpdateProjectReadmeFile, readme_file)
UPDATE_PROJECT_FIELD(UpdateProjectRequirementsFile, requirements_file)
UPDATE_PROJECT_FIELD(UpdateProjectCodeOfConductFile, code_of_conduct_file)
UPDATE_PROJECT_FIELD(UpdateProjectSecurityFile, security_file)
UPDATE_PROJECT_FIELD(UpdateProjectName, name)
UPDATE_PROJECT_FIELD(UpdateProjectVersion, version)
UPDATE_PROJECT_FIELD(UpdateProjectAuthor, author)
UPDATE_PROJECT_FIELD(UpdateProjectDescription, description)
UPDATE_PROJECT_FIELD(UpdateProjectType, type)
UPDATE_PROJECT_FIELD(UpdateProjectLogoPath, logoPath)
UPDATE_PROJECT_FIELD(UpdateProjectRootContentPath, root_content_path)
UPDATE_PROJECT_FIELD(UpdateProjectStartupScript, startup_script)
