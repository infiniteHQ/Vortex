#include "modules_manager.hpp"
#include <optional>

using namespace Cherry;

static int item_current = 0;
static bool open_ADDMODULE = false;
static std::string s_Parent;

static int mb_selected = 0;
static std::vector<std::string> mb_syslabels;

static std::vector<uint8_t> loadPngHex(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "Erreur lors de l'ouverture du fichier. -> " + filePath << std::endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> hexContent(fileSize);

    file.read(reinterpret_cast<char *>(hexContent.data()), fileSize);

    return hexContent;
}
static std::vector<std::shared_ptr<Cherry::Image>> logos;
static std::vector<ImTextureID> textures;

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

    if (index == -1)
        return;

    if (total > logos.size())
    {
        void *data = Cherry::Image::Decode(hexData, hexTable.size(), w, h);
        std::shared_ptr<Cherry::Image> _icon = std::make_shared<Cherry::Image>(w, h, Cherry::ImageFormat::RGBA, s_Parent, data); // ML
        logos.push_back(_icon);
        VX_FREE(data);
        ImTextureID addIcon = logos[index]->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        textures.push_back(addIcon);
    }

    ImGui::Image(textures[index], ImVec2(50, 50));
}

namespace VortexEditor
{
    ModuleManagerAppWindow::ModuleManagerAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
        m_AppWindow->SetClosable(false);

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);
    }

    std::shared_ptr<Cherry::AppWindow> &ModuleManagerAppWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<ModuleManagerAppWindow> ModuleManagerAppWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<ModuleManagerAppWindow>(new ModuleManagerAppWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void ModuleManagerAppWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void ModuleManagerAppWindow::Render()
    {
        float oldsize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= 1.3;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module of project(s)");
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

        for (auto em : VortexMaker::GetCurrentContext()->IO.em)
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

        for (auto em : VortexMaker::GetCurrentContext()->IO.sys_em)
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
                    std::string label = "All modules (" + std::to_string(VortexMaker::GetCurrentContext()->IO.em.size()) + ")";
                    ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
                }

                std::string label;

                int number = 0;
                for (auto em : VortexMaker::GetCurrentContext()->IO.em)
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

        for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size(); i++)
        {
            resetValue(VortexMaker::GetCurrentContext()->IO.em[i]->m_group);
        }

        for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size(); i++)
        {
            if (VortexMaker::GetCurrentContext()->IO.em[i]->m_group == labels[selected])
            {
                std::string childLabel = "module##" + VortexMaker::GetCurrentContext()->IO.em[i]->m_name;

                ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

                {
                    ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                    logo(VortexMaker::GetCurrentContext()->IO.em[i]->m_logo_path, VortexMaker::GetCurrentContext()->IO.em[i]->m_group, VortexMaker::GetCurrentContext()->IO.em.size());

                    ImGui::EndChild();
                    ImGui::SameLine();
                }

                {
                    ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                    float oldsize = ImGui::GetFont()->Scale;
                    ImGui::GetFont()->Scale *= 1.3;
                    ImGui::PushFont(ImGui::GetFont());

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), VortexMaker::GetCurrentContext()->IO.em[i]->m_proper_name.c_str());

                    ImGui::GetFont()->Scale = oldsize;
                    ImGui::PopFont();

                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                    ImGui::SameLine();
                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.em[i]->m_version.c_str());
                    ImGui::EndChild();
                }

                if (VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "running")
                {
                    ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 0.8f), "Running");
                }
                if (VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "failed")
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 0.8f), "Failed");
                }
                if (VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "unknow" || VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "stopped")
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Not running");
                }

                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                    ImGui::SameLine();
                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.em[i]->m_name.c_str());
                }

                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                    ImGui::SameLine();
                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.em[i]->m_author.c_str());
                }

                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                    ImGui::SameLine();
                    ImGui::Text(VortexMaker::GetCurrentContext()->IO.em[i]->m_description.c_str());
                }

                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                    int contrib = 0;
                    for (auto contributor : VortexMaker::GetCurrentContext()->IO.em[i]->m_contributors)
                    {
                        contrib++;
                        if (contrib <= 3)
                        {
                            if (contrib < VortexMaker::GetCurrentContext()->IO.em[i]->m_contributors.size())
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
                            int counter = VortexMaker::GetCurrentContext()->IO.em[i]->m_contributors.size() - 3;
                            std::string label = " and " + std::to_string(counter) + " other...";
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                            break;
                        }
                    }
                }

                if (VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "running")
                {
                    if (ImGui::Button("Stop"))
                    {
                        VortexMaker::GetCurrentContext()->IO.em[i]->Stop();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Details"))
                    {
                        // Behavior
                        // std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(VortexMaker::GetCurrentContext(), VortexMaker::GetCurrentContext()->IO.em[i]);
                        // this->factory->SpawnInstance(instance);
                    }
                }

                if (VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "failed")
                {
                    /*if (ImGui::ImageButtonWithText(listIcon, "Retry to launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                    {
                        VortexMaker::GetCurrentContext()->IO.em[i]->Start();
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
                        std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(VortexMaker::GetCurrentContext(), VortexMaker::GetCurrentContext()->IO.em[i]);
                        this->factory->SpawnInstance(instance);
                    }*/
                }
                if (VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "unknow" || VortexMaker::GetCurrentContext()->IO.em[i]->m_state == "stopped")
                {
                    /*if (ImGui::ImageButtonWithText(startIcon, "Launch", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                    {
                        VortexMaker::GetCurrentContext()->IO.em[i]->Start();
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
                        std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(VortexMaker::GetCurrentContext(), VortexMaker::GetCurrentContext()->IO.em[i]);
                        this->factory->SpawnInstance(instance);
                    }*/
                }

                incrementValue(VortexMaker::GetCurrentContext()->IO.em[i]->m_group);
                ImGui::EndChild();
            }
        }

        ImGui::EndChildFrame();
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

    void ModuleManagerAppWindow::addModuleModal()
    {
        //
    }

    void ModuleManagerAppWindow::menubar()
    {
        for (auto em : VortexMaker::GetCurrentContext()->IO.sys_em)
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
            if (ImGui::BeginPopupModal("Add module(s)", NULL, ImGuiWindowFlags_MenuBar))
            {
                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::Button("Close"))
                    {
                        open_ADDMODULE = false;
                    }
                    /* TODO if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                    {
                        handleRefresh();
                    }
                    ImGui::Separator();
                    if (ImGui::ImageButtonWithText(addIcon, "Browse public modules", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
                    {
                        // Behavior
                    }*/
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
                        std::string label = "All modules (" + std::to_string(VortexMaker::GetCurrentContext()->IO.sys_em.size()) + ")";
                        ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), label.c_str());
                    }

                    std::string label;

                    int number = 0;
                    for (auto sysem : VortexMaker::GetCurrentContext()->IO.sys_em)
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

                for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.sys_em.size(); i++)
                {
                    if (VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_group == mb_syslabels[mb_selected])
                    {
                        std::string childLabel = "module##" + VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_name + VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_version;

                        ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

                        {
                            ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);
                            logo(VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_logo_path, VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_group, VortexMaker::GetCurrentContext()->IO.sys_em.size());
                            ImGui::EndChild();
                            ImGui::SameLine();
                        }

                        {
                            ImGui::BeginChild("TEXT_", ImVec2(220, 68), true);
                            float oldsize = ImGui::GetFont()->Scale;
                            ImGui::GetFont()->Scale *= 1.3;
                            ImGui::PushFont(ImGui::GetFont());

                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.8f), VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_proper_name.c_str());

                            ImGui::GetFont()->Scale = oldsize;
                            ImGui::PopFont();

                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "v");
                            ImGui::SameLine();
                            ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_version.c_str());
                            ImGui::EndChild();
                        }

                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Name : ");
                            ImGui::SameLine();
                            ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_name.c_str());
                        }
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author(s) : ");
                            ImGui::SameLine();
                            ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_author.c_str());
                        }
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Description : ");
                            ImGui::SameLine();
                            ImGui::Text(VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_description.c_str());
                        }
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Contributors : ");
                            int contrib = 0;
                            for (auto contributor : VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_contributors)
                            {
                                contrib++;
                                if (contrib <= 3)
                                {
                                    if (contrib < VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_contributors.size())
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
                                    int counter = VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_contributors.size() - 3;
                                    std::string label = " and " + std::to_string(counter) + " other...";
                                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), label.c_str());
                                    break;
                                }
                            }
                        }

                        static bool restart_modules = false;

                        {
                            bool exist = false;
                            for (auto em : VortexMaker::GetCurrentContext()->IO.em)
                            {
                                if (VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_name == em->m_name && VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_version == em->m_version)
                                {
                                    exist = true;
                                }
                            }

                            if (exist)
                            {
                                /*ImGui::BeginDisabled();
                                if (ImGui::ImageButtonWithText(addIcon, "Already in the project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                                {
                                    // Behavior
                                }
                                ImGui::EndDisabled();*/
                            }
                            else
                            {
                                /*ImGui::Checkbox("Restart modules, and launch this module directly", &restart_modules);
                                if (ImGui::ImageButtonWithText(addIcon, "Add to the current project", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
                                {
                                    handleAddToProject(VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_name, VortexMaker::GetCurrentContext()->IO.sys_em[i]->m_version, restart_modules);
                                }*/
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
            /*if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
            {
                open_ADDMODULE = true;
            }

            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
            {
                handleRefresh();
            }*/
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

} // namespace VortexEditor