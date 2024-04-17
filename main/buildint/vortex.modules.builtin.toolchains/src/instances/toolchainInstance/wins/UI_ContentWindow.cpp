#include "../ToolchainRenderInstance.h"

void ToolchainRenderInstance::UI_ContentWindow()
{
    if (this->show_UI_ContentWindow)
    {

         std::string label = this->name + " - Content Window";
        ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
        if (ImGui::Begin(label.c_str(), &this->show_UI_ContentWindow))
        {
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
        }
        ImGui::End();
    }
}