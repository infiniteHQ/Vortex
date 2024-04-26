#include "../ToolchainRenderInstance.h"
#include <array> 

void ToolchainRenderInstance::UI_SnapshotUtility()
{

    if (this->show_UI_SnapshotUtility)
    {

         std::string label = this->name + " - Snapshot Utility###" +  this->name + "snapshotutility";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

          ImGui::Begin(label.c_str(), &toolIcon, &this->show_UI_SnapshotUtility, ImGuiWindowFlags_MenuBar);

            if (ImGui::BeginMenuBar())
            {

                if(ImGui::ImageButtonWithText(buildIcon, "Build", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
                }
                if(ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
                    this->toolchain->RefreshSnapshots();
                }
                
                const char* items[] = { "Packages", "Scripts", "Patchs", "Automations" };
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
        static std::array<char[128], 4> labels = {"Create", "Revert", "Plan Snapshots", "Errors"};


        {
            ImGui::BeginChild("left pane", ImVec2(170, 0), true);
            for (int i = 0; i < labels.size(); i++)
            {
                if (ImGui::Selectable(labels[i], selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

            ImGui::Separator();
        ImGui::SameLine();
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Project Settings

        if(selected == 0){
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table_snap", 2, flags))
        {
            ImGui::TableSetupColumn("Create new Snapshot", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
             for (int row = 0; row < 1; row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            if (ImGui::ImageButtonWithText(addIcon, "Create", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->toolchain->MakeSnapshot(label);
                                this->Refresh();
                            }
                        }
                        if (column == 1)
                        {
                            ImGui::InputText("Name", label, 128);
                        }
                    }
                }

                ImGui::EndTable();
        }
        }

        if(selected == 1){

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table_snap_2", 4, flags))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Include Data ?", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
             for (int row = 0; row < this->toolchain->snapshots.size(); row++)
                {
                    static std::pair<char[128], char[128]> newItem;
                    static char label[128];

                    ImGui::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            std::string label = "Revert###" + this->toolchain->snapshots[row].name;
                            if (ImGui::ImageButtonWithText(addIcon, label.c_str(), ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
                            {
                                this->toolchain->RetakeSnapshot(this->toolchain->snapshots[row].name);
                                this->Refresh();
                            }
                        }
                        if (column == 1)
                        {
                            ImGui::Text(this->toolchain->snapshots[row].name.c_str());
                        }
                        if (column == 2)
                        {
                            ImGui::Text(this->toolchain->snapshots[row].snapshotSystem.size.c_str());
                        }
                        if (column == 3)
                        {
                            static bool test;
                            ImGui::Checkbox("Include Data", &test);
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
