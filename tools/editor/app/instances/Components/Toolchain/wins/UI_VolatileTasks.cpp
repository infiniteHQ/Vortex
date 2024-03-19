#include "../ToolchainInstance.h"
#include <array>


void ToolchainInstance::UI_VolatileTasks()
{

    if (this->show_UI_VolatileTasks)
    {

         std::string label = this->name + " - Volatile Tasks ###" + this->name + "volatiletasks";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &toolIcon, &this->show_UI_VolatileTasks, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(buildIcon, "Build", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                // Save behavior
            }
            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                // Save behavior
            }

            const char *items[] = {"Packages", "Scripts", "Patchs", "Automations"};
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
        static std::array<char[128], 7> labels = {"Packages", "Scripts", "Patchs", "Tasklists", "Configurations", "Filesytem", "Permissions"};

        {
            ImGui::BeginChild("left pane", ImVec2(170, 0), true);
            for (int i = 0; i < labels.size(); i++)
            {
                if(i == 0){
                    ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), "On components");
                }

                if(i == 4){ // 2 = 2/HERE/3
                    ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), "On toolchain");
                }
                if (ImGui::Selectable(labels[i], selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        ImGui::Separator();
        ImGui::SameLine();

        // Project Settings
        if (selected == 0)
        {
            ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);
            ImGui::Columns(4, NULL);

            std::vector<const char *> items;
            for (auto chaine : this->toolchain->tasks)
            {
                items.push_back(chaine->tasktype.c_str());
            }
            static int item_current = 0;
            static int item_component_current = 0;

            int i = 1;
            for (int row = 0; row < this->toolchain->packages.size(); row++)
            {
                std::string label = "packageView###" + std::to_string(row) + this->toolchain->packages[row]->label + std::to_string(i);
                ImGuiID id = ImGui::GetID(label.c_str());
                ImGui::BeginChildFrame(id, ImVec2(0, 220), true);

                // Affichage des éléments individuels
                std::string ll = this->toolchain->packages[row]->label;
                ImGui::Text(ll.c_str());

                if (ImGui::BeginCombo("Task", items[item_current]))
                {
                    for (int i = 0; i < items.size(); ++i)
                    {
                        bool is_selected = (item_current == i);
                        if (ImGui::Selectable(items[i], is_selected))
                        {
                            item_current = i; // Met à jour l'ID de l'élément sélectionné
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Build"))
                {

                    std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                    props->add("toolchain", this->toolchain);


                    for (auto task : this->toolchain->tasks)
                    {
                        if (task->tasktype == items[item_current])
                        {
                            std::shared_ptr<Task> _task = task->clone();
                            //_task = task;
                            std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                            props->add("toolchain", this->toolchain);
                            props->add("package", this->toolchain->packages[row]);

                            _task->id = task->tasktype + "-" + VortexMaker::gen_random(8);
                            _task->tasktype = task->tasktype;
                            _task->component = this->toolchain->packages[row]->label;
                            _task->priority = task->priority;
                            _task->props = props;
                            _task->state = "not_started";

                            // Ajout de la tâche aux listes appropriées
                            if(this->toolchain->taskProcessor){

                {
                    std::lock_guard<std::mutex> lock(this->toolchain->taskProcessor->mutex);
                    this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                }

                                this->toolchain->currentLoadedSystem.executedTasks.push_back(_task);
                                this->toolchain->packages[row]->latestTask = _task;
                                this->toolchain->currentLoadedSystem.Save(this->toolchain);
                            }
                            else{
                                std::cout << "Failed while accessing taskToProcess" << std::endl;
                            }
                        }
                    }

                    /////////////////////
                }

                ImGui::EndChildFrame();
                ImGui::NextColumn();
            }
            ImGui::EndChild();
        }

        if (selected == 1)
        {
        }

        // Right
        {
        }

        ImGui::End();
    }
}
