#include "../ToolchainInstance.h"
#include <array>

static std::string formatElapsedTime(double elapsedSeconds)
{
    int hours = static_cast<int>(elapsedSeconds / 3600);
    int minutes = static_cast<int>((elapsedSeconds - hours * 3600) / 60);
    int seconds = static_cast<int>(elapsedSeconds - hours * 3600 - minutes * 60);

    std::string formattedTime;
    if (hours > 0)
    {
        formattedTime += std::to_string(hours) + "h ";
    }
    if (minutes > 0 || hours > 0)
    {
        formattedTime += std::to_string(minutes) + "m ";
    }
    formattedTime += std::to_string(seconds) + "s";

    return formattedTime;
}

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

    ImVec4 color(0.0f, 0.0f, 0.0f, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);

    ImGui::Button(name.c_str());

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
}

void ToolchainInstance::UI_CurrentToolchainPreview()
{

    if (this->show_UI_CurrentToolchainPreview)
    {

        static std::string label = this->name + " - Preview Current Host###" + this->name + "previewcurrenttoolchain";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static std::thread receiveThread;

        static ImTextureID errorIcon = this->m_ErrorIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID warnIcon = this->m_WarningIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID successIcon = this->m_SuccessIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID unknowIcon = this->m_UnknowIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID flipbookIcon = this->m_FlipBookIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID eyeIcon = this->m_EyeIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static bool destroyModalOpened = false;

        {

            ImGui::Begin(label.c_str(), &eyeIcon, &this->show_UI_CurrentToolchainPreview, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar())
            {

                if (this->toolchain->haveCurrentSys == false)
                {
                    ImGui::Text("No working system detected.");

                    if (ImGui::ImageButtonWithText(addIcon, "Create", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                    {
                        this->toolchain->CreateCurrentToolchainSystem();
                    }
                }
                else
                {
                    if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                    {
                        destroyModalOpened = true;
                    }
                if (ImGui::ImageButtonWithText(buildIcon, "Chroot", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                {
                    // Save behavior
                }
                if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                {
                    // Save behavior
                }
                }
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


                    if (destroyModalOpened)
                        ImGui::OpenPopup("DestroyCurrentSys");


if (ImGui::BeginPopupModal("DestroyCurrentSys"))
            {

                // inputs widget
                ImGui::Text("Warning ! You will destroy the current working system, all your dist data will be erase but you will keep basic data");
                ImGui::Text("Note: You can create a snapshot to save it before destroy it.");
                if (ImGui::Button("Destroy", ImVec2(120, 0)))
                {
                    this->toolchain->DeleteCurrentToolchainSystem();
                    this->toolchain->haveCurrentSys = false;
                        destroyModalOpened = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Snapshot & Destroy", ImVec2(120, 0)))
                {
                    this->toolchain->MakeSnapshot("before-destroy");
                    this->toolchain->DeleteCurrentToolchainSystem();
                    this->toolchain->haveCurrentSys = false;
                        destroyModalOpened = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                        destroyModalOpened = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        // Left
        static int selected = 0;

        std::string patchlabel = "Executed tasks (" + std::to_string(this->toolchain->currentLoadedSystem.executedTasks.size()) + ")";
        std::string packagelabel = "Packages (" + std::to_string(this->toolchain->packages.size()) + ")";
        std::array<std::string, 6> labels = {"General", patchlabel, packagelabel, "Filesystem", "Actions", "Scripts"};

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

        // Project Settings

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (selected == 1)
        {
            if (ImGui::BeginTable("table_", 9, flags))
            {
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Retry", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Started", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                for (int row = 0; row < this->toolchain->currentLoadedSystem.executedTasks.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 9; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::swap(this->toolchain->currentLoadedSystem.executedTasks[row], this->toolchain->currentLoadedSystem.executedTasks.back());
                                this->toolchain->currentLoadedSystem.executedTasks.pop_back();
                                this->toolchain->currentLoadedSystem.Save(this->toolchain);
                            }
                        }
                        if (column == 1)
                        {

                            // TODO : task for toolchains, test reports and latest tasks by elements
{

                            std::string buttonid = "Report###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(flipbookIcon, buttonid.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
							    std::shared_ptr<ReportInstance> instance = std::make_shared<ReportInstance>(m_ctx, this->toolchain->currentLoadedSystem.executedTasks[row]);
							    this->factory->SpawnInstance(instance);	
                            }
}
{
/*
                            std::string buttonid = "Retry###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(refreshIcon, buttonid.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                props->add("toolchain", this->toolchain);

                                std::shared_ptr<Task> task = VortexMaker::CreateTask(this->toolchain->currentLoadedSystem.executedTasks[row]->tasktype, this->toolchain->currentLoadedSystem.executedTasks[row]->component, "SecondTestHostTask-123-retry", 123, props);

                                task->state = "retry";
                                props->add("self", task);

                                this->toolchain->currentLoadedSystem.executedTasks.push_back(task);

                                this->toolchain->currentLoadedSystem.Save(this->toolchain);
                            }*/
}
                        }
                        if (column == 2)
                        {
                            idTag(this->toolchain->currentLoadedSystem.executedTasks[row]->id);
                        }
                        if (column == 3)
                        {
                            ImGui::Text(this->toolchain->currentLoadedSystem.executedTasks[row]->tasktype.c_str());
                        }
                        if (column == 4)
                        {
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "finished")
                            {
                                coloredTag("Finished", ImVec4(0.874f, 0.635f, 0.015f, 0.7f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "success")
                            {
                                coloredTag("Success", ImVec4(0.0f, 1.0f, 0.2f, 1.0f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "failed")
                            {
                                coloredTag("Failed", ImVec4(1.0f, 0.2f, 0.2f, 0.8f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "paused")
                            {
                                coloredTag("Paused", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "retry")
                            {
                                coloredTag("Retry", ImVec4(0.874f, 0.635f, 0.015f, 1.0f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "process")
                            {
                                coloredTag("Processing...", ImVec4(0.8f, 0.5f, 0.5f, 1.0f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "not_started")
                            {
                                coloredTag("Not Started", ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
                            }
                        }
                        if (column == 5)
                        {
                            std::string errorButtonID = std::to_string(this->toolchain->currentLoadedSystem.executedTasks[row]->failCounter) + "###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(errorIcon, errorButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }
                            ImGui::SameLine();

                            std::string warnButtonID = std::to_string(this->toolchain->currentLoadedSystem.executedTasks[row]->warningCounter) + "###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(warnIcon, warnButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }
                            ImGui::SameLine();

                            std::string successButtonID = std::to_string(this->toolchain->currentLoadedSystem.executedTasks[row]->successCounter) + "###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(successIcon, successButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }

                            ImGui::SameLine();

                            std::string unknowButtonID = std::to_string(this->toolchain->currentLoadedSystem.executedTasks[row]->unknowCounter) + "###" + std::to_string(row) + "-" + std::to_string(column);
                            if (ImGui::ImageButtonWithText(unknowIcon, unknowButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }
                        }
                        if (column == 6)
                        {
                            ImGui::Text(std::to_string(this->toolchain->currentLoadedSystem.executedTasks[row]->priority).c_str());
                        }
                        if (column == 7)
                        {
                            double elapsed = this->toolchain->currentLoadedSystem.executedTasks[row]->elapsedSeconds();
                            std::string formattedElapsedTime = formatElapsedTime(elapsed);
                            ImGui::Text(formattedElapsedTime.c_str());
                        }
                        if (column == 8)
                        {
                            ImGui::Text(this->toolchain->currentLoadedSystem.executedTasks[row]->startTime().c_str());
                        }
                    }
                }

                ImGui::EndTable();
            }
        }

        if (selected == 2)
        {
            
            if (ImGui::BeginTable("table_package", 4, flags))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Latest action", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Latest ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Latest result", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                for (int row = 0; row < this->toolchain->packages.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            ImGui::Text(this->toolchain->packages[row]->label.c_str());
                        }
                        if (column == 1)
                        {
                            if(this->toolchain->packages[row]->latestTask->tasktype.c_str() == "unknow"){
                                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 0.4f), "No action yet");

                            }
                        }
                        if (column == 2)
                        {
                            idTag(this->toolchain->packages[row]->latestTask->id);
                        }
                        if (column == 3)
                        {
                            if (this->toolchain->packages[row]->latestTask->state == "finished")
                            {
                                coloredTag("Finished", ImVec4(0.0f, 1.0f, 0.2f, 0.4f));
                            }
                            if (this->toolchain->packages[row]->latestTask->state == "success")
                            {
                                coloredTag("Success", ImVec4(0.0f, 1.0f, 0.2f, 0.4f));
                            }
                            if (this->toolchain->packages[row]->latestTask->state == "failed")
                            {
                                coloredTag("Failed", ImVec4(1.0f, 0.2f, 0.2f, 0.4f));
                            }
                            if (this->toolchain->packages[row]->latestTask->state == "paused")
                            {
                                coloredTag("Paused", ImVec4(0.5f, 0.5f, 0.2f, 0.4f));
                            }
                            if (this->toolchain->packages[row]->latestTask->state == "retry")
                            {
                                coloredTag("Retry", ImVec4(0.5f, 0.5f, 0.2f, 0.4f));
                            }
                            if (this->toolchain->packages[row]->latestTask->state == "process")
                            {
                                coloredTag("Processing...", ImVec4(0.8f, 0.5f, 0.5f, 0.4f));
                            }
                            if (this->toolchain->packages[row]->latestTask->state == "not_started")
                            {
                                coloredTag("Not Started", ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
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
