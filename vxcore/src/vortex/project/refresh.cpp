#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::RefreshProjectInformations() {
  VxContext &ctx = *CVortexMaker;

  const std::string project_file = (ctx.projectPath / "vortex.config").string();

  std::ifstream file(project_file);
  nlohmann::json jsonContent;

  if (file) {
    try {
      file >> jsonContent;
    } catch (const std::exception &e) {
      VortexMaker::LogError(
          "Core",
          std::string("Failed to parse vortex.config JSON: ") + e.what());
      return;
    }
    // VortexMaker::InitProject(jsonContent);
  } else {
    VortexMaker::LogError("Core", "Failed to open vortex.config file");
    return;
  }

  if (!jsonContent.contains("project") || !jsonContent["project"].is_object()) {
    VortexMaker::LogError("Core",
                          "vortex.config missing valid 'project' section");
    return;
  }

  const auto &project = jsonContent["project"];

  if (project.contains("name") && project["name"].is_string())
    ctx.name = project["name"].get<std::string>();

  ctx.label = ctx.name;

  if (project.contains("author") && project["author"].is_string())
    ctx.author = project["author"].get<std::string>();

  if (project.contains("description") && project["description"].is_string())
    ctx.description = project["description"].get<std::string>();

  if (project.contains("type") && project["type"].is_string())
    ctx.type = project["type"].get<std::string>();

  if (project.contains("version") && project["version"].is_string())
    ctx.project_version = project["version"].get<std::string>();

  if (project.contains("compatibleWith") &&
      project["compatibleWith"].is_string())
    ctx.compatibleWith = project["compatibleWith"].get<std::string>();

  if (project.contains("include_system_templates") &&
      project["include_system_templates"].is_boolean())
    ctx.include_system_templates =
        project["include_system_templates"].get<bool>();

  if (project.contains("tags") && project["tags"].is_string())
    ctx.tags = project["tags"].get<std::string>();

  if (project.contains("logo_path") && project["logo_path"].is_string())
    ctx.logo_path = project["logo_path"].get<std::string>();

  if (project.contains("website") && project["website"].is_string())
    ctx.website = project["website"].get<std::string>();

  if (project.contains("support_contact") &&
      project["support_contact"].is_string())
    ctx.support_contact = project["support_contact"].get<std::string>();

  if (project.contains("copyright_file") &&
      project["copyright_file"].is_string())
    ctx.copyright_file = project["copyright_file"].get<std::string>();

  if (project.contains("license_file") && project["license_file"].is_string())
    ctx.license_file = project["license_file"].get<std::string>();

  if (project.contains("readme_file") && project["readme_file"].is_string())
    ctx.readme_file = project["readme_file"].get<std::string>();

  if (project.contains("requirements_file") &&
      project["requirements_file"].is_string())
    ctx.requirements_file = project["requirements_file"].get<std::string>();

  if (project.contains("code_of_conduct_file") &&
      project["code_of_conduct_file"].is_string())
    ctx.code_of_conduct_file =
        project["code_of_conduct_file"].get<std::string>();

  if (project.contains("security_file") && project["security_file"].is_string())
    ctx.security_file = project["security_file"].get<std::string>();

  if (project.contains("root_content_path") &&
      project["root_content_path"].is_string())
    ctx.root_content_path = project["root_content_path"].get<std::string>();
}
