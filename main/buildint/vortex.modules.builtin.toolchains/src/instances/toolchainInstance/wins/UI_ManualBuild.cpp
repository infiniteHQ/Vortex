#include "../ToolchainRenderInstance.h"
#include <array>

void ToolchainRenderInstance::UI_ManualBuild()
{

    if (this->show_UI_ManualBuild)
    {

         std::string label = this->name + " - Manual Build###" + this->name + "manualbuild";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &toolIcon, &this->show_UI_ManualBuild, ImGuiWindowFlags_MenuBar);

            // Tab Bar
            const char* names[4] = { "Packages", "Users", "Skeleton", "Scripts" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                // Save behavior
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Pannels"))
            {
                if (ImGui::MenuItem("Packages", NULL, &opened[0]))
                {
                }
                if (ImGui::MenuItem("Users", NULL, &opened[1]))
                {
                }
                if (ImGui::MenuItem("Skeleton", NULL, &opened[2]))
                {
                }
                if (ImGui::MenuItem("Scripts", NULL, &opened[3]))
                {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

     // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton |ImGuiTabBarFlags_FittingPolicyMask_;

        
        

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin():
            // the underlying bool will be set to false when the tab is closed.
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && ImGui::BeginTabItem(names[n], &opened[n], ImGuiTabItemFlags_None))
                    {
                        if(opened[n] && n == 0){


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
                        if(column == 3){
                            float progress = 0.5f;
                            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
                            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
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
