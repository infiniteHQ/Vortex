#include "../ToolchainRenderInstance.h"

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

float cookProgress(std::shared_ptr<Task> task){
    int completedCount = task->successCounter + task->failCounter + task->warningCounter;
    int totalCount = completedCount + task->unknowCounter;

    if (totalCount == 0) {
        return 0.0f; 
    } else {
        return static_cast<float>(completedCount) / totalCount;
    }
}


static void coloredText(std::string name, ImVec4 color)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::Button(name.c_str());
    ImGui::PopStyleColor(4);
}

static void labelTag(std::string name)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Button(name.c_str());
    ImGui::PopStyleColor(4);
}

static void coloredTag(std::string name, ImVec4 color)
{
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::Button(name.c_str());
    ImGui::PopStyleColor(3);
}

static void progressBar(float progress){

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.324f, 0.511f, 0.929f, 1.0f));
    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
    ImGui::PopStyleColor(1);
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

void ToolchainRenderInstance::UI_CurrentToolchainPreview()
{

    if (this->show_UI_CurrentToolchainPreview)
    {

         std::string label = this->toolchain->name + " - Preview Current Host###" + this->toolchain->name + "previewcurrenttoolchain";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static std::thread receiveThread;

        static bool advancedView;
        static bool lowColorView = true;

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
                    if (ImGui::ImageButtonWithText(addIcon, "Import", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                    {
                        // TODO :Ask the root of third party toolchain, scan it to find target options & params and init it. (Copy FS, populate working_host...)
                        //this->toolchain->CreateCurrentToolchainSystem();
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
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Advanced view", NULL, &advancedView))
                {
                }
                if (ImGui::MenuItem("Low Color view", NULL, &lowColorView))
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

        std::string variableslabel = "Variables (" + std::to_string(this->toolchain->currentLoadedSystem.variables.size()) + ")";
        std::array<std::string, 6> labels = {"General", patchlabel, packagelabel, variableslabel, "Actions", "Scripts"};

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
            // Advanced mode : (Make a boolean with a simple mod (only, name, state & progress))

            if(advancedView){
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

							    //std::shared_ptr<ReportInstance> instance = std::make_shared<ReportInstance>(m_ctx, this->toolchain->currentLoadedSystem.executedTasks[row]);
							    //this->factory->SpawnInstance(instance);	
                                std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
                                args->add<std::shared_ptr<Task>>("task", this->toolchain->currentLoadedSystem.executedTasks[row]);
                                VortexMaker::CallModuleEvent(args, "OpenTaskReportInstance", "vortex.modules.builtin.tasks");
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
                                coloredTag("Success", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "failed")
                            {
                                coloredTag("Failed", ImVec4(1.0f, 0.2f, 0.2f, 0.8f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "paused")
                            {
                                coloredTag("Paused", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "waiting")
                            {
                                coloredTag("Waiting", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));
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
            else{ // Simple mode


                            if (ImGui::BeginTable("table_", 4, flags))
            {
                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Progress", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                for (int row = 0; row < this->toolchain->currentLoadedSystem.executedTasks.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
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
                            labelTag(this->toolchain->currentLoadedSystem.executedTasks[row]->tasktype.c_str());
                            //ImGui::Text(this->toolchain->currentLoadedSystem.executedTasks[row]->tasktype.c_str());
                        }
                        if (column == 2)
                        {
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "finished")
                            {
                                if(lowColorView){
                                    coloredText("Finished", ImVec4(0.874f, 0.635f, 0.015f, 0.7f));

                                }
                                else{
                                    coloredTag("Finished", ImVec4(0.874f, 0.635f, 0.015f, 0.7f));
                                }
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "success")
                            {
                                if(lowColorView){
                                    coloredText("Success", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));

                                }
                                else{
                                coloredTag("Success", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
                                }
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "failed")
                            {
                                if(lowColorView){
                                    coloredText("Failed", ImVec4(1.0f, 0.2f, 0.2f, 0.8f));

                                }
                                else{
                                coloredTag("Failed", ImVec4(1.0f, 0.2f, 0.2f, 0.8f));
                                }
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "paused")
                            {
                                if(lowColorView){
                                    coloredText("Paused", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));

                                }
                                else{
                                coloredTag("Paused", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));
                                }
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "waiting")
                            {
                                if(lowColorView){
                                    coloredText("Waiting", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));

                                }
                                else{
                                coloredTag("Waiting", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));
                                }
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "retry")
                            {
                                if(lowColorView){
                                    coloredText("Retry", ImVec4(0.874f, 0.635f, 0.015f, 1.0f));

                                }
                                else{
                                coloredTag("Retry", ImVec4(0.874f, 0.635f, 0.015f, 1.0f));
                                }
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "process")
                            {
                                //coloredTag("Processing...", ImVec4(0.8f, 0.5f, 0.5f, 1.0f));
                                const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                                const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
                                ImGui::Spinner("##spinner", 6, 3, col);
                            }
                            if (this->toolchain->currentLoadedSystem.executedTasks[row]->state == "not_started")
                            {
                                coloredTag("Not Started", ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
                            }
                        }
                        if (column == 3)
                        {
                            float progress = cookProgress(this->toolchain->currentLoadedSystem.executedTasks[row]);

progressBar(progress);

                        }
                    }
                }

                ImGui::EndTable();
            }
        

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

        if (selected == 3)
        {
            
            if (ImGui::BeginTable("table_package", 3, flags))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Task Owner", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Acutal value", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();

                for (int row = 0; row < this->toolchain->currentLoadedSystem.variables.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                         if (column == 0)
                                {
                                    ImGui::Text(std::get<0>(this->toolchain->currentLoadedSystem.variables[row]).c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(0.2f, 0.2f , 1.0f, 1.0f),std::get<1>(this->toolchain->currentLoadedSystem.variables[row]).c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::Text(std::get<2>(this->toolchain->currentLoadedSystem.variables[row]).c_str());
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
