#include "../ToolchainRenderInstance.h"
#include <array>

//#include "../../backend/Platform/GUI/editor/UI/Spinner.h"
/*

TODO: Main progressbar for all tasks & manipulations
TODO: Add a overview of all tasks states (errors, warns, fatal, etc..)
TODO: Testing after.

*/

static void coloredTag(std::string name, ImVec4 color)
{
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::Button(name.c_str());
    ImGui::PopStyleColor(3);
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





void ToolchainRenderInstance::UI_FullBuild()
{

    if (this->show_UI_FullBuild)
    {

        std::string label = this->name + " - Full Build###" + this->name + "fullbuild";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);
        static std::shared_ptr<TaskList> selectedTasklist;
        static bool packagePropAdded = false;
        static bool toolchainPropAdded = false;
        static bool tasklistPropAdded = false;
        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        float progress = 0.5f; // Set to zero, and calculate the progress of all tasks

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &toolIcon, &this->show_UI_FullBuild, ImGuiWindowFlags_MenuBar);

        // Tab Bar
        const char *names[4] = {"Overview", "Packages", "Dist", "Scripts"};
        static bool opened[4] = {true, true, true, true}; // Persistent user state
        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(buildIcon, "Build", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                for (auto tasklists : this->toolchain->tasklists)
                {
                    // Barre de pourcentage générale sur le nombre de taches dans la TLMAIN+ TLTEST
                    if (tasklists->label == "TL_Main")
                    {
                         if (this->toolchain->taskProcessor)
                            {
                                toolchainPropAdded = false;
                                            packagePropAdded = false;

                                           tasklists->Refresh();

                                            selectedTasklist = std::make_shared<TaskList>();
                                            selectedTasklist->list.clear();

                                            selectedTasklist->label =tasklists->label;

                                            for (auto task :tasklists->list)
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
                                                            if (env_prop.first == "package")
                                                            {
                                                                for (auto package : this->toolchain->packages)
                                                                {
                                                                    if (package->name == env_prop.second)
                                                                    {
                                                                        _props->add("package", package);
                                                                        packagePropAdded = true;
                                                                    }
                                                                }
                                                            }
                                                            else if (env_prop.first == "toolchain")
                                                            {
                                                                for (auto toolchain : CToolchainModule->m_toolchains)
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

                                                                std::shared_ptr<Package> _package = std::make_shared<Package>();
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
                                                    }
                                                }
                                            }


                                    if (this->toolchain->taskProcessor)
                                    {
                                        this->toolchain->taskProcessor->stop = true;
                                    }

                                //tasklists->Refresh();
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
                                        this->toolchain->currentLoadedSystem.parent = this->toolchain;
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
                            
                            

                                    if (this->toolchain->taskProcessor)
                                    {
                                        this->toolchain->taskProcessor->stop = false;
                                    }
                            
                            
                            }
                            else
                            {
                                std::cout << "Failed while accessing taskToProcess" << std::endl;
                            }
                        // Si working host détécté, proposer de le suppr avant de continuer.
                        // Ensuite, lancer la l'execution de toutes les taches de la tasklist.
                    }
                    else
                    {
                        // Display overlay error : To main tasklist (TL_Main) is missing
                    }
                }
                // Save behavior
            }
            ImGui::Separator();
            ImGui::EndMenuBar();
        }

        // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
        static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyMask_;

        // Passing a bool* to BeginTabItem() is similar to passing one to Begin():
        // the underlying bool will be set to false when the tab is closed.
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                if (opened[n] && ImGui::BeginTabItem(names[n], &opened[n], ImGuiTabItemFlags_None))
                {
                    if (opened[n] && n == 0)
                    {

                        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
                        ImGui::Text("Ready for the build ?");

                                        /*
                                        if (ImGui::ImageButtonWithText(buildIcon, "Error", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            ImGui::InsertNotification({ImGuiToastType::Error, 3000, "That is a error! %s", "(Format here)"});
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }*/
                    }
                    if (opened[n] && n == 1)
                    {

                        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                        if (ImGui::BeginTable("table_manual_build", 4, flags))
                        {
                            ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Latest", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Progress", ImGuiTableColumnFlags_WidthFixed);
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
                                        if (ImGui::ImageButtonWithText(buildIcon, "Build", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }
                                    }
                                    if (column == 1)
                                    {
                                        ImGui::Text(this->toolchain->packages[row]->label.c_str());
                                    }
                                    if (column == 2)
                                    { if (this->toolchain->packages[row]->latestTask->state== "finished")
                            {
                                    coloredText("Finished", ImVec4(0.874f, 0.635f, 0.015f, 0.7f));

                            }
                            if (this->toolchain->packages[row]->latestTask->state == "success")
                            {
                                    coloredText("Success", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));

                            }
                            if (this->toolchain->packages[row]->latestTask->state== "failed")
                            {
                                    coloredText("Failed", ImVec4(1.0f, 0.2f, 0.2f, 0.8f));

                            }
                            if (this->toolchain->packages[row]->latestTask->state== "paused")
                            {
                                    coloredText("Paused", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));

                                
                            }
                            if (this->toolchain->packages[row]->latestTask->state== "waiting")
                            {
                                    coloredText("Waiting", ImVec4(0.5f, 0.5f, 0.2f, 1.0f));

                            }
                            if (this->toolchain->packages[row]->latestTask->state== "retry")
                            {
                                    coloredText("Retry", ImVec4(0.874f, 0.635f, 0.015f, 1.0f));

                            }
                            if (this->toolchain->packages[row]->latestTask->state== "process")
                            {
                                //coloredTag("Processing...", ImVec4(0.8f, 0.5f, 0.5f, 1.0f));
                                const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
                                const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
                                //ImGui::Spinner("##spinner", 6, 3, col);
                            }
                            if (this->toolchain->packages[row]->latestTask->state== "not_started")
                            {
                                coloredTag("Not Started", ImVec4(0.5f, 0.5f, 0.5f, 0.1f));
                            }
                                    }
                                }
                            }

                            ImGui::EndTable();
                        }
                    }

                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
