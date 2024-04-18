#include "../PackageRenderInstance.h"
#include <array>
#include <random>

void PackageRenderInstance::UI_CompilationArguments()
{

    if (this->show_UI_CompilationArguments)
    {

     std::string label = this->name + " - Compilations Arguments###" +  this->name+ " - Compilations Arguments";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &editIcon, &this->show_UI_MainSettings, ImGuiWindowFlags_MenuBar);

        static int item_current = 0;
        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                this->Save();
            }
            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                this->Refresh();
            }

            const char *items[] = {"Configuration Prefixes", "Configuration Suffixes", "Compilation Prefixes", "Compilation Suffixes", "Installation Prefixes", "Installation Suffixes"};
            ImGui::Combo("Type", &item_current, items, IM_ARRAYSIZE(items));

            ImGui::Separator();
            if (ImGui::BeginMenu("Pannels"))
            {
                if (ImGui::MenuItem("Options Editor"))
                {
                }
                if (ImGui::MenuItem("Contents Window"))
                {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table", 3, flags))
        {
            ImGui::TableSetupColumn("Add a new argument", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            if (item_current == 0)
            {
                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->configurationPrefixes.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.second, 128);
                        }
                    }
                }
            }


            if (item_current == 1)
            {
                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->configurationSuffixes.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.second, 128);
                        }
                    }
                }
            }

            if (item_current == 2)
            {
                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->compilationPrefixes.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.second, 128);
                        }
                    }
                }
            }

            if (item_current == 3)
            {
                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->compilationSuffixes.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.second, 128);
                        }
                    }
                }
            }

            if (item_current == 4)
            {
                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->installationPrefixes.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.second, 128);
                        }
                    }
                }
            }

            if (item_current == 5)
            {
                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->installationSuffixes.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.second, 128);
                        }
                    }
                }
            }
            ImGui::EndTable();
        }

        if (ImGui::BeginTable("table", 3, flags))
        {
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("When", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Argument", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            if (item_current == 0)
            {
                for (int row = 0; row < this->m_currentSave->configurationPrefixes.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->configurationPrefixes[row], this->m_currentSave->configurationPrefixes.back());
                                this->m_currentSave->configurationPrefixes.pop_back();
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-when";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->configurationPrefixes[row].first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-arg";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->configurationPrefixes[row].second, 128);
                        }
                    }
                }
            }

            if (item_current == 1)
            {
                for (int row = 0; row < this->m_currentSave->configurationSuffixes.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->configurationSuffixes[row], this->m_currentSave->configurationSuffixes.back());
                                this->m_currentSave->configurationSuffixes.pop_back();
                            }
                        }

                        if (column == 1)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-when";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->configurationSuffixes[row].first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-arg";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->configurationSuffixes[row].second, 128);
                        }
                    }
                }
            }


            if (item_current == 2)
            {
                for (int row = 0; row < this->m_currentSave->compilationPrefixes.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::cout << "sfdljkhj" << std::endl;
                                std::swap(this->m_currentSave->compilationPrefixes[row], this->m_currentSave->compilationPrefixes.back());
                                this->m_currentSave->compilationPrefixes.pop_back();
                            }
                        }

                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->compilationPrefixes[row].first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->compilationPrefixes[row].second, 128);
                        }
                    }
                }
            }

            if (item_current == 3)
            {
                for (int row = 0; row < this->m_currentSave->compilationSuffixes.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->compilationSuffixes[row], this->m_currentSave->compilationSuffixes.back());
                                this->m_currentSave->compilationSuffixes.pop_back();
                            }
                        }

                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->compilationSuffixes[row].first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->compilationSuffixes[row].second, 128);
                        }
                    }
                }
            }

            if (item_current == 4)
            {
                for (int row = 0; row < this->m_currentSave->installationPrefixes.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->installationPrefixes[row], this->m_currentSave->installationPrefixes.back());
                                this->m_currentSave->installationPrefixes.pop_back();
                            }
                        }

                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->installationPrefixes[row].first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->installationPrefixes[row].second, 128);
                        }
                    }
                }
            }

            if (item_current == 5)
            {
                for (int row = 0; row < this->m_currentSave->installationSuffixes.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->installationSuffixes[row], this->m_currentSave->installationSuffixes.back());
                                this->m_currentSave->installationSuffixes.pop_back();
                            }
                        }

                        if (column == 1)
                        {
                            std::string label = "When ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->installationSuffixes[row].first, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Arg ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->installationSuffixes[row].second, 128);
                        }
                    }
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}
