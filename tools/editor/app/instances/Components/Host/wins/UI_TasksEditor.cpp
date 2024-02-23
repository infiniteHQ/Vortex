#include "../HostInstance.h"
#include <array>
#include <random>

void HostInstance::UI_TasksEditor()
{

    if (this->show_UI_TasksEditor)
    {

        static std::string label = this->name + " - Tasks Editor";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);        
        static ImTextureID eyeIcon = this->m_EyeIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
            ImGui::TableSetupColumn("Add a new argument", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

                for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Create new", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                            }
                        }
                        if (column == 1)
                        {
                        }

                        if (column == 2)
                        {
                        }

                        if (column == 3)
                        {
                        }
                    }
                }

            ImGui::EndTable();
        }

        if (ImGui::BeginTable("table_2", 4, flags))
        {
            ImGui::TableSetupColumn("Open", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Number of tasks", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

                for (int row = 0; row < this->host->tasklists.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        static std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);
                        static std::string openButtonID = "Open###" + std::to_string(row) + "-" + std::to_string(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, openButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
							    std::shared_ptr<TasklistInstance> instance = std::make_shared<TasklistInstance>(m_ctx, this->host->tasklists[row], this->host);
							    this->factory->SpawnInstance(instance);	
                            }
                        }
                        if (column == 1)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                            }
                        }

                        if (column == 2)
                        {
                            ImGui::Text(this->host->tasklists[row]->label.c_str());
                        }

                        if (column == 3)
                        {
                            ImGui::Text(std::to_string(this->host->tasklists[row]->list.size()).c_str());
                        }
                    }
                }
            

            ImGui::EndTable();
        }

        ImGui::End();
    }
}
