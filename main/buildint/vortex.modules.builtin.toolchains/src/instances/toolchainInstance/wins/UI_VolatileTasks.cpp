#include "../ToolchainRenderInstance.h"

static std::vector<std::shared_ptr<Task>> tasklist;
static void refreshTaskList(const char *poolname)
{
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    args->add("pool_name", poolname);
    VortexMaker::CallModuleEvent(args, "GetTaskPool", "vortex.modules.builtin.tasks");
    std::shared_ptr<TaskPool> list = args->get<std::shared_ptr<TaskPool>>("taskpool", nullptr);

    if (list != nullptr)
    {
        tasklist = list->m_list;
    }
}

void ToolchainRenderInstance::UI_VolatileTasks()
{

    if (this->show_UI_VolatileTasks)
    {
        refreshTaskList(this->toolchain->pool_name);
        std::string label = this->name + " - Volatile Tasks ###" + this->name + "volatiletasks";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static std::shared_ptr<Task> selectedTask;
        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID trashIcon = this->m_TrashIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

        // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
        static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyMask_;

        const char *names[2] = {"Task selector", "Launcher"};
        static bool opened[2] = {true, true}; // Persistent user state

        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                if (opened[n] && ImGui::BeginTabItem(names[n], &opened[n], ImGuiTabItemFlags_None))
                {
                    if (opened[n] && n == 0)
                    {

                        // Left
                        static int selected = 0;
                        static std::array<char[128], 7> labels = {"Packages", "Scripts", "Patchs", "Tasklists", "Configurations", "Filesytem", "Permissions"};

                        {
                            ImGui::BeginChild("left pane", ImVec2(170, 0), true);
                            for (int i = 0; i < labels.size(); i++)
                            {
                                if (i == 0)
                                {
                                    ImGui::TextColored(ImVec4(0.4, 0.4, 0.4, 1), "On components");
                                }

                                if (i == 4)
                                { // 2 = 2/HERE/3
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

                            ImGui::Columns(3, NULL);

                            std::vector<const char *> items;
                            for (auto chaine : tasklist)
                            {
                                items.push_back(chaine->tasktype.c_str());
                            }
                            static int item_current = 0;
                            static int item_component_current = 0;

                            int i = 1;
                            for (int row = 0; row < this->toolchain->packages.size(); row++)
                            {
                                std::string label = "packhhageView###" + std::to_string(row) + this->toolchain->packages[row]->label + std::to_string(i);
                                ImGuiID id = ImGui::GetID(label.c_str());
                                ImGui::BeginChildFrame(id, ImVec2(0, 150), true);

                                // Affichage des éléments individuels
                                std::string ll = this->toolchain->packages[row]->label;
                                ImGui::Text(ll.c_str());

                                if (!items.empty())
                                {

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
                                }
                                else
                                {
                                    ImGui::Text("Empty");
                                }
                                if (ImGui::Button("Select"))
                                {

                                    std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                    props->add("toolchain", this->toolchain);

                                    for (auto task : tasklist)
                                    {
                                        if (task->tasktype == items[item_current])
                                        {
                                            std::shared_ptr<Task> _task = task->clone();
                                            //_task = task;
                                            std::shared_ptr<hArgs> props = std::make_shared<hArgs>();

                                            _task->init();
                                            props->add("toolchain", this->toolchain);
                                            props->add("package", this->toolchain->packages[row]);

                                            _task->id = task->tasktype + "-" + VortexMaker::gen_random(8);
                                            _task->tasktype = task->tasktype;
                                            _task->component = this->toolchain->packages[row]->label;
                                            _task->priority = task->priority;
                                            _task->props = props;
                                            _task->state = "not_started";

                                            selectedTask = _task;
                                        }
                                    }

                                    /////////////////////
                                }
                                /*
                                                if (ImGui::Button("Build"))
                                                {

                                                    std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                                    props->add("toolchain", this->toolchain);


                                                    for (auto task : tasklist)
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
                                */
                                ImGui::EndChildFrame();
                                ImGui::NextColumn();
                            }
                            ImGui::EndChild();
                        }

                        // Project Settings
                        if (selected == 3)
                        {
                            ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);
                            ImGui::Columns(4, NULL);

                            std::vector<const char *> items;
                            for (auto chaine : tasklist)
                            {
                                items.push_back(chaine->tasktype.c_str());
                            }
                            static int item_current = 0;
                            static int item_component_current = 0;

                            int i = 1;
                            /* for (int row = 0; row < this->toolchain->tasklists.size(); row++)
                             {
                                 std::string label = "packageVifghgfew###" + std::to_string(row) + this->toolchain->tasklists[row]->label + std::to_string(i);
                                 ImGuiID id = ImGui::GetID(label.c_str());
                                 ImGui::BeginChildFrame(id, ImVec2(0, 220), true);

                                 // Affichage des éléments individuels
                                 std::string ll = this->toolchain->tasklists[row]->label;
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

                                 /*

                                     TODO :
                                     - Selecteur de taches et builder de tache (possibilité d'attacher des props, des variables, etc...)


                                 *

                                 if (ImGui::Button("Select"))
                                 {
                                     for (auto task : tasklist)
                                     {
                                         if (task->tasktype == items[item_current])
                                         {
                                             std::shared_ptr<Task> _task = task->clone();
                                             //_task = task;

                                             _task->id = task->tasktype + "-" + VortexMaker::gen_random(8);
                                             _task->tasktype = task->tasktype;
                                             _task->component = this->toolchain->packages[row]->label;
                                             _task->priority = task->priority;
                                             _task->props->add("toolchain", this->toolchain);
                                             _task->props->add("packages", this->toolchain->packages[row]);
                                             _task->props->add("tasklist", this->toolchain->tasklists[row]);
                                             _task->state = "not_started";

                                             selectedTask = _task;
                                         }
                                     }
                                 }
                                 /*
                                                 if (ImGui::Button("Build"))
                                                 {

                                                     std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                                     props->add("toolchain", this->toolchain);


                                                     for (auto task : tasklist)
                                                     {
                                                         if (task->tasktype == items[item_current])
                                                         {
                                                             std::shared_ptr<Task> _task = task->clone();
                                                             //_task = task;
                                                             std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                                             props->add("toolchain", this->toolchain);
                                                             props->add("packages", this->toolchain->packages[row]);
                                                             props->add("tasklist", this->toolchain->tasklists[row]);

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
                                 *
                                 ImGui::EndChildFrame();
                                 ImGui::NextColumn();
                             }*/
                            ImGui::EndChild();
                        }

                        // Right
                        {
                        }
                    }

                    if (opened[n] && n == 1)
                    {

                        ImGui::BeginChild("Pans_VolatileTasks", ImVec2(0, 0), true);

                        if (ImGui::ImageButtonWithText(buildIcon, "Exec", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                        { // Ajout de la tâche aux listes appropriées
                          

                            {
                                std::shared_ptr<hArgs> arguments = std::make_shared<hArgs>();
                                arguments->add("pool_name", this->toolchain->pool_name);
                                arguments->add("processor_name", this->toolchain->pool_name);
                                arguments->add("task_name", selectedTask->tasktype.c_str());

                                std::shared_ptr<hArgs> props = std::make_shared<hArgs>();
                                props->add<std::shared_ptr<Toolchain>>("toolchain", this->toolchain);
                                arguments->add("arguments", props);

                                VortexMaker::CallModuleEvent(arguments, "StopProcessor", "vortex.modules.builtin.tasks");
                                VortexMaker::CallModuleEvent(arguments, "AddTaskToProcess", "vortex.modules.builtin.tasks");
                                VortexMaker::CallModuleEvent(arguments, "StartProcessor", "vortex.modules.builtin.tasks");

                                std::shared_ptr<Task> newtask = arguments->get<std::shared_ptr<Task>>("task", nullptr);
                                if (newtask != nullptr)
                                {
                                    this->toolchain->currentLoadedSystem.executedTasks.push_back(newtask);
                                    this->toolchain->currentLoadedSystem.parent = this->toolchain;
                                                            // this->toolchain->packages[row]->latestTask = _task;
                                    this->toolchain->currentLoadedSystem.Save(this->toolchain);
                                }
                                else{
                                    
                                }
                            }
                            /*
                                                            {
                                                                std::lock_guard<std::mutex> lock(this->toolchain->taskProcessor->mutex);
                                                                this->toolchain->taskProcessor->tasksToProcess.push_back(selectedTask);
                                                            }


                                                            this->toolchain->currentLoadedSystem.executedTasks.push_back(selectedTask);
                                                            // this->toolchain->packages[row]->latestTask = _task;
                                                            this->toolchain->currentLoadedSystem.Save(this->toolchain);*/
                        }

                        float oldsize = ImGui::GetFont()->Scale;
                        ImGui::GetFont()->Scale *= 1.3;
                        ImGui::PushFont(ImGui::GetFont());

                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Task :");

                        if (selectedTask != NULL)
                        {
                            ImGui::Text(selectedTask->tasktype.c_str());
                        }
                        else
                        {
                            ImGui::Text("No task selected.");
                        }

                        ImGui::GetFont()->Scale = oldsize;
                        ImGui::PopFont();

                        ImGuiID selectedId = ImGui::GetID("selectedView");
                        ImGui::BeginChildFrame(selectedId, ImVec2(0, 0), true);

                        if (selectedTask != NULL)
                        {

                            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                            ImGui::Text("Needed Variables : ");
                            if (ImGui::BeginTable("table1", 1, flags))
                            {
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableHeadersRow();

                                for (int row = 0; row < selectedTask->neededVariables.size(); row++)
                                {
                                    ImGui::TableNextRow();
                                    for (int column = 0; column < 1; column++)
                                    {
                                        ImGui::TableSetColumnIndex(column);
                                        if (column == 0)
                                        {

                                            ImGui::TextColored(ImVec4(0.2f, 0.2f, 1.0f, 1.0), selectedTask->neededVariables[row].c_str());
                                        }
                                    }
                                }
                                ImGui::EndTable();
                            }

                            ImGui::Text("Needed Props : ");
                            if (ImGui::BeginTable("table1", 1, flags))
                            {
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableHeadersRow();

                                for (int row = 0; row < selectedTask->neededProps.size(); row++)
                                {
                                    ImGui::TableNextRow();
                                    for (int column = 0; column < 1; column++)
                                    {
                                        ImGui::TableSetColumnIndex(column);
                                        if (column == 0)
                                        {

                                            ImGui::TextColored(ImVec4(0.2f, 0.2f, 1.0f, 1.0), selectedTask->neededProps[row].c_str());
                                        }
                                    }
                                }
                                ImGui::EndTable();
                            }

                            ImGui::Text("Loaded props : ");
                            if (ImGui::BeginTable("table3", 3, flags))
                            {
                                ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                ImGui::TableSetupColumn("Reference", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                char newPropName[128];
                                char newPropReference[128];

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

                                for (int row = 0; row < selectedTask->props->registered_arguments.size(); row++)
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

                                            ImGui::Text(selectedTask->props->registered_arguments[row].c_str());
                                        }

                                        if (column == 2)
                                        {
                                            if (selectedTask->props->registered_arguments[row] == "toolchain")
                                            {
                                                ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.7f, 1.0), selectedTask->props->get<std::shared_ptr<Toolchain>>("toolchain", nullptr)->name.c_str());
                                            }
                                            if (selectedTask->props->registered_arguments[row] == "package")
                                            {
                                                ImGui::TextColored(ImVec4(0.0f, 0.5f, 0.7f, 1.0), selectedTask->props->get<std::shared_ptr<Package>>("package", nullptr)->name.c_str());
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
