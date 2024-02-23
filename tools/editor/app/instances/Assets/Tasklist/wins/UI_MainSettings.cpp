#include "../TasklistInstance.h"
#include <array>
#include <random>

void TasklistInstance::UI_MainSettings()
{

    if (this->show_UI_MainSettings)
    {

        static std::string label = this->name + " - Main Settings###" +  this->name;
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);    
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID listIcon = this->m_TaskListIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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

        if (ImGui::BeginTable("table", 4, flags))
        {
            ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

                for (int row = 0; row < 1; row++)
                {
                    static TaskSave newtask;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Create new", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->m_currentSave->list.push_back(newtask);
                            }
                        }
                        if (column == 1)
                        {
                            ImGui::InputText("Task", newtask.task, 128);
                        }

                        if (column == 2)
                        {
                            ImGui::InputText("Component", newtask.component, 128);
                        }

                        if (column == 3)
                        {
                            ImGui::InputInt("Priority", &newtask.priority);
                        }
                    }
                }

            ImGui::EndTable();
        }

        if (ImGui::BeginTable("table", 4, flags))
        {
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Component", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

                for (int row = 0; row < this->m_currentSave->list.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                         std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);


                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->m_currentSave->list[row], this->m_currentSave->list.back());
                                this->m_currentSave->list.pop_back();
                            }
                        }
                        if (column == 1)
                        {
                             std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-task";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->list[row].task, 128);
                        }

                        if (column == 2)
                        {
                             std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-component";
                            label += std::to_string(row);
                            ImGui::InputText(label.c_str(), m_currentSave->list[row].component, 128);
                        }
                        if (column == 3)
                        {
                             std::string label = "###" + std::to_string(row) + "-" + std::to_string(column) + "-priority";
                            label += std::to_string(row);
                            ImGui::InputInt(label.c_str(), &m_currentSave->list[row].priority);
                        }
                    }
                }
            

            ImGui::EndTable();
        }

        ImGui::End();
    }
}
