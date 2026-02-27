#include "../../../include/modules/load.h"
#include "../../../include/modules/runtime.h"
#include "../../../include/plugins/load.h"
#include "../../../include/plugins/runtime.h"
#include "../../../include/templates/load.h"
#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief Initialize project settings based on provided JSON configurations.
 *
 * This function initializes project settings based on the configurations
 * provided in a JSON object. It sets various properties of the VortexMaker
 * context object accordingly.
 *
 * @param main_configs JSON object containing main project configurations.
 */
VORTEX_API void VortexMaker::InitProject(const nlohmann::json &main_configs) {
  // Get reference to the Vortex context
  VxContext &ctx = *CVortexMaker;

  if (!main_configs.contains("project")) {
    VortexMaker::LogError("Core", "Invalid config: missing 'project' section");
    return;
  }

  const auto &project = main_configs["project"];

  if (project.contains("name"))
    ctx.name = project["name"].get<std::string>();

  if (project.contains("author"))
    ctx.author = project["author"].get<std::string>();

  if (project.contains("description"))
    ctx.description = project["description"].get<std::string>();

  if (project.contains("type"))
    ctx.type = project["type"].get<std::string>();

  if (project.contains("version"))
    ctx.version = project["version"].get<std::string>();

  if (project.contains("compatibleWith"))
    ctx.compatibleWith = project["compatibleWith"].get<std::string>();

  if (project.contains("tags"))
    ctx.tags = project["tags"].get<std::string>();

  if (project.contains("startup_script"))
    ctx.startup_script = project["startup_script"].get<std::string>();

  if (project.contains("logo_path"))
    ctx.logo_path = project["logo_path"].get<std::string>();

  if (project.contains("website"))
    ctx.website = project["website"].get<std::string>();

  if (project.contains("support_contact"))
    ctx.support_contact = project["support_contact"].get<std::string>();

  if (project.contains("copyright_file"))
    ctx.copyright_file = project["copyright_file"].get<std::string>();

  if (project.contains("license_file"))
    ctx.license_file = project["license_file"].get<std::string>();

  if (project.contains("readme_file"))
    ctx.readme_file = project["readme_file"].get<std::string>();

  if (project.contains("requirements_file"))
    ctx.requirements_file = project["requirements_file"].get<std::string>();

  if (project.contains("code_of_conduct_file"))
    ctx.code_of_conduct_file =
        project["code_of_conduct_file"].get<std::string>();

  if (project.contains("security_file"))
    ctx.security_file = project["security_file"].get<std::string>();

  if (project.contains("include_system_templates"))
    ctx.include_system_templates =
        project["include_system_templates"].get<bool>();

  if (project.contains("name"))
    ctx.label = ctx.name;

  // Set project path to current working directory
  ctx.projectPath = fs::current_path();

  if (project.contains("root_content_path")) {
    ctx.root_content_path = project["root_content_path"].get<std::string>();

    fs::path rootContentPath = ctx.projectPath / ctx.root_content_path;

    if (fs::exists(rootContentPath)) {
      ctx.projectDataPath = rootContentPath;

    } else {
      ctx.projectDataPath = ctx.projectPath;
      VortexMaker::LogError("Core", "root_content_path does not exist: " +
                                        rootContentPath.string());
    }
  }

  // TODO : Only for early development.
  ctx.logger = true;
  ctx.logger_registering = true;

  // Initialize environment
  VortexMaker::InitEnvironment();

  // Update projet metadata (last opened, etc...) in the
  // ~/.vx/data/projects.json
  VortexMaker::UpdateEnvironmentProject();

  // Load modules installed in the current project
  VortexMaker::LoadEditorModules(ctx.projectPath.string(), ctx.IO.em_handles,
                                 ctx.IO.em);

  // Load plugins installed in the current project
  VortexMaker::LoadEditorPlugins(ctx.projectPath.string(), ctx.IO.ep_handles,
                                 ctx.IO.ep);

  // Load modules installed in the system
  // Note: These modules are simply initialized in the project, not loaded, but
  // we can add these in CLI/GUI
  VortexMaker::LoadSystemModules(ctx.IO.sys_em);

  // Load plugins installed in the system
  // Note: These modules are simply initialized in the project, not loaded, but
  // we can add these in CLI/GUI
  VortexMaker::LoadSystemPlugins(ctx.IO.sys_ep);

  // Load templates installed in the system if the configuration allow it
  // Note: These templates are simply initialized in the project, not included,
  // but we can add these in CLI/GUI
  if (ctx.include_system_templates) {
    VortexMaker::LoadSystemTemplates(ctx.IO.sys_templates);
  }

  // Refresh themes
  VortexMaker::VerifyAndPouplateThemes();
  VortexMaker::RefreshProjectThemes();
  VortexMaker::RebuildTheme();

  // Startup features
  VortexMaker::ExecuteStartScript();

  // Finally, start all loaded modules.
  VortexMaker::BootstrappAllModules();
}