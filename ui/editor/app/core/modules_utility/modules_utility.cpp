#include "./modules_utility.hpp"

using namespace Cherry;

static int item_current = 0;
static bool open_ADDMODULE = false;
static std::string s_Parent;

static int mb_selected = 0;
static std::vector<std::string> mb_syslabels;

namespace VortexEditor
{
    void ShowSystemModules(const std::string& filter = "none")
    {
            for (int row = 0; row < m_VortexRegisteredVersions.size(); row++)
            {
                if (areStringsSimilar(m_VortexRegisteredVersions[row]->m_VersionName, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch))
                {
                    VersionButton(m_VortexRegisteredVersions[row]->m_VersionName, 300, 100, m_VortexRegisteredVersions[row]->m_Version);
                }
            }
    }
    
    void ShowModules(const std::string& filter = "none")
    {
/*for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size(); i++)
        {
            resetValue(VortexMaker::GetCurrentContext()->IO.em[i]->m_group);
        }*/

        for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size(); i++)
        {
            if (VortexMaker::GetCurrentContext()->IO.em[i]->m_group == labels[selected])
            {
                std::string childLabel = "module##" + VortexMaker::GetCurrentContext()->IO.em[i]->m_name;

                ImGui::BeginChild(childLabel.c_str(), ImVec2(0, 250), true);

                {
                    ImGui::BeginChild("LOGO_", ImVec2(70, 70), true);

                    ImGui::Image(Cherry::GetTexture(Cherry::GetPath(VortexMaker::GetCurrentContext()->IO.em[i]->m_logo_path)), ImVec2(15,15));

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
                        //std::shared_ptr<ModuleDetails> instance = std::make_shared<ModuleDetails>(VortexMaker::GetCurrentContext(), VortexMaker::GetCurrentContext()->IO.em[i]);
                        //this->factory->SpawnInstance(instance);
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

                //incrementValue(VortexMaker::GetCurrentContext()->IO.em[i]->m_group);
                ImGui::EndChild();
            }
        }
    }
    
    ModulesUtilityAppWindow::ModulesUtilityAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
        m_AppWindow->SetClosable(false);
        RegisterAvailableVersions();

        m_AppWindow->m_TabMenuCallback = []()
        {
            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                ImGui::Text("Main stuff");
                ImGui::PopStyleColor();

                ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                ImGui::Separator();
                ImGui::PopStyleColor();

                if (ImGui::MenuItem("Logs Utility", "Overview of all logs"))
                {
                }

                if (ImGui::MenuItem("Logs2 Utility", "Overview of all logs"))
                {
                }

                ImGui::EndMenu();
            }
        };

        m_AppWindow->SetInternalPaddingX(10.0f);
        m_AppWindow->SetInternalPaddingY(10.0f);

        m_SelectedChildName = "?loc:loc.window_names.welcome.overview";



        this->AddChild("All modules in project", "All modules (453)", [this]()
                       {
                        ShowSystemModules();
                       });

        this->AddChild("All modules in project (groups)", "Visual Development (21)", [this]()
                       {
                        ShowModules();
                       });

        this->AddChild("Import/Install", "Import a module from system", [this]()
                       {
                        ShowModules();
                        
                       });
                       

        std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
    }

    void ModulesUtilityAppWindow::AddChild(const std::string &parent_name, const std::string &child_name, const std::function<void()> &child)
    {
        m_Childs.push_back(ModulesUtilityAppWindowChild(parent_name, child_name, child));
    }

    void ModulesUtilityAppWindow::RemoveChild(const std::string &child_name)
    {
    }

    std::function<void()> ModulesUtilityAppWindow::GetChild(const std::string &child_name)
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


    void ModulesUtilityAppWindow::RefreshRender(const std::shared_ptr<ModulesUtilityAppWindow> &instance)
    {
        m_AppWindow->SetRenderCallback([instance]()
                                       {
                                           static float leftPaneWidth = 300.0f;
                                           const float minPaneWidth = 50.0f;
                                           const float splitterWidth = 1.5f;
                                           static int selected;


      std::map<std::string, std::vector<ModulesUtilityAppWindowChild>> groupedByParent;
                                       for (const auto &child : instance->m_Childs)
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
                                               if (child.m_ChildName == instance->m_SelectedChildName)
                                               {
                                                   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                                               }
                                               else
                                               {
                                                   ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                                               }

                                               if (TextButtonUnderline(child.m_ChildName.c_str()))
                                               {
                                                   instance->m_SelectedChildName = child.m_ChildName;
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

                                           if(!instance->m_SelectedChildName.empty())
                                           {
                                                std::function<void()> pannel_render = instance->GetChild(instance->m_SelectedChildName);
                                                if(pannel_render)
                                                {
                                                    pannel_render();
                                                }
                                           }
                                        
                                           ImGui::EndGroup(); });
    }

}
