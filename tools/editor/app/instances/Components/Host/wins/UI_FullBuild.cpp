#include "../HostInstance.h"
#include <array> 

void HostInstance::UI_FullBuild()
{

    if (this->show_UI_FullBuild)
    {

         std::string label = this->name + " - Full Build###" +  this->name +"fullbuild";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID buildIcon = this->m_BuildIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
static ImTextureID toolIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

          ImGui::Begin(label.c_str(), &toolIcon, &this->show_UI_FullBuild, ImGuiWindowFlags_MenuBar);

            if (ImGui::BeginMenuBar())
            {

                if(ImGui::ImageButtonWithText(buildIcon, "Build", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
                }
                if(ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight()))){
                    //Save behavior
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
        static std::array<char[128], 4> labels = {"Build", "General View", "Logs", "Errors"};


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

        }

        if(selected == 1){

        }

        // Right
        {
        }
        
        ImGui::End();
        }

    
}
