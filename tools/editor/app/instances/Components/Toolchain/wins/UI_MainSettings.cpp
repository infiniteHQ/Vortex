#include "../ToolchainInstance.h"
#include <array>

void ToolchainInstance::UI_MainSettings()
{

    if (this->show_UI_MainSettings)
    {

        std::string label = this->name + " - Toolchain Settings###" + this->name + "mainsettings";
        ImGui::SetNextWindowDockID(this->dockspaceID, ImGuiCond_FirstUseEver);

        static ImTextureID editIcon = this->m_EditIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID saveIcon = this->m_SaveIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static ImTextureID settingsIcon = this->m_SettingsIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Begin(label.c_str(), &editIcon, &this->show_UI_MainSettings, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(saveIcon, "Save", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                this->Save();
            }
            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_SaveIcon->GetWidth(), this->m_SaveIcon->GetHeight())))
            {
                this->Refresh();
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

        // Left
        static int selected = 0;
        static std::array<char[128], 5> labels = {"Project Settings", "Configuration", "Production", "Maintenance", "Danger zone"};

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
        if (selected == 0)
        {

            ImGui::BeginGroup();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::Text("Informations");
            ImGui::Separator();

            ImGui::InputText("Toolchain Name", this->m_currentSave->name, 128);
            ImGui::InputText("Toolchain Author", this->m_currentSave->author, 128);
            ImGui::InputText("Toolchain Description", this->m_currentSave->description, 128);
            ImGui::InputText("Toolchain Version", this->m_currentSave->version, 128);

            ImGui::PopStyleVar(2);
            if (ImGui::Button("Revert"))
            {
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
            }
            ImGui::EndGroup();
        }

        if (selected == 1)
        {

            ImGui::BeginGroup();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::Text("General Informations");
            ImGui::Separator();

            ImGui::Text("Toolchain Builder system configuration");

            std::vector<const char *> archs_items = {
                "unspecified",
                "x86_64",
                "none"
                };

            int build_archs_items_current = 0;

            if (ImGui::BeginCombo("Builder architecture (this)", archs_items[build_archs_items_current]))
            {
                for (int i = 0; i < archs_items.size(); ++i)
                {
                    bool is_selected = (build_archs_items_current == i);
                    if (ImGui::Selectable(archs_items[i], is_selected))
                    {
                        build_archs_items_current = i;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                        // Set the save build arch
                    }
                }
                ImGui::EndCombo();
            }

            static char buf1[128] = "";
            ImGui::InputText("Deuxiemme", buf1, 128);
            static char buf2[128] = "";
            ImGui::InputText("Host Author", buf2, 128);
            static char buf3[128] = "";
            ImGui::InputText("Host Version", buf3, 128);

            ImGui::PopStyleVar(2);
            if (ImGui::Button("Revert"))
            {
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
            }
            ImGui::EndGroup();
        }

        if (selected == 4)
        {

            ImGui::BeginGroup();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::Text("Delete Toolchain");
            ImGui::Separator();

            ImGui::PopStyleVar(2);
            if (ImGui::ColorButton("Delete", ImVec4(1.0f, 0.2f, 0.2f, 1.0f)))
            {
                VortexMaker::DeleteToolchain(this->toolchain);
                this->opened = false;
            }
            ImGui::SameLine();
            ImGui::EndGroup();
        }

        // Right
        {
        }

        ImGui::End();
    }
}
