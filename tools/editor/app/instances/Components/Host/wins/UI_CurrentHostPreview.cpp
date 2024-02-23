#include "../HostInstance.h"
#include <array>

void HostInstance::UI_CurrentHostPreview()
{

    if (this->show_UI_CurrentHostPreview)
    {

        static std::string label = this->name + " - Preview Current Host###" +  this->name + "previewcurrenthost";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static std::thread receiveThread;

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID eyeIcon = this->m_EyeIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &eyeIcon, &this->show_UI_FullBuild, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(buildIcon, "Chroot", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                // Save behavior
            }
            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                // Save behavior
            }

            const char *items[] = {"Filesystem", "Binaries", "Patchs", "Automations"};
            static int item_current = 0;
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

        // Left
        static int selected = 0;
        // For tasks, check from host tasks, and get all reports
        static std::array<char[128], 6> labels = {"General", "Tasks", "Package", "Filesystem", "Actions", "Scripts"};

        {
            ImGui::BeginChild("left pane", ImVec2(170, 0), true);
            for (int i = 0; i < labels.size(); i++)
            {
                if (ImGui::Selectable(labels[i], selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        ImGui::Separator();
        ImGui::SameLine();
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Project Settings

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (selected == 1)
            {
        if (ImGui::BeginTable("table_", 6, flags))
        {
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Component", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
                for (int row = 0; row < this->host->currentLoadedSystem.reports.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 6; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {

                            }
                        }
                        if (column == 1)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->parent.task.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->parent.uniqueID.c_str());
                        }
                        if (column == 3)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->parent.component.c_str());
                        }
                        if (column == 4)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->result.c_str());
                        }
                        if (column == 5)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->description.c_str());
                        }
                    }
                }

            ImGui::EndTable();
        }

            }

                      if (selected == 2)
            {
        if (ImGui::BeginTable("table_", 3, flags))
        {
            ImGui::TableSetupColumn("Package", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();


                for (int row = 0; row < this->host->currentLoadedSystem.reports.size(); row++)
                {
                    if(this->host->currentLoadedSystem.reports[row]->parent.task == "testpackage"){
                        
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->parent.component.c_str());
                        }
                        if (column == 1)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->parent.uniqueID.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.reports[row]->result.c_str());
                        }
                    }
                    }
                }

            ImGui::EndTable();
        }

            }
    
        // Right
        {
        }

        ImGui::End();

    }
}
