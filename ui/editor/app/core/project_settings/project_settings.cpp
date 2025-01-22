
#include "project_settings.hpp"

using namespace Cherry;

namespace VortexEditor
{
    ProjectSettingsAppWindow::ProjectSettingsAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        this->AddChild("General", "Project informations", [this]() {});

        this->AddChild("General", "Compatibility", [this]() {});

        this->AddChild("General", "Migrate", [this]() {});
    }

    void ProjectSettingsAppWindow::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs.push_back(ProjectSettingsChild(parent_name, child_name, child));
    }

    void ProjectSettingsAppWindow::RemoveChild(const std::string &child_name)
    {
    }

    std::function<void()> ProjectSettingsAppWindow::GetChild(const std::string &child_name)
    {
        for (auto &child : m_Childs)
        {
            if (child.m_ChildName == child_name)
            {
                return child.m_Callback;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Cherry::AppWindow> &ProjectSettingsAppWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<ProjectSettingsAppWindow> ProjectSettingsAppWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<ProjectSettingsAppWindow>(new ProjectSettingsAppWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void ProjectSettingsAppWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void ProjectSettingsAppWindow::Render()
    {
        static float leftPaneWidth = 300.0f;
        const float minPaneWidth = 50.0f;
        const float splitterWidth = 1.5f;
        static int selected;

        std::map<std::string, std::vector<ProjectSettingsChild>> groupedByParent;
        for (const auto &child : m_Childs)
        {
            groupedByParent[child.m_Parent].push_back(child);
        }

        ImGui::BeginChild("left_pane", ImVec2(leftPaneWidth, 0), true, ImGuiWindowFlags_NoBackground);

        ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

        TitleThree("Manage Modules");
        for (const auto &[parent, children] : groupedByParent)
        {

            ImGui::GetFont()->Scale *= 0.8;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
            ImGui::Text(parent.c_str());
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
            ImGui::Separator();
            ImGui::PopStyleColor();

            ImGui::GetFont()->Scale = 0.84;
            ImGui::PopFont();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

            for (const auto &child : children)
            {
                if (child.m_ChildName == m_SelectedChildName)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                }

                if (TextButtonUnderline(child.m_ChildName.c_str()))
                {
                    m_SelectedChildName = child.m_ChildName;
                }

                ImGui::PopStyleColor();
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, HexToRGBA("#44444466"));
        ImGui::Button("splitter", ImVec2(splitterWidth, -1));
        ImGui::PopStyleVar();

        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (ImGui::IsItemActive())
        {
            float delta = ImGui::GetIO().MouseDelta.x;
            leftPaneWidth += delta;
            if (leftPaneWidth < minPaneWidth)
                leftPaneWidth = minPaneWidth;
        }

        ImGui::SameLine();
        ImGui::BeginGroup();

        if (!m_SelectedChildName.empty())
        {
            std::function<void()> pannel_render = GetChild(m_SelectedChildName);
            if (pannel_render)
            {
                pannel_render();
            }
        }

        ImGui::EndGroup();

        /*float oldsize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 1.3;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project settings of");
        ImGui::SameLine();
        ImGui::Text(VortexMaker::GetCurrentContext()->name.c_str());

        ImGui::GetFont()->Scale = oldsize;
        ImGui::PopFont();

        oldsize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 0.9;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Vortex version");
        ImGui::SameLine();
        ImGui::Text(VortexMaker::GetCurrentContext()->version.c_str());

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
            ImGui::InputText("Project name", current_save->name, 128);
            ImGui::InputText("Project author", current_save->author, 128);
            ImGui::InputText("Project version", current_save->version, 128);
            ImGui::InputTextMultiline("Project description", current_save->description, 128);
            ImGui::InputText("Project type", current_save->type, 128);
            ImGui::Checkbox("Include system template", &current_save->include_system_templates);

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
        }*/
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

    void ProjectSettingsAppWindow::addModuleModal()
    {
    }

    void ProjectSettingsAppWindow::mainButtonsMenuItem()
    {

        /*if (ImGui::ImageButtonWithText(refreshIcon, "Save", ImVec2(m_RefreshIcon->GetWidth(), m_RefreshIcon->GetHeight())))
        {
            Update();
        }
        if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(m_RefreshIcon->GetWidth(), m_RefreshIcon->GetHeight())))
        {
            Refresh();
        }*/
    }

    void ProjectSettingsAppWindow::filterMenuItem()
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

    void ProjectSettingsAppWindow::createMenuItem()
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

    void ProjectSettingsAppWindow::searchMenuItem()
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

    void ProjectSettingsAppWindow::menubar()
    {
    }

    void ProjectSettingsAppWindow::Refresh()
    {
        std::string path = VortexMaker::GetCurrentContext()->projectPath.string();
        path += "/vortex.config";

        nlohmann::json projectData = VortexMaker::DumpJSON(path);

        std::shared_ptr<ProjectSettingsSave> newsave = std::make_shared<ProjectSettingsSave>();

        VortexMaker::GetCurrentContext()->name = projectData["project"]["name"].get<std::string>();
        strncpy(newsave->name, VortexMaker::GetCurrentContext()->name.c_str(), sizeof(newsave->name));
        newsave->name[sizeof(newsave->name) - 1] = '\0';

        VortexMaker::GetCurrentContext()->author = projectData["project"]["author"].get<std::string>();
        strncpy(newsave->author, VortexMaker::GetCurrentContext()->author.c_str(), sizeof(newsave->author));
        newsave->author[sizeof(newsave->author) - 1] = '\0';

        VortexMaker::GetCurrentContext()->description = projectData["project"]["description"].get<std::string>();
        strncpy(newsave->description, VortexMaker::GetCurrentContext()->description.c_str(), sizeof(newsave->description));
        newsave->description[sizeof(newsave->description) - 1] = '\0';

        VortexMaker::GetCurrentContext()->type = projectData["project"]["type"].get<std::string>();
        strncpy(newsave->type, VortexMaker::GetCurrentContext()->type.c_str(), sizeof(newsave->type));
        newsave->type[sizeof(newsave->type) - 1] = '\0';

        VortexMaker::GetCurrentContext()->project_version = projectData["project"]["version"].get<std::string>();
        strncpy(newsave->version, VortexMaker::GetCurrentContext()->version.c_str(), sizeof(newsave->version));
        newsave->version[sizeof(newsave->version) - 1] = '\0';

        VortexMaker::GetCurrentContext()->include_system_templates = projectData["project"]["include_system_templates"].get<bool>();
        newsave->include_system_templates = VortexMaker::GetCurrentContext()->include_system_templates;

        current_save = newsave;
    }

    void ProjectSettingsAppWindow::Update()
    {
        std::string oldname = VortexMaker::GetCurrentContext()->name;

        std::string vortex_version = VORTEX_VERSION;

        nlohmann::json toolchainData;
        toolchainData["project"]["name"] = current_save->name;
        toolchainData["project"]["author"] = current_save->author;
        toolchainData["project"]["description"] = current_save->description;
        toolchainData["project"]["compatibleWith"] = vortex_version;
        toolchainData["project"]["type"] = current_save->type;
        toolchainData["project"]["version"] = current_save->version;
        toolchainData["project"]["include_system_templates"] = current_save->include_system_templates;

        std::string path = VortexMaker::GetCurrentContext()->projectPath.string();
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

        Refresh();

        VortexMaker::UpdateEnvironmentProject(oldname);
    }

} // namespace VortexEditor