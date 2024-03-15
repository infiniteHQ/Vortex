#include "../ToolchainInstance.h"
#include "../../../Assets/Package/PackageInstance.h"


static bool compareLabels(const std::shared_ptr<VxPackage> &obj, const std::string &label)
{
    return obj->label == label;
}

void ToolchainInstance::UI_AssetsViewer()
{
    if (this->show_UI_AssetsViewer)
    {
        static std::string label = this->name + " - Assets Viewer###" +  this->name + "assetsviewer";
        ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID databaseIcon = this->m_DatabaseIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID packageIcon = this->m_PackageIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &databaseIcon, &this->opened, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
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

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table1", 4, flags))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Place", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            for (int row = 0; row < this->m_currentSave->registeredPackages.size(); row++)
            {
                         auto it = std::find_if(this->toolchain->packages.begin(), this->toolchain->packages.end(),
                                               [&](const std::shared_ptr<VxPackage> &obj)
                                               { return compareLabels(obj, this->m_currentSave->registeredPackages[row].first); });

                        std::shared_ptr<VxPackage> currentPackage = *it;

                ImGui::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    if (column == 0)
                    {
                        if (it != this->toolchain->packages.end())
                        {

                        ImGui::TableSetColumnIndex(column);   std::string openButtonID = "Open###" + std::to_string(row) + "-" + std::to_string(column);


                            if (ImGui::ImageButtonWithText(packageIcon, openButtonID.c_str(), ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
                            {
                                std::cout << "Opening " << currentPackage->name << std::endl;
							    std::shared_ptr<PackageInstance> instance = std::make_shared<PackageInstance>(m_ctx, currentPackage);
							    this->factory->SpawnInstance(instance);	
                            }
                            // Vous pouvez faire d'autres opérations avec l'élément trouvé ici
                        }
                        else
                        {

                            if (ImGui::Button("Not exist !"))
                            {
                                // Save behavior
                            }
                        }

                    }

                    if (column == 1)
                        ImGui::Text(this->m_currentSave->registeredPackages[row].second);

                    if (column == 2)
                        ImGui::Text(this->m_currentSave->registeredPackages[row].first);

                    if (column == 3)
                        if (it != this->toolchain->packages.end())
                        {
                        ImGui::Text(currentPackage->compilation.compilationCommand.c_str());
                        }
                }
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
}