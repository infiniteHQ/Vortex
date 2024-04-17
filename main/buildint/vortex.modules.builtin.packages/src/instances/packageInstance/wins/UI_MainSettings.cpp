#include "../PackageRenderInstance.h"
#include <array> 

void PackageRenderInstance::UI_MainSettings()
{

    if (this->show_UI_MainSettings)
    {

     std::string label = this->name + " - Package Settings###" +  this->name + " - Compilations Arguments";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

          ImGui::Begin(label.c_str(), &editIcon, &this->show_UI_MainSettings, ImGuiWindowFlags_MenuBar);

            if (ImGui::BeginMenuBar())
            {

                if(ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    this->Save();
                }
                if(ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    this->Refresh();
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
        static std::array<char[128], 4> labels = {"Project Settings", "Configuration", "Production", "Maintenance"};


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

        // Project Settings
        if(selected == 0){

            ImGui::BeginGroup();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::Text("Informations");
            ImGui::Separator();

            ImGui::InputText("Package Name",     this->m_currentSave->name, 128);
            ImGui::InputText("Package Author",     this->m_currentSave->author, 128);
            ImGui::InputText("Package Description",     this->m_currentSave->description, 128);
            ImGui::InputInt("Package Priority",     &this->m_currentSave->priority);
            ImGui::InputText("Package Compressed",     this->m_currentSave->compressed, 128);
            ImGui::InputText("Package Filename",     this->m_currentSave->filename, 128);
            ImGui::InputText("Package Label",     this->m_currentSave->label, 128);


            ImGui::PopStyleVar(2);
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
            ImGui::EndGroup();

        }

        if(selected == 1){

            ImGui::BeginGroup();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::Text("General Informations");
            ImGui::Separator();

            ImGui::InputText("Custom compilation command",     this->m_currentSave->compilationExclusiveCommand, 128);
            ImGui::InputText("Custom configuration command",     this->m_currentSave->configurationExclusiveCommand, 128);
            ImGui::InputText("Custom installation command",     this->m_currentSave->installationExclusiveCommand, 128);

            ImGui::PopStyleVar(2);
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
            ImGui::EndGroup();

        }

        // Right
        {
        }
        
        ImGui::End();
        }

    
}
