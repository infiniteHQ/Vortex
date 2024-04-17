#include "../PackageRenderInstance.h"
#include <array>
#include <random>

void PackageRenderInstance::UI_ActionsEditor()
{

    if (this->show_UI_ActionsEditor)
    {

     std::string label = this->name + " - Actions Editor###" +  this->name +  " - Actions Editor";
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

        if (ImGui::BeginTable("table", 5, flags))
        {
            ImGui::TableSetupColumn("Add a new argument", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

                for (int row = 0; row < 1; row++)
                {
                    static PackageActionSave_ newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 5; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->actions.push_back(newItem);
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "Sequence ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.sequence, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "Type ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.type, 128);
                        }

                        if (column == 3)
                        {
                            std::string label = "Priority ";
                            label += std::to_string(row);
                            ImGui::InputInt(label.c_str(), &newItem.priority);
                        }

                        if (column == 4)
                        {
                            std::string label = "Command ";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), newItem.command, 128);
                        }
                    }
                
            }
            ImGui::EndTable();
        }

        if (ImGui::BeginTable("table", 5, flags))
        {
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Sequence", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Command", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

                for (int row = 0; row < this->m_currentSave->actions.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 5; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->actions[row], this->m_currentSave->actions.back());
                                this->m_currentSave->actions.pop_back();
                            }
                        }
                        if (column == 1)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-sequence";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->actions[row].sequence, 128);
                        }

                        if (column == 2)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-priority";
                            label += std::to_string(row);
                            ImGui::InputInt(label.c_str(), &this->m_currentSave->actions[row].priority);
                        }
                        if (column == 3)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-type";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->actions[row].type, 128);
                        }
                        if (column == 4)
                        {
                            std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-command";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), this->m_currentSave->actions[row].command, 128);
                        }
                    }
                
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}
