
#include "ModuleManager.hpp"
#include <optional>

static int item_current = 0;    
static bool open_ADDMODULE = false;
static std::string s_Parent;


// Left
static int mb_selected = 0;
static std::vector<std::string> mb_syslabels;

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

// Left
static int selected = 0;
static std::vector<std::string> labels;
static std::vector<std::shared_ptr<std::pair<std::string, int>>> labels_indexes;
static std::vector<std::string> syslabels;

void incrementValue(const std::string &str)
{
    for (auto index : labels_indexes)
    {
        if (index->first == str)
        {
            index->second++;
        }
    }
}

void resetValue(const std::string &str)
{
    for (auto index : labels_indexes)
    {
        if (index->first == str)
        {
            index->second = 0;
        }
    }
}

int getIndex(const std::string &str)
{
    for (auto index : labels_indexes)
    {
        if (index->first == str)
        {
            return index->second;
        }
    }
    return -1;
}

static void logo(const std::string &path, std::string index_group, int total)
{
    uint32_t w, h;
    // Chargez le contenu hexadécimal du fichier .png
    std::vector<uint8_t> hexTable = loadPngHex(path);
    const uint8_t *hexData = hexTable.data();

    int index = getIndex(index_group);

    if(index == -1)
    return;

    if (total > logos.size())
    {
        void *data = UIKit::Image::Decode(hexData, hexTable.size(), w, h);
        std::shared_ptr<UIKit::Image> _icon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data); // ML
        logos.push_back(_icon);
        VX_FREE(data);
        ImTextureID addIcon = logos[index]->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        textures.push_back(addIcon);
    }

    ImGui::Image(textures[index], ImVec2(50, 50));
}

ModuleManager::ModuleManager(VxContext *_ctx, InstanceFactory *_factory, const std::string &parent)
{
    this->ctx = _ctx;
    this->factory = _factory;

    s_Parent = parent;

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_stop, icons::i_stop_size, w, h);
        m_StopIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_start, icons::i_start_size, w, h);
        m_StartIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_logs, icons::i_logs_size, w, h);
        m_LogsIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_module, icons::i_module_size, w, h);
        m_ModuleIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, s_Parent, data);
        free(data);
    }
}

void ModuleManager::OnImGuiRender(const std::string& parent, std::function<void(ImGuiWindow*)> controller)
{
    static ImTextureID listIcon = this->m_ListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID moduleIcon = this->m_ModuleIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID startIcon = this->m_StartIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID stopIcon = this->m_StopIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID logIcon = this->m_LogsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (ImGui::Begin("Modules manager", &moduleIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        this->menubar();

        static ImGuiWindow *win = ImGui::GetCurrentWindow();
        this->m_Parent = s_Parent;
        controller(win);

    float oldsize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 1.3;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module of project(s)");
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

    for (auto em : ctx->IO.em)
    {
        bool add = true;

        for (auto label : labels)
        {
            if (label == em->m_group)
            {
                add = false;
            }
        }

        if (add)
        {
            std::shared_ptr<std::pair<std::string, int>> new_index = std::make_shared<std::pair<std::string, int>>();
            new_index->first = em->m_group;
            new_index->second = 0;

            labels.push_back(em->m_group);
            labels_indexes.push_back(new_index);
        }
    }

    for (auto em : ctx->IO.sys_em)
    {
        bool add = true;

        for (auto label : syslabels)
        {
            if (label == em->m_group)
            {
                add = false;
            }
        }

        if (add)
        {
            syslabels.push_back(em->m_group);
        }
    }

    {
        ImGui::BeginChild("left pane", ImVec2(230, -1), true);
        for (int i = 0; i < labels.size(); i++)
        {
            if (i == 0)
            {
                std::string label = "All modules (" + std::to_string(this->ctx->IO.em.size()) + ")";
                ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
            }

            std::string label;

            int number = 0;
            for (auto em : this->ctx->IO.em)
            {
                if (em->m_group == labels[i])
                {
                    number++;
                }
            }
            label = labels[i];
            label += " (";
            label += std::to_string(number);
            label += ")";

            label += "##";
            label += labels[i];

            if (ImGui::Selectable(label.c_str(), selected == i))
                selected = i;
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();

    std::string label = "packhhqsdsdageView###";
    ImGuiID id = ImGui::GetID(label.c_str());
    ImGui::BeginChildFrame(id, ImVec2(0, 0), true);

    for (int i = 0; i < ctx->IO.em.size(); i++)
    {
        resetValue(ctx->IO.em[i]->m_group);
    }
    
    for (int i = 0; i < ctx->IO.em.size(); i++)
    {
        if (ctx->IO.em[i]->m_group == labels[selected])
        {
            std::string childLabel = "module##" + ctx->IO.em[i]->m_name;

            ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

            {
                ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                logo(ctx->IO.em[i]->m_logo_path, ctx->IO.em[i]->m_group, this->ctx->IO.em.size());

                ImGui::EndChild();
                ImGui::SameLine();
            }

            {
                ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ctx->IO.em[i]->m_proper_name.c_str());

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_version.c_str());
                ImGui::EndChild();
            }

            if (ctx->IO.em[i]->m_state == "running")
            {
                ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 0.8f), "Running");
            }
            if (ctx->IO.em[i]->m_state == "failed")
            {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 0.8f), "Failed");
            }
            if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state == "stopped")
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Not running");
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_name.c_str());
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_author.c_str());
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                ImGui::SameLine();
                ImGui::Text(ctx->IO.em[i]->m_description.c_str());
            }

            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                int contrib = 0;
                for (auto contributor : ctx->IO.em[i]->m_contributors)
                {
                    contrib++;
                    if (contrib <= 3)
                    {
                        if (contrib < ctx->IO.em[i]->m_contributors.size())
                        {
                            ImGui::SameLine();
                            std::string contri = contributor + ",";
                            ImGui::Text(contri.c_str());
                        }
                        else
                        {
                            ImGui::SameLine();
                            ImGui::Text(contributor.c_str());
                        }
                    }
                    else
                    {
                        ImGui::SameLine();
                        int counter = ctx->IO.em[i]->m_contributors.size() - 3;
                        std::string label = " and " + std::to_string(counter) + " other...";
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                        break;
                    }
                }
            }

            if (ctx->IO.em[i]->m_state == "running")
            {
                if (ImGui::ImageButtonWithText(stopIcon, "Stop", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Stop();
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(logIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                    this->factory->SpawnInstance(instance);
                }
            }

            if (ctx->IO.em[i]->m_state == "failed")
            {
                if (ImGui::ImageButtonWithText(listIcon, "Retry to launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Start();
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(logIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                    this->factory->SpawnInstance(instance);
                }
            }
            if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state == "stopped")
            {
                if (ImGui::ImageButtonWithText(startIcon, "Launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    ctx->IO.em[i]->Start();
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                }
                ImGui::SameLine();
                if (ImGui::ImageButtonWithText(logIcon, "Details", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    // Behavior
                    std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
                    this->factory->SpawnInstance(instance);
                }
            }

            incrementValue(ctx->IO.em[i]->m_group);
            ImGui::EndChild();
        }
    }

    ImGui::EndChildFrame();
    ImGui::End();
}

// Helper functions for menu items

static void handleRefresh()
{
    // Behavior
}

static void handleAddToProject(const std::string &name, const std::string &version, bool &restart_modules)
{
    VortexMaker::InstallModule(name, version, restart_modules);
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

void ModuleManager::addModuleModal()
{
}

// Main menu function

void ModuleManager::menubar()
{
    static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    for (auto em : ctx->IO.sys_em)
    {
        bool add = true;

        for (auto label : mb_syslabels)
        {
            if (label == em->m_group)
            {
                add = false;
            }
        }

        if (add)
        {
            mb_syslabels.push_back(em->m_group);
        }
    }

    if (open_ADDMODULE)
    {
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (ImGui::BeginPopupModal("Add module(s)", NULL, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::Button("Close"))
                {
                    open_ADDMODULE = false;
                }
                if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                {
                    handleRefresh();
                }
                ImGui::Separator();
                if (ImGui::ImageButtonWithText(addIcon, "Browse public modules", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
                {
                    // Behavior
                }
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
                ImGui::EndMenuBar();
            }

            ImGui::BeginChild("left pane", ImVec2(230, -1), true);
            for (int i = 0; i < mb_syslabels.size(); i++)
            {
                if (i == 0)
                {
                    std::string label = "All modules (" + std::to_string(this->ctx->IO.sys_em.size()) + ")";
                    ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
                }

                std::string label;

                int number = 0;
                for (auto sysem : this->ctx->IO.sys_em)
                {
                    if (sysem->m_group == mb_syslabels[i])
                    {
                        number++;
                    }
                }
                label = mb_syslabels[i];
                label += " (";
                label += std::to_string(number);
                label += ")";

                label += "##";
                label += mb_syslabels[i];

                if (ImGui::Selectable(label.c_str(), mb_selected == i))
                    mb_selected = i;
            }
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::Separator();
            ImGui::SameLine();

            std::string label = "packhhqsdsdageView###";
            ImGuiID id = ImGui::GetID(label.c_str());
            ImGui::BeginChildFrame(id, ImVec2(0, 0), true);

            for (int i = 0; i < ctx->IO.sys_em.size(); i++)
            {
                if (ctx->IO.sys_em[i]->m_group == mb_syslabels[mb_selected])
                {
                    std::string childLabel = "module##" + ctx->IO.sys_em[i]->m_name + ctx->IO.sys_em[i]->m_version;

                    ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

                    {
                        ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                        logo(ctx->IO.sys_em[i]->m_logo_path, ctx->IO.sys_em[i]->m_group, this->ctx->IO.sys_em.size());
                        ImGui::EndChild();
                        ImGui::SameLine();
                    }

                    {
                        ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                        float oldsize = ImGui::GetFont()->Scale;
                        ImGui::GetFont()->Scale *= 1.3;
                        ImGui::PushFont(ImGui::GetFont());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), ctx->IO.sys_em[i]->m_proper_name.c_str());

                        ImGui::GetFont()->Scale = oldsize;
                        ImGui::PopFont();

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_version.c_str());
                        ImGui::EndChild();
                    }

                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_name.c_str());
                    }
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_author.c_str());
                    }
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                        ImGui::SameLine();
                        ImGui::Text(ctx->IO.sys_em[i]->m_description.c_str());
                    }
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                        int contrib = 0;
                        for (auto contributor : ctx->IO.sys_em[i]->m_contributors)
                        {
                            contrib++;
                            if (contrib <= 3)
                            {
                                if (contrib < ctx->IO.sys_em[i]->m_contributors.size())
                                {
                                    ImGui::SameLine();
                                    std::string contri = contributor + ",";
                                    ImGui::Text(contri.c_str());
                                }
                                else
                                {
                                    ImGui::SameLine();
                                    ImGui::Text(contributor.c_str());
                                }
                            }
                            else
                            {
                                ImGui::SameLine();
                                int counter = ctx->IO.sys_em[i]->m_contributors.size() - 3;
                                std::string label = " and " + std::to_string(counter) + " other...";
                                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                                break;
                            }
                        }
                    }

                    static bool restart_modules = false;

                    {
                        bool exist = false;
                        for (auto em : ctx->IO.em)
                        {
                            if (ctx->IO.sys_em[i]->m_name == em->m_name && ctx->IO.sys_em[i]->m_version == em->m_version)
                            {
                                exist = true;
                            }
                        }

                        if (exist)
                        {
                            ImGui::BeginDisabled();
                            if (ImGui::ImageButtonWithText(addIcon, "Already in the project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                            {
                                // Behavior
                            }
                            ImGui::EndDisabled();
                        }
                        else
                        {
                            ImGui::Checkbox("Restart modules, and launch this module directly", &restart_modules);
                            if (ImGui::ImageButtonWithText(addIcon, "Add to the current project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                            {
                                handleAddToProject(ctx->IO.sys_em[i]->m_name, ctx->IO.sys_em[i]->m_version, restart_modules);
                            }
                        }
                    }

                    ImGui::EndChild();
                }
            }
            ImGui::EndChildFrame();
            ImGui::EndPopup();
        }
    }

    if (open_ADDMODULE)
        ImGui::OpenPopup("Add module(s)");

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
        {
            open_ADDMODULE = true;
        }

        if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
        {
            handleRefresh();
        }
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
        ImGui::EndMenuBar();
    }
}
