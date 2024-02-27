#include "../HostInstance.h"
#include <array>

static void coloredTag(std::string name, ImVec4 color)
{
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::Button(name.c_str());
    ImGui::PopStyleColor(3);
}

static void idTag(std::string name)
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
    
    ImVec4 color(0.0f,0.0f,0.0f,0.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);

    ImGui::Button(name.c_str());

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
}

void HostInstance::UI_CurrentHostPreview()
{

    if (this->show_UI_CurrentHostPreview)
    {

        static std::string label = this->name + " - Preview Current Host###" + this->name + "previewcurrenthost";
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

        std::string patchlabel = "Executed tasks (" + std::to_string(this->host->currentLoadedSystem.executedTasks.size()) + ")";
        std::array<std::string, 6> labels = {"General", patchlabel, "Package", "Filesystem", "Actions", "Scripts"};

        {
            ImGui::BeginChild("left pane", ImVec2(170, 0), true);
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
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Project Settings

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (selected == 1)
        {
            if (ImGui::BeginTable("table_", 7, flags))
            {
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Retry", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Started", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                for (int row = 0; row < this->host->currentLoadedSystem.executedTasks.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 7; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->host->currentLoadedSystem.executedTasks[row], this->host->currentLoadedSystem.executedTasks.back());
                                this->host->currentLoadedSystem.executedTasks.pop_back();
                                this->host->currentLoadedSystem.Save(this->host);
                            }
                        }
                        if (column == 1)
                        {

                            std::string retryButtonID = "Retry###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(refreshIcon, retryButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                props->add("host", this->host);

                                std::shared_ptr<Task> task = VortexMaker::CreateTask(this->host->currentLoadedSystem.executedTasks[row]->tasktype, "SecondTestHostTask-123-retry", 123, props);

                                task->state = "retry";
                                props->add("self", task);

                                this->host->currentLoadedSystem.executedTasks.push_back(task);

                                this->host->currentLoadedSystem.Save(this->host);
                            }
                        }
                        if (column == 2)
                        {
                            idTag(this->host->currentLoadedSystem.executedTasks[row]->id);
                        }
                        if (column == 3)
                        {
                            ImGui::Text(this->host->currentLoadedSystem.executedTasks[row]->tasktype.c_str());
                        }
                        if (column == 4)
                        {
                            if (this->host->currentLoadedSystem.executedTasks[row]->state == "finished")
                            {
                                coloredTag("Finished", ImVec4(0.0f, 1.0f, 0.2f, 0.4f));
                            }
                            if (this->host->currentLoadedSystem.executedTasks[row]->state == "success")
                            {
                                coloredTag("Success", ImVec4(0.0f, 1.0f, 0.2f, 0.4f));
                            }
                            if (this->host->currentLoadedSystem.executedTasks[row]->state == "failed")
                            {
                                coloredTag("Fail", ImVec4(1.0f, 0.2f, 0.2f, 0.4f));
                            }
                            if (this->host->currentLoadedSystem.executedTasks[row]->state == "paused")
                            {
                                coloredTag("Fail", ImVec4(0.5f, 0.5f, 0.2f, 0.4f));
                            }
                            if (this->host->currentLoadedSystem.executedTasks[row]->state == "retry")
                            {
                                coloredTag("Retry", ImVec4(0.5f, 0.5f, 0.2f, 0.4f));
                            }
                            if (this->host->currentLoadedSystem.executedTasks[row]->state == "not_started")
                            {
                                coloredTag("Not Started", ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
                            }
                        }
                        if (column == 5)
                        {
                            ImGui::Text(std::to_string(this->host->currentLoadedSystem.executedTasks[row]->total_time).c_str());
                        }
                        if (column == 6)
                        {
                            ImGui::Text(std::to_string(this->host->currentLoadedSystem.executedTasks[row]->start_time).c_str());
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

                /* for (int row = 0; row < this->host->currentLoadedSystem.reports.size(); row++)
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
                 }*/

                ImGui::EndTable();
            }
        }

        // Right
        {
        }

        ImGui::End();
    }
}
