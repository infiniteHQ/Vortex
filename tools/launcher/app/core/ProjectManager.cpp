
#include "ProjectManager.hpp"

static std::vector<std::string> labels = {"Open a project", "Create a new project"};
static int selected = 0;
static int selected_template = 0;
static bool template_is_selected = false;
static std::shared_ptr<TemplateInterface> selected_template_object;
static std::string title = "none";

// Fonction pour charger le contenu hexadécimal d'un fichier .png
static std::vector<uint8_t> loadPngHex(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Erreur lors de l'ouverture du fichier. -> " + filePath << std::endl;
        return {};
    }

    // Déterminez la taille du fichier
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Créez un vecteur pour stocker le contenu hexadécimal
    std::vector<uint8_t> hexContent(fileSize);

    // Lisez le contenu hexadécimal du fichier
    file.read(reinterpret_cast<char *>(hexContent.data()), fileSize);

    return hexContent;
}
static std::vector<std::shared_ptr<UIKit::Image>> logos;
static std::vector<ImTextureID> textures;
static std::vector<std::shared_ptr<std::pair<std::string, int>>> labels_indexes;
static std::vector<std::string> syslabels;

static void logo(const std::string &path, int index, int total)
{
    uint32_t w, h;
    // Chargez le contenu hexadécimal du fichier .png
    std::vector<uint8_t> hexTable = loadPngHex(path);
    const uint8_t *hexData = hexTable.data();

    if (total > logos.size())
    {
        void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
        std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        logos.push_back(_icon);
        VX_FREE(data);
        ImTextureID addIcon = logos[index]->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        textures.push_back(addIcon);
    }

    if (index <= textures.size())
    {
        ImGui::Image(textures[index], ImVec2(50, 50));
    }
}

ProjectManager::ProjectManager(VxContext *_ctx)
{
    this->ctx = _ctx;

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

void ProjectManager::OnImGuiRender()
{
    static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::Begin("Project managers", &listIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        this->menubar();

    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), title.c_str());

    ImGui::GetFont()->Scale = oldsize;
    ImGui::PopFont();

    oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 0.9;
    ImGui::PushFont(ImGui::GetFont());

    std::string label = "Vortex version ";
    label += ctx->version.c_str();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
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
    static int _i;
    if (selected == 0)
    {
        title = "Open a project";

        ImGuiID _id = ImGui::GetID("_main");
        ImGui::BeginChildFrame(_id, ImVec2(0, 0), true);
        for (int row = 0; row < this->ctx->IO.sys_projects.size(); row++)
        {
            std::string label = this->ctx->IO.sys_projects[row]->name + "###" + std::to_string(row);
            ImGuiID id = ImGui::GetID(label.c_str());
            ImGui::BeginChild(id, ImVec2(0, 200), true);

            {
                ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                if (!this->ctx->IO.sys_projects[row]->logoPath.empty())
                {
                    logo(this->ctx->IO.sys_projects[row]->logoPath, row, this->ctx->IO.sys_projects.size());
                }

                ImGui::EndChild();
                ImGui::SameLine();
            }

            {
                ImGuiID _id = ImGui::GetID("TEXT_");
                ImGui::BeginChild(_id, ImVec2(0, 100), true);
                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), this->ctx->IO.sys_projects[row]->name.c_str());

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Last opened ");
                ImGui::SameLine();
                ImGui::Text(this->ctx->IO.sys_projects[row]->lastOpened.c_str());

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) ");
                ImGui::SameLine();
                ImGui::Text(this->ctx->IO.sys_projects[row]->author.c_str());

                ImGui::EndChild();
            }

            if (ImGui::Button("Open"))
            {
                std::string projectPath = this->ctx->IO.sys_projects[row]->path;

                std::thread([projectPath]()
                            {
        std::string cmd = "cd " + projectPath + " && vortex -e";
        system(cmd.c_str()); })
                    .detach();
            }

            ImGui::EndChildFrame();
        }
    }
    else if (selected == 1 && template_is_selected == false)
    {
        title = "Create a new project";
        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

        ImGui::Columns(2, NULL);

        for (int i = 0; i <= project_templates.size(); i++)
        {
            _i++;
            if (project_templates[i])
            {
                {
                    ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                    try
                    {

                        if (!project_templates[i]->m_logo_path.empty())
                        {
                            logo(project_templates[i]->m_logo_path, i, project_templates.size());
                        }
                    }
                    catch (std::exception e)
                    {
                        std::cout << e.what() << std::endl;
                        sleep(5);
                    }
                    ImGui::EndChild();
                    ImGui::SameLine();
                }

                {
                    ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                    float oldsize = ImGui::GetFont()->Scale;
                    ImGui::GetFont()->Scale *= 1.3;
                    ImGui::PushFont(ImGui::GetFont());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), project_templates[i]->m_proper_name.c_str());

                    ImGui::GetFont()->Scale = oldsize;
                    ImGui::PopFont();

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                    ImGui::SameLine();
                    ImGui::Text(project_templates[i]->m_author.c_str());
                    ImGui::EndChild();
                }

                ImGui::TextWrapped(project_templates[i]->m_description.c_str());

                if (ImGui::Button("Create"))
                {
                    template_is_selected = true;
                    selected_template_object = project_templates[i];
                }
            }
            ImGui::NextColumn();
        }
        ImGui::EndChild();
    }
    else if (selected == 1 && template_is_selected == true)
    {
        title = "Create a new project based on " + selected_template_object->m_proper_name;
        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static char buf[255]{};
        std::string s{VortexMaker::getHomeDirectory() + "/VortexProjects/"};

        strncpy(buf, s.c_str(), sizeof(buf) - 1);

        static char name[128] = "UnknowName";
        static char desc[128] = "My project description !";
        static char version[128] = "1.0.0";
        static char auth[128] = "you";
        static bool open = true;
        {
            ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
            if (!selected_template_object->m_logo_path.empty())
            {
                logo(selected_template_object->m_logo_path, _i++, project_templates.size());
            }

            ImGui::EndChild();
            ImGui::SameLine();
        }

        {
            ImGui::BeginChild("TEXT_", ImVec2(0, 68), true);
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), name);
            ImGui::SameLine();

            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();
            static std::string _name = " based on " + selected_template_object->m_proper_name;
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), _name.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "from");
            ImGui::SameLine();
            ImGui::Text(auth);
            ImGui::EndChild();
        }

        ImGui::InputText("Path", buf, sizeof(buf));
        ImGui::InputText("Name", name, 128);
        ImGui::InputText("Description", desc, 128);
        ImGui::InputText("Authors", auth, 128);
        ImGui::InputText("Version", version, 128);

        ImGui::Separator();
        float oldsize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 1.2;
        ImGui::PushFont(ImGui::GetFont());
        ImGui::Text("About the template used : ");
        ImGui::GetFont()->Scale = oldsize;
        ImGui::PopFont();
        ImGui::Text("Name & Version : ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), selected_template_object->m_proper_name.c_str());
        ImGui::Text("Description : ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), selected_template_object->m_description.c_str());
        ImGui::Separator();

        ImGui::Checkbox("Open the project after creation", &open);
        std::string path = s + name;

        ImGui::Text("This new project will be install in "); // Fix path and allow project creation + fix core dumped
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), path.c_str());

        if (ImGui::Button("Back"))
        {
            template_is_selected = false;
        }
        ImGui::SameLine();

        if (ImGui::ImageButtonWithText(addIcon, "Create", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            VortexMaker::CreateProject(name, auth, version, desc, path, selected_template_object->m_name);
        }
        ImGui::EndChild();
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

void ProjectManager::addModuleModal()
{
}

void ProjectManager::mainButtonsMenuItem()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::ImageButtonWithText(addIcon, "Create", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
    {
        selected = 1;
    }
    if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
    {
        this->Refresh();
    }
}

void ProjectManager::filterMenuItem()
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

void ProjectManager::createMenuItem()
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

void ProjectManager::searchMenuItem()
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

void ProjectManager::menubar()
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

void ProjectManager::Refresh()
{
    project_templates.clear();
    for (auto tem : this->ctx->IO.sys_templates)
    {
        if (tem->m_type == "project")
        {
            project_templates.push_back(tem);
        }
    }
};
