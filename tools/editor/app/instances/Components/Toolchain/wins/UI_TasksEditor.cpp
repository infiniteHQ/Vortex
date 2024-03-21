#include "../ToolchainInstance.h"
#include <array>
#include <random>
#include <mutex>



void ToolchainInstance::UI_TasksEditor()
{

    if (this->show_UI_TasksEditor)
    {

         std::string label = this->name + " - Tasks Editor###" + this->name + "taskseditor";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

        if (ImGui::BeginTable("table_2", 5, flags))
        {
            ImGui::TableSetupColumn("Execute", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Open", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Number of tasks", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            for (int row = 0; row < this->toolchain->tasklists.size(); row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 5; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    std::string execButtonID = "Execute###" + std::to_string(row) + "-" + std::to_string(column);
                    std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);
                    std::string openButtonID = "Open###" + std::to_string(row) + "-" + std::to_string(column);

                    if (column == 0)
                    {
                        if (ImGui::ImageButtonWithText(buildIcon, execButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                        {
                            this->toolchain->tasklists[row]->Refresh();
for (auto task : this->toolchain->tasklists[row]->list)
{
    for(auto runtime_tasks : this->toolchain->tasks)
    {
        if(runtime_tasks->tasktype == task->tasktype)
        {
            std::shared_ptr<Task> _task = runtime_tasks->clone();
            //_task = task;
            std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
            props->add("toolchain", this->toolchain);
           
                            // Resolve and add components
            props->add("package", this->toolchain->packages[row]);
            for(auto component : this->toolchain->packages)
            {
                if(component->name == task->component){
                    props->add("package", component);
                }
            }

            _task->id = runtime_tasks->tasktype + "-" + VortexMaker::gen_random(8);
            _task->tasktype = runtime_tasks->tasktype;
            _task->component = task->component;
            _task->priority = task->priority;
            _task->props = props;
            _task->state = "not_started";

            // Ajout de la tâche aux listes appropriées
            if (this->toolchain->taskProcessor)
            {
                //this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                //this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                this->toolchain->currentLoadedSystem.executedTasks.push_back(_task);
                //this->toolchain->packages[row]->latestTask = _task;
                this->toolchain->currentLoadedSystem.Save(this->toolchain);

                {
                    std::lock_guard<std::mutex> lock(this->toolchain->taskProcessor->mutex);
                    this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                }
            }
            else
            {
                std::cout << "Failed while accessing taskToProcess" << std::endl;
            }
        }
    }
}

                        }
                    }
                    if (column == 1)
                    {
                        if (ImGui::ImageButtonWithText(eyeIcon, openButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                        {
                            std::shared_ptr<TasklistInstance> instance = std::make_shared<TasklistInstance>(m_ctx, this->toolchain->tasklists[row], this->toolchain);
                            this->factory->SpawnInstance(instance);
                        }
                    }
                    if (column == 2)
                    {
                        if (ImGui::ImageButtonWithText(trashIcon, deleteButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                        {
                        }
                    }

                    if (column == 3)
                    {
                        ImGui::Text(this->toolchain->tasklists[row]->label.c_str());
                    }

                    if (column == 4)
                    {
                        ImGui::Text(std::to_string(this->toolchain->tasklists[row]->list.size()).c_str());
                    }
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}
