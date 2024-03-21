#include "../ToolchainInstance.h"
#include <array>
#include <random>
#include <mutex>

/*
         IDEM QUE volatile tasks

         - Selectionner une tasklist
         - Possibilité de prévisualiser toutes les taches
         - Récupérer l'ensemble des props et des variables requises
         - Ajouter/Retirer les props chargés par défauts


         // L'initialisation des props de taches se fait au dernier moment


         // Tasklists -> EXEC -> Tasklist


        // Plus tard : Taskgraph // Tasklist avec une prog nodable (conditions, fonctions customs, etc...)


*/

void ToolchainInstance::UI_TasksEditor()
{

    if (this->show_UI_TasksEditor)
    {
        static std::shared_ptr<TaskList> selectedTasklist;

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

        // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
        static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyMask_;

        const char *names[2] = {"Tasklist selector", "Launcher"};
        static bool opened[2] = {true, true}; // Persistent user state

        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                if (opened[n] && ImGui::BeginTabItem(names[n], &opened[n], ImGuiTabItemFlags_None))
                {
                    if (opened[n] && n == 0)
                    {

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
                                    std::string execButtonID = "Select###" + std::to_string(row) + "-" + std::to_string(column);
                                    std::string deleteButtonID = "Delete###" + std::to_string(row) + "-" + std::to_string(column);
                                    std::string openButtonID = "Open###" + std::to_string(row) + "-" + std::to_string(column);

                                    if (column == 0)
                                    {
                                        if (ImGui::ImageButtonWithText(buildIcon, execButtonID.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            this->toolchain->tasklists[row]->Refresh();

                                            for (auto task : this->toolchain->tasklists[row]->list)
                                            {
                                                // Récup tout les props 

                                                std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                                props->add("toolchain", this->toolchain);
                                                selectedTasklist = this->toolchain->tasklists[row];
                                                selectedTasklist->props = props;
                                                /*
                                                for (auto runtime_tasks : this->toolchain->tasks)
                                                {
                                                    if (runtime_tasks->tasktype == task->tasktype)
                                                    {
                                                        std::shared_ptr<Task> _task = runtime_tasks->clone();

                                                        _task->props = props; // Props de la tasklist

                                                        std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                                        props->add("toolchain", this->toolchain);

                                                        for (auto component : this->toolchain->packages)
                                                        {
                                                            if (component->name == task->component)
                                                            {
                                                                props->add("package", component);
                                                            }
                                                        }

                                                        _task->id = runtime_tasks->tasktype + "-" + VortexMaker::gen_random(8);
                                                        _task->tasktype = runtime_tasks->tasktype;
                                                        _task->component = task->component;
                                                        _task->priority = task->priority;
                                                        _task->state = "not_started";

                                                        // Ajout de la tâche aux listes appropriées
                                                        if (this->toolchain->taskProcessor)
                                                        {
                                                            // this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                                                            // this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                                                            this->toolchain->currentLoadedSystem.executedTasks.push_back(_task);
                                                            // this->toolchain->packages[row]->latestTask = _task;
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


                                                */
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
                    }

                    if (opened[n] && n == 1)
                    {

                        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

                        if (ImGui::ImageButtonWithText(buildIcon, "Exec", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                        { // Ajout de la tâche aux listes appropriées
                            if (this->toolchain->taskProcessor)
                            {

                                for (auto task : selectedTasklist->list)
                                {
                                    
                                        std::shared_ptr<Task> _task = task->clone();

                                        selectedTasklist->props->add("toolchain", this->toolchain);

                                        _task->init();

                                        _task->id = task->tasktype + "-" + VortexMaker::gen_random(8);
                                        _task->tasktype = task->tasktype;
                                        _task->priority = task->priority;
                                        _task->props = selectedTasklist->props;
                                        _task->state = "not_started";
                                        

                                        if (this->toolchain->taskProcessor)
                                        {

                                            
                                                std::lock_guard<std::mutex> lock(this->toolchain->taskProcessor->mutex);
                                                this->toolchain->taskProcessor->tasksToProcess.push_back(_task);
                                            

                                            this->toolchain->currentLoadedSystem.executedTasks.push_back(_task);
                                            // this->toolchain->packages[row]->latestTask = _task;
                                            this->toolchain->currentLoadedSystem.Save(this->toolchain);
                                        }
                                        else
                                        {
                                            std::cout << "Failed while accessing taskToProcess" << std::endl;
                                        }
                                    
                                }
                            }
                            else
                            {
                                std::cout << "Failed while accessing taskToProcess" << std::endl;
                            }
                        }

                        float oldsize = ImGui::GetFont()->Scale;
                        ImGui::GetFont()->Scale *= 1.3;
                        ImGui::PushFont(ImGui::GetFont());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Task :");

                        if (selectedTasklist != NULL)
                        {
                            ImGui::Text(selectedTasklist->label.c_str());
                        }
                        else
                        {
                            ImGui::Text("No task selected.");
                        }

                        ImGui::GetFont()->Scale = oldsize;
                        ImGui::PopFont();

                        ImGuiID selectedId = ImGui::GetID("selectedView");
                        ImGui::BeginChildFrame(selectedId, ImVec2(0, 0), true);

                        if (selectedTasklist != NULL)
                        {

                            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                            ImGui::Text("Loaded props : ");
                            if (ImGui::BeginTable("table3", 3, flags))
                            {
                                ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                static char newPropName[128];
                                static char newPropReference[128];

                                for (int row = 0; row < 1; row++)
                                {
                                    ImGui::TableNextRow();
                                    for (int column = 0; column < 3; column++)
                                    {

                                        ImGui::TableSetColumnIndex(column);

                                        if (column == 0)
                                        {

                                            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                            {
                                                // Config Task
                                                // Compile Task
                                                // Install Task


                                                if(newPropName == "package"){

                                                for(auto package : this->toolchain->packages){
                                                    std::cout << package->name.c_str() << "w/"<<  newPropReference <<  std::endl;
                                                    if(package->name == newPropReference){
                                                        selectedTasklist->props->add(newPropName, package);
                                                    }
                                                }
                                                }

                                            }
                                        }

                                        if (column == 1)
                                        {
                                            ImGui::InputText("Name", newPropName, 128);
                                        }

                                        if (column == 2)
                                        {
                                            ImGui::InputText("Reference", newPropReference, 128);
                                        }
                                    }
                                }
                                ImGui::EndTable();
                            }
                            if (ImGui::BeginTable("table3", 3, flags))
                            {
                                ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                for (int row = 0; row < selectedTasklist->props->registered_arguments.size(); row++)
                                {
                                    ImGui::TableNextRow();
                                    for (int column = 0; column < 3; column++)
                                    {

                                        ImGui::TableSetColumnIndex(column);

                                        if (column == 0)
                                        {

                                            if (ImGui::ImageButtonWithText(trashIcon, "Delete", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                            {
                                                // Config Task
                                                // Compile Task
                                                // Install Task
                                            }
                                        }

                                        if (column == 1)
                                        {

                                            ImGui::Text(selectedTasklist->props->registered_arguments[row].c_str());
                                        }

                                        if (column == 2)
                                        {
                                            if (selectedTasklist->props->registered_arguments[row] == "toolchain")
                                            {
                                                ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.7f, 1.0), selectedTasklist->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr)->name.c_str());
                                            }
                                            if (selectedTasklist->props->registered_arguments[row] == "package")
                                            {
                                                ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.7f, 1.0), selectedTasklist->props->get<std::shared_ptr<VxPackage>>("package", nullptr)->name.c_str());
                                            }
                                        }
                                    }
                                }
                                ImGui::EndTable();
                            }
                        }
                        else
                        {
                            ImGui::Text("No selected task yet.");
                            ImGui::Text("Please selecte a task, associated with a component (a package, or other asset...).");
                        }
                        ImGui::EndChildFrame();
                        ImGui::EndChild();
                    }

                    ImGui::EndTabItem();
                }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
