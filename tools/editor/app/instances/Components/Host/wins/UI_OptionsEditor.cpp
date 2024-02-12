#include "../HostInstance.h"

void HostInstance::UI_OptionsEditor()
{
    if (this->show_UI_OptionsEditor)
    {
        static std::string label = this->name + " - Options Window";
        ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);

        ImGui::Begin(label.c_str(), &this->opened, ImGuiWindowFlags_MenuBar);

            if (ImGui::BeginMenuBar())

            {const char* items[] = { "Configuration Options", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

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

            if (ImGui::BeginTable("table1", 3, flags))
            {
                ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("CCC", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int row = 0; row < 5; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text("%s %d,%d", (column == 2) ? "Stretch" : "Fixed", column, row);
                    }
                }
                ImGui::EndTable();
            }
        
        ImGui::End();
    }
}