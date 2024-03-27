#include "../ToolchainInstance.h"
#include <array>

/*

TODO: Main progressbar for all tasks & manipulations
TODO: Add a overview of all tasks states (errors, warns, fatal, etc..)
TODO: Testing after.

*/

void ToolchainInstance::UI_FullBuild()
{

    if (this->show_UI_FullBuild)
    {

        std::string label = this->name + " - Manual Build###" + this->name + "manualbuild";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

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
                for (auto tasklists : this->toolchain->packages)
                {
                    if (tasklists->name == "TL_Main")
                    {
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
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
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


                                        if (ImGui::ImageButtonWithText(buildIcon, "Error", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            std::cout << "EE" << std::endl;
                                            ImGui::InsertNotification({ImGuiToastType::Error, 3000, "That is a error! %s", "(Format here)"});
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }
                                        if (ImGui::ImageButtonWithText(buildIcon, "Info", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            std::cout << "EE" << std::endl;
                                            ImGui::InsertNotification({ImGuiToastType::Info, 3000, "That is a info! %s", "(Format here)"});
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }
                                        if (ImGui::ImageButtonWithText(buildIcon, "none", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            std::cout << "EE" << std::endl;
                                            ImGui::InsertNotification({ImGuiToastType::None, 3000, "That is a none! %s", "(Format here)"});
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }
                                        if (ImGui::ImageButtonWithText(buildIcon, "Success", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            std::cout << "EE" << std::endl;
                                            ImGui::InsertNotification({ImGuiToastType::Success, 3000, "That is a success! %s", "(Format here)"});
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }
                                        if (ImGui::ImageButtonWithText(buildIcon, "Warn", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            std::cout << "EE" << std::endl;
                                            ImGui::InsertNotification({ImGuiToastType::Warning, 3000, "That is a success! %s", "(Format here)"});
                                            // Config Task
                                            // Compile Task
                                            // Install Task
                                        }
                    }
                    if (opened[n] && n == 1)
                    {

                        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                        if (ImGui::BeginTable("table_manual_build", 4, flags))
                        {
                            ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableSetupColumn("Progress", ImGuiTableColumnFlags_WidthFixed);
                            ImGui::TableHeadersRow();
                            for (int row = 0; row < this->toolchain->packages.size(); row++)
                            {
                                static std::pair<char[128], char[128]> newItem;
                                static char label[128];

                                ImGui::TableNextRow();
                                for (int column = 0; column < 2; column++)
                                {
                                    ImGui::TableSetColumnIndex(column);

                                    if (column == 0)
                                    {
                                        if (ImGui::ImageButtonWithText(buildIcon, "Build", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                                        {
                                            std::cout << "EE" << std::endl;
                                            ImGui::InsertNotification({ImGuiToastType::Success, 3000, "That is a success! %s", "(Format here)"});
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
                                    {
                                        ImGui::Text(this->toolchain->packages[row]->latestTask->state.c_str());
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
