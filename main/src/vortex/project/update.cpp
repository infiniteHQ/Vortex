#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void VortexMaker::UpdateProjectName(const std::string &name)
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream input_file(project_file);
    if (!input_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : bad file");
        return;
    }

    nlohmann::json json_data;
    input_file >> json_data;
    input_file.close();

    if (json_data.contains("project") && json_data["project"].contains("name"))
    {
        json_data["project"]["name"] = name;
    }
    else
    {
        VortexMaker::LogError("Core", "Cannot update the project name : no value in the file");
        return;
    }

    std::ofstream output_file(project_file);
    if (!output_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : cannot write into the file");
        return;
    }
    output_file << json_data.dump(4);
    output_file.close();
}

VORTEX_API void VortexMaker::UpdateProjectVersion(const std::string &version)
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream input_file(project_file);
    if (!input_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : bad file");
        return;
    }

    nlohmann::json json_data;
    input_file >> json_data;
    input_file.close();

    if (json_data.contains("project") && json_data["project"].contains("version"))
    {
        json_data["project"]["version"] = version;
    }
    else
    {
        VortexMaker::LogError("Core", "Cannot update the project version : no value in the file");
        return;
    }

    std::ofstream output_file(project_file);
    if (!output_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project version : cannot write into the file");
        return;
    }
    output_file << json_data.dump(4);
    output_file.close();
}

VORTEX_API void VortexMaker::UpdateProjectAuthor(const std::string &author)
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream input_file(project_file);
    if (!input_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : bad file");
        return;
    }

    nlohmann::json json_data;
    input_file >> json_data;
    input_file.close();

    if (json_data.contains("project") && json_data["project"].contains("author"))
    {
        json_data["project"]["author"] = author;
    }
    else
    {
        VortexMaker::LogError("Core", "Cannot update the project author : no value in the file");
        return;
    }

    std::ofstream output_file(project_file);
    if (!output_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project author : cannot write into the file");
        return;
    }
    output_file << json_data.dump(4);
    output_file.close();
}

VORTEX_API void VortexMaker::UpdateProjectDescription(const std::string &description)
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream input_file(project_file);
    if (!input_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : bad file");
        return;
    }

    nlohmann::json json_data;
    input_file >> json_data;
    input_file.close();

    if (json_data.contains("project") && json_data["project"].contains("description"))
    {
        json_data["project"]["description"] = description;
    }
    else
    {
        VortexMaker::LogError("Core", "Cannot update the project description : no value in the file");
        return;
    }

    std::ofstream output_file(project_file);
    if (!output_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project description : cannot write into the file");
        return;
    }
    output_file << json_data.dump(4);
    output_file.close();
}

VORTEX_API void VortexMaker::UpdateProjectType(const std::string &type)
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream input_file(project_file);
    if (!input_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : bad file");
        return;
    }

    nlohmann::json json_data;
    input_file >> json_data;
    input_file.close();

    if (json_data.contains("project") && json_data["project"].contains("type"))
    {
        json_data["project"]["type"] = type;
    }
    else
    {
        VortexMaker::LogError("Core", "Cannot update the project type : no value in the file");
        return;
    }

    std::ofstream output_file(project_file);
    if (!output_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project type : cannot write into the file");
        return;
    }
    output_file << json_data.dump(4);
    output_file.close();
}

VORTEX_API void VortexMaker::UpdateProjectLogoPath(const std::string &path)
{
    VxContext &ctx = *CVortexMaker;

#if defined(_WIN32) || defined(_WIN64)
    const std::string project_file = ctx.projectPath.string() + "\\vortex.config";
#else
    const std::string project_file = ctx.projectPath.string() + "/vortex.config";
#endif

    std::ifstream input_file(project_file);
    if (!input_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project name : bad file");
        return;
    }

    nlohmann::json json_data;
    input_file >> json_data;
    input_file.close();

    if (json_data.contains("project") && json_data["project"].contains("logoPath"))
    {
        json_data["project"]["logoPath"] = path;
    }
    else
    {
        VortexMaker::LogError("Core", "Cannot update the project logo path : no value in the file");
        return;
    }

    std::ofstream output_file(project_file);
    if (!output_file.is_open())
    {
        VortexMaker::LogError("Core", "Cannot update the project logo path : cannot write into the file");
        return;
    }
    output_file << json_data.dump(4);
    output_file.close();
}
