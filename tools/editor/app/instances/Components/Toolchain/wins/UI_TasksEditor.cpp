#include "../ToolchainInstance.h"
#include <array>
#include <random>
#include <mutex>


/*

TODO: Improuve the composant/props system for more flexibility and modularity.


*/

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
        static bool packagePropAdded = false;
        static bool toolchainPropAdded = false;
        static bool tasklistPropAdded = false;

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
                                            toolchainPropAdded = false;
                                            packagePropAdded = false;

                                            this->toolchain->tasklists[row]->Refresh();

                                            selectedTasklist = std::make_shared<TaskList>();
                                            selectedTasklist->list.clear();

                                            selectedTasklist->label = this->toolchain->tasklists[row]->label;

                                            for (auto task : this->toolchain->tasklists[row]->list)
                                            {
                                                // Récup tout les props
                                                packagePropAdded = false;
                                                toolchainPropAdded = false;

                                                for (auto runtime_tasks : this->toolchain->tasks)
                                                {
                                                    if (runtime_tasks->tasktype == task->tasktype)
                                                    {
                                                        std::shared_ptr<Task> _task = runtime_tasks->clone();
                                                        std::shared_ptr<hArgs> _props = std::make_shared<hArgs>();
                                                        _task->init();
                                                        _task->priority = task->priority;
                                                        _task->tasktype = task->tasktype;
                                                        _task->component = task->component;
                                                        // task = runtime_tasks->clone();
                                                        // task->init();
                                                        // task->props = std::make_shared<hArgs>();



                                                                for(auto tasklist : this->toolchain->tasklists){
                                                                    if(task->component == tasklist->label){                                            
                                                                        _props->add("tasklist", tasklist); // Or, add the default element of the tasklist
                                                                    }
                                                                }

                                                                for(auto package : this->toolchain->packages){
                                                                    if(task->component == package->label){                                           
                                                                        _props->add("package", package); // Or, add the default element of the tasklist
                                                                    }
                                                                }
                                                        for (auto env_prop : task->env_props)
                                                        {
                                                            std::cout << "Processing " << env_prop.first << " " << env_prop.second << std::endl;
                                                            if (env_prop.first == "package")
                                                            {
                                                                for (auto package : this->toolchain->packages)
                                                                {
                                                                    std::cout << "Package name " << package->name << " " << env_prop.second << std::endl;
                                                                    if (package->name == env_prop.second)
                                                                    {
                                                                        _props->add("package", package);
                                                                        packagePropAdded = true;
                                                                    }
                                                                }
                                                            }
                                                            else if (env_prop.first == "toolchain")
                                                            {
                                                                for (auto toolchain : this->m_ctx->IO.toolchains)
                                                                {
                                                                    if (toolchain->name == env_prop.second)
                                                                    {
                                                                        _props->add("toolchain", toolchain);
                                                                        toolchainPropAdded = true;
                                                                    }
                                                                }
                                                            }
                                                        }

                                                        for (auto prop : _task->neededProps)
                                                        {
                                                            if (prop == "toolchain" && !toolchainPropAdded)
                                                            {
                                                                _props->add("toolchain", this->toolchain);
                                                                toolchainPropAdded = true;
                                                            }
                                                            else if (prop == "package" && !packagePropAdded)
                                                            {

                                                                std::shared_ptr<VxPackage> _package = std::make_shared<VxPackage>();
                                                                _package->name = "none";
                                                                _props->add("package", _package); // Or, add the default element of the tasklist
                                                                packagePropAdded = true;
                                                            }
                                                            else if (prop == "tasklist" && !tasklistPropAdded)
                                                            {

                                                                std::shared_ptr<TaskList> _tasklist = std::make_shared<TaskList>();
                                                                _tasklist->label = "none";


                                                                tasklistPropAdded = true;
                                                            }
                                                        }

                                                        _props->add("env_props", task->env_props);
                                                        _task->props = _props;
                                                        selectedTasklist->list.push_back(_task);
                                                        std::cout << _task->tasktype << _task->props << std::endl;
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
                    }

                    if (opened[n] && n == 1)
                    {

                        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

                        if (ImGui::ImageButtonWithText(buildIcon, "Exec", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                        { // Ajout de la tâche aux listes appropriées
                            if (this->toolchain->taskProcessor)
                            {

                                // this->toolchain->tasklists[row]->Refresh();
                                for (auto task : selectedTasklist->list)
                                {

                                    // std::shared_ptr<Task> _task = runtime_tasks->clone();
                                    task->id = task->tasktype + "-" + VortexMaker::gen_random(8);
                                    //_task->tasktype = runtime_tasks->tasktype;
                                    //_task->component = task->component;
                                    //_task->priority = task->priority;
                                    //_task->props = task->props; // Props are from the tasklist conf
                                    task->state = "not_started";

                                    /*
                                            Initialiser les props par la conf globale (de la tasklist)
                                            Essayer d'ecraser par une conf plus proche (de la task).




                                    */

                                    if (this->toolchain->taskProcessor)
                                    {
                                        this->toolchain->currentLoadedSystem.executedTasks.push_back(task);
                                        {
                                            std::lock_guard<std::mutex> lock(this->toolchain->taskProcessor->mutex);
                                            this->toolchain->taskProcessor->tasksToProcess.push_back(task);
                                        }

                                        this->toolchain->currentLoadedSystem.Save(this->toolchain);
                                    }
                                    else
                                    {
                                        std::cout << "Failed while accessing taskToProcess" << std::endl;
                                    }

                                    /*
                                    for (auto runtime_tasks : this->toolchain->tasks)
                                    {
                                        if (runtime_tasks->tasktype == task->tasktype)
                                        {

                                        }
                                    }*/
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

                        if (selectedTasklist != NULL)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Task :");
                            ImGui::SameLine();
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

                            for (int task = 0; task < selectedTasklist->list.size(); task++)
                            {
                                std::string taskLabel = "Loaded props of task " + selectedTasklist->list[task]->tasktype;
                                ImGui::Text(taskLabel.c_str());
                                if (ImGui::BeginTable("table3", 3, flags))
                                {
                                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                    ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthStretch);
                                    ImGui::TableHeadersRow();

                                    for (int row = 0; row < selectedTasklist->list[task]->props->registered_arguments.size(); row++)
                                    {

                                        ImGui::TableNextRow();
                                        for (int column = 0; column < 3; column++)
                                        {

                                            ImGui::TableSetColumnIndex(column);

                                            if (column == 0)
                                            {
                                            }

                                            if (column == 1)
                                            {
                                                ImGui::Text(selectedTasklist->list[task]->props->registered_arguments[row].c_str());
                                            }
                                            if (column == 2)
                                            {

                                                if (selectedTasklist->list[task]->props->registered_arguments[row] == "package")
                                                {
                                                    std::vector<const char *> packages_items = {"none"};


                                                    int packages_items_current = 0;

                                                    for (int i = 0; i < this->toolchain->packages.size(); i++)
                                                    {
                                                        if (this->toolchain->packages[i]->name == selectedTasklist->list[task]->props->get<std::shared_ptr<VxPackage>>("package", nullptr)->name)
                                                        {
                                                            packages_items_current = i + 1;
                                                        }
                                                    }

                                                    for (auto &chaine : this->toolchain->packages)
                                                    {
                                                        packages_items.push_back(chaine->name.c_str());
                                                    }

                                                    std::shared_ptr<VxPackage> package = selectedTasklist->list[task]->props->get<std::shared_ptr<VxPackage>>("package", nullptr);
                                                    if (package != nullptr)
                                                    {
                                                        //ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0), package->name.c_str());
                                                    }
                                                    else
                                                    {
                                                        ImGui::Text("none");
                                                    }

                                                        std::string packages = "Packages###" + std::to_string(task) + "-" + std::to_string(row);

                                                    if (ImGui::BeginCombo(packages.c_str(), packages_items[packages_items_current]))
                                                    {
                                                        for (int i = 0; i < packages_items.size(); ++i)
                                                        {
                                                            bool is_selected = (packages_items_current == i);
                                                            if (ImGui::Selectable(packages_items[i], is_selected))
                                                            {
                                                                packages_items_current = i;

                                                                for (auto package : this->toolchain->packages)
                                                                {
                                                                    std::cout << package->name.c_str() << " </" << packages_items[packages_items_current] << std::endl;
                                                                    if (package->name == packages_items[packages_items_current])
                                                                    {
                                                                        bool alreadyRegistered = false;
                                                                        for (auto alreadyRegisteredProps : selectedTasklist->list[task]->props->registered_arguments)
                                                                        {
                                                                            if (alreadyRegisteredProps == packages_items[packages_items_current])
                                                                            {
                                                                                alreadyRegistered = true;
                                                                            }
                                                                        }

                                                                        if (!alreadyRegistered)
                                                                        {

                                                                            std::cout << "Add " << package << std::endl;
                                                                            selectedTasklist->list[task]->props->remove("package");
                                                                            selectedTasklist->list[task]->props->add("package", package);

                                                                            std::cout << "Task package name is " << selectedTasklist->list[task]->props->get<std::shared_ptr<VxPackage>>("package", nullptr)->name.c_str() << std::endl;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            if (is_selected)
                                                            {
                                                                ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                                                            }
                                                        }
                                                        ImGui::EndCombo();
                                                    }
                                                }


                                                    if (selectedTasklist->list[task]->props->registered_arguments[row] == "toolchain")
                                                    {
                                                        if (selectedTasklist->list[task]->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr)->name.c_str() == this->toolchain->name.c_str())
                                                        {
                                                            ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.7f, 1.0), "this");
                                                        }
                                                        else
                                                        {
                                                            ImGui::Text(selectedTasklist->list[task]->props->get<std::shared_ptr<VxToolchain>>("toolchain", nullptr)->name.c_str());
                                                        }
                                                    }


                                                    if (selectedTasklist->list[task]->props->registered_arguments[row] == "env_props")
                                                    {
                                                            ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.7f, 1.0), "current env props");
                                                    
                                                    }
                                               
                                               

                                                if (selectedTasklist->list[task]->props->registered_arguments[row] == "tasklist")
                                                {
                                                    std::vector<const char *> tasklists_items = {"none"};

                                                    int tasklist_items_current = 0;

                                                    for (auto &chaine : this->toolchain->tasklists)
                                                    {
                                                        tasklists_items.push_back(chaine->label.c_str());
                                                    }

                                                    for (int i = 0; i < this->toolchain->tasklists.size(); i++)
                                                    {
                                                        if (this->toolchain->tasklists[i]->label == selectedTasklist->list[task]->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr)->label)
                                                        {
                                                            tasklist_items_current = i + 1;
                                                        }
                                                    }

                                                    if (selectedTasklist->list[task]->props->registered_arguments[row] == "tasklist")
                                                    {
                                                        std::shared_ptr<TaskList> tasklist = selectedTasklist->list[task]->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr);
                                                        if (tasklist != nullptr)
                                                        {
                                                            //ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0), tasklist->label.c_str());
                                                        }
                                                        else
                                                        {
                                                            ImGui::Text("none");
                                                        }
                                                        std::string label = "Tasklist###" + std::to_string(task) + "-" + std::to_string(row);

                                                        if (ImGui::BeginCombo(label.c_str(), tasklists_items[tasklist_items_current]))
                                                        {
                                                            for (int i = 0; i < tasklists_items.size(); ++i)
                                                            {
                                                                bool is_selected = (tasklist_items_current == i);
                                                                if (ImGui::Selectable(tasklists_items[i], is_selected))
                                                                {
                                                                    tasklist_items_current = i;

                                                                    for (auto tasklist : this->toolchain->tasklists)
                                                                    {
                                                                        std::cout << tasklist->label.c_str() << " </" << tasklists_items[tasklist_items_current] << std::endl;
                                                                        if (tasklist->label == tasklists_items[tasklist_items_current])
                                                                        {
                                                                            bool alreadyRegistered = false;
                                                                            for (auto alreadyRegisteredProps : selectedTasklist->list[task]->props->registered_arguments)
                                                                            {
                                                                                if (alreadyRegisteredProps == tasklists_items[tasklist_items_current])
                                                                                {
                                                                                    alreadyRegistered = true;
                                                                                }
                                                                            }

                                                                            if (!alreadyRegistered)
                                                                            {

                                                                                std::cout << "Add " << tasklist << std::endl;
                                                                                selectedTasklist->list[task]->props->remove("tasklist");
                                                                                selectedTasklist->list[task]->props->add("tasklist", tasklist);

                                                                                std::cout << "Task tasklist name is " << selectedTasklist->list[task]->props->get<std::shared_ptr<TaskList>>("tasklist", nullptr)->label.c_str() << std::endl;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                                if (is_selected)
                                                                {
                                                                    ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                                                                }
                                                            }
                                                            ImGui::EndCombo();
                                                        }
                                                    }
                                                     }
                                            }
                                        }
                                    }
                                    ImGui::EndTable();
                                }
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
