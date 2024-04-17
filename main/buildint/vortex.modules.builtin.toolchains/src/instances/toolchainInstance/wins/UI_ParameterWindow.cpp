#include "../ToolchainRenderInstance.h"

void ToolchainRenderInstance::UI_ParametersWindow()
{
    if (this->show_UI_ParametersWindow)
    {

         std::string label = this->name + " - Parameters ###" + this->name + "Parameters";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);
        if (ImGui::Begin(label.c_str(), &this->show_UI_ParametersWindow))
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (ImGui::BeginTable("taskEditorTable", 3, flags))
            {
                ImGui::TableSetupColumn("taskname", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("CCC", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                for (int row = 0; row < this->toolchain->tasks.size(); row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text((char*)this->toolchain->tasks[row]->tasktype.c_str());
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
