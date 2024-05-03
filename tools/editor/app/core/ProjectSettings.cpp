
#include "ProjectSettings.hpp"

static std::vector<std::string> labels = {"General Informations", "Configuration", "Production"};
static int selected = 0;

ProjectSettings::ProjectSettings(VxContext *_ctx, InstanceFactory *_factory)
{
    this->ctx = _ctx;
    this->factory = _factory;

    this->Refresh();

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
}

void ProjectSettings::OnImGuiRender()
{
    static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::Begin("Project settings", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        this->menubar();

    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project settings of");
    ImGui::SameLine();
    ImGui::Text(this->ctx->name.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 0.9;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Vortex version");
    ImGui::SameLine();
    ImGui::Text(this->ctx->version.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    ImGui::Separator();

    {
        ImGui::BeginChild("left pane", ImVec2(230, -1), true);

        for (int i = 0; i < labels.size(); i++)
        {
            if (ImGui::Selectable(labels[i].c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();

    if (selected == 0)
    {
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
        ImGui::InputText("Project name", this->current_save->name, 128);
        ImGui::InputText("Project author", this->current_save->author, 128);
        ImGui::InputText("Project version", this->current_save->version, 128);
        ImGui::InputTextMultiline("Project description", this->current_save->description, 128);
        ImGui::InputText("Project type", this->current_save->type, 128);
        ImGui::Checkbox("Include system template", &this->current_save->include_system_templates);

        ImGui::PopStyleVar(2);
        ImGui::EndGroup();
    }
    else if (selected == 1)
    {
        ImGui::Text("1");
    }
    else if (selected == 2)
    {
        ImGui::Text("2");
    }

    ImGui::End();
}

// Helper functions for menu items

static void handleRefresh()
{
    // Behavior
}

static void handleAddToProject(const std::string &name, const std::string &version)
{
    // Behavior
}

static void handleFilterBuildRebuild()
{
    // Behavior
}

static void handleGlobalBuild()
{
    // Behavior
}

static void handleCreateModule()
{
    // Behavior
}

static void handleSearch()
{
    // Behavior
}

void ProjectSettings::addModuleModal()
{
}

void ProjectSettings::mainButtonsMenuItem()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::ImageButtonWithText(refreshIcon, "Save", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
    {
        this->Update();
    }
    if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
    {
        this->Refresh();
    }
}

void ProjectSettings::filterMenuItem()
{
    ImGui::Separator();
    if (ImGui::BeginMenu("Filters"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleFilterBuildRebuild();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectSettings::createMenuItem()
{
    if (ImGui::BeginMenu("Create a module"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleCreateModule();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectSettings::searchMenuItem()
{
    if (ImGui::BeginMenu("Search"))
    {
        if (ImGui::MenuItem("Build/Rebuild single parts"))
        {
            handleSearch();
        }
        if (ImGui::MenuItem("Global build"))
        {
            handleGlobalBuild();
        }
        ImGui::EndMenu();
    }
}

void ProjectSettings::menubar()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::BeginMenuBar())
    {
        this->mainButtonsMenuItem();
        this->filterMenuItem();
        this->createMenuItem();
        this->searchMenuItem();
        ImGui::EndMenuBar();
    }
}

void ProjectSettings::Refresh()
{
    std::string path = this->ctx->projectPath;
    path += "/vortex.config";

    nlohmann::json projectData = VortexMaker::DumpJSON(path);

    std::shared_ptr<ProjectSettingsSave> newsave = std::make_shared<ProjectSettingsSave>();

    this->ctx->name = projectData["project"]["name"].get<std::string>();
    strncpy(newsave->name, this->ctx->name.c_str(), sizeof(newsave->name));
    newsave->name[sizeof(newsave->name) - 1] = '\0';

    this->ctx->author = projectData["project"]["author"].get<std::string>();
    strncpy(newsave->author, this->ctx->author.c_str(), sizeof(newsave->author));
    newsave->author[sizeof(newsave->author) - 1] = '\0';

    this->ctx->description = projectData["project"]["description"].get<std::string>();
    strncpy(newsave->description, this->ctx->description.c_str(), sizeof(newsave->description));
    newsave->description[sizeof(newsave->description) - 1] = '\0';

    this->ctx->type = projectData["project"]["type"].get<std::string>();
    strncpy(newsave->type, this->ctx->type.c_str(), sizeof(newsave->type));
    newsave->type[sizeof(newsave->type) - 1] = '\0';

    this->ctx->project_version = projectData["project"]["version"].get<std::string>();
    strncpy(newsave->version, this->ctx->version.c_str(), sizeof(newsave->version));
    newsave->version[sizeof(newsave->version) - 1] = '\0';

    this->ctx->include_system_templates = projectData["project"]["include_system_templates"].get<bool>();
    newsave->include_system_templates = this->ctx->include_system_templates;

    this->current_save = newsave;
}

void ProjectSettings::Update()
{
    std::string oldname = this->ctx->name;

    nlohmann::json toolchainData;
    toolchainData["project"]["name"] = this->current_save->name;
    toolchainData["project"]["author"] = this->current_save->author;
    toolchainData["project"]["description"] = this->current_save->description;
    toolchainData["project"]["type"] = this->current_save->type;
    toolchainData["project"]["version"] = this->current_save->version;
    toolchainData["project"]["include_system_templates"] = this->current_save->include_system_templates;

    std::string path = this->ctx->projectPath;
    path += "/vortex.config";

    std::ofstream file(path);
    if (file.is_open())
    {
        file << std::setw(4) << toolchainData << std::endl;
        VortexMaker::LogInfo("Core", "Object saved to " + path);
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " + path + " for writing!");
    }

    this->Refresh();

    VortexMaker::UpdateEnvironmentProject(oldname);
}