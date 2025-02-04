#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::RefreshProjectInformations()
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream file(project_file);
    nlohmann::json jsonContent;

    if (file)
    {
        file >> jsonContent;
        VortexMaker::InitProject(jsonContent);
    }
    // Initialize context properties based on JSON configurations
    ctx.author = jsonContent["project"]["author"].get<std::string>();
    ctx.description = jsonContent["project"]["description"].get<std::string>();
    ctx.label = jsonContent["project"]["name"].get<std::string>();
    ctx.name = jsonContent["project"]["name"].get<std::string>();
    ctx.type = jsonContent["project"]["type"].get<std::string>();
    ctx.project_version = jsonContent["project"]["version"].get<std::string>();
    ctx.compatibleWith = jsonContent["project"]["compatibleWith"].get<std::string>();
    ctx.include_system_templates = jsonContent["project"]["include_system_templates"].get<bool>();
}