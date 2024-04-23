#include "../ToolchainRenderInstance.h"
#include <array>

void ToolchainRenderInstance::UI_MainSettings()
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
        static std::array<char[128], 5> labels = {"Project Settings", "Configuration", "Dist Configs", "Maintenance", "Danger zone"};

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

            // TODO : Fix gcc and all toolchain components, add scripts assets and specifiy package (P_) as a make package
            // All dist for toolchain and add this logic for host & gpos.

            std::vector<const char *> archs_items = {
                "unspecified",
                "aarch64",
                "aarch64be",
                "arcle-750d",
                "arcle-hs38",
                "armebv7-eabihf",
                "armv5-eabi",
                "armv6-eabihf",
                "armv7-eabihf",
                "armv7m",
                "bfin",
                "m68k-68xxx",
                "m68k-coldfire",
                "microblazebe",
                "microblazeel",
                "mips32",
                "mips32el",
                "mips32r5el",
                "mips32r6el",
                "mips64-n32",
                "mips64el-n32",
                "mips64r6el-n32",
                "nios2",
                "i686",
                "openrisc",
                "powerpc-440fp",
                "powerpc-e300c3",
                "powerpc-e500mc",
                "powerpc64-e5500",
                "powerpc64-e6500",
                "powerpc64-power8",
                "powerpc64le-power8",
                "riscv32-ilp32d",
                "riscv64-lp64d",
                "s390x-z13",
                "sh-sh4",
                "sh-sh4aeb",
                "sparc64",
                "sparcv8",
                "x86_64",
                "x86-64-core-i7",
                "x86-64-v2",
                "x86-64-v3",
                "x86-64-v4",
                "x86-core2",
                "x86-i686",
                "xtensa-lx60"};

            std::vector<const char *> types_items = {
                "native",
                "cross",
                "cross_native",
                "canadian",
                "custom"};

            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::Text("Toolchain main type");

            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            int current_toolchain_type = 0;

            for (size_t i = 0; i < types_items.size(); ++i)
            {
                if (std::strcmp(this->m_currentSave->toolchain_type, types_items[i]) == 0)
                {
                    current_toolchain_type = i;
                    break;
                }
            }

            if (ImGui::BeginCombo("Type of toolchain", types_items[current_toolchain_type]))
            {
                for (int i = 0; i < types_items.size(); ++i)
                {
                    bool is_selected = (current_toolchain_type == i);
                    if (ImGui::Selectable(types_items[i], is_selected))
                    {
                        std::strcpy(this->m_currentSave->toolchain_type, types_items[i]);

                        current_toolchain_type = i;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                        // Set the save build arch
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            {

                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::Text("Toolchain Builder system configuration");

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();
            }

            int build_archs_items_current = 0;

            for (size_t i = 0; i < archs_items.size(); ++i)
            {
                if (std::strcmp(this->m_currentSave->builder_arch, archs_items[i]) == 0)
                {
                    build_archs_items_current = i;
                    break;
                }
            }

            ImGui::Text("The builder triplet will be : ");
            ImGui::SameLine();
            ImGui::Text(this->toolchain->GetTriplet("builder").c_str());

            if (ImGui::BeginCombo("Builder architecture (tools/toolchain builder)", archs_items[build_archs_items_current]))
            {
                for (int i = 0; i < archs_items.size(); ++i)
                {
                    bool is_selected = (build_archs_items_current == i);
                    if (ImGui::Selectable(archs_items[i], is_selected))
                    {
                        std::strcpy(this->m_currentSave->builder_arch, archs_items[i]);

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

            ImGui::InputText("Builder vendor", this->m_currentSave->builder_vendor, 128);
            ImGui::InputText("Builder platform", this->m_currentSave->builder_platform, 128);

            ImGui::Separator();
            {

                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::Text("Toolchain Host system configuration");

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();
            }

            ImGui::Text("The host triplet will be : ");
            ImGui::SameLine();
            ImGui::Text(this->toolchain->GetTriplet("host").c_str());

            int host_archs_items_current = 0;

            for (size_t i = 0; i < archs_items.size(); ++i)
            {
                if (std::strcmp(this->m_currentSave->host_arch, archs_items[i]) == 0)
                {
                    host_archs_items_current = i;
                    break;
                }
            }
            if (ImGui::BeginCombo("Host architecture (final build environment)", archs_items[host_archs_items_current]))
            {
                for (int i = 0; i < archs_items.size(); ++i)
                {
                    bool is_selected = (host_archs_items_current == i);
                    if (ImGui::Selectable(archs_items[i], is_selected))
                    {
                        std::strcpy(this->m_currentSave->host_arch, archs_items[i]);

                        host_archs_items_current = i;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                        // Set the save build arch
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::InputText("Host vendor", this->m_currentSave->host_vendor, 128);
            ImGui::InputText("Host platform", this->m_currentSave->host_platform, 128);

            ImGui::Separator();
            {

                float oldsize = ImGui::GetFont()->Scale;
                ImGui::GetFont()->Scale *= 1.3;
                ImGui::PushFont(ImGui::GetFont());

                ImGui::Text("Toolchain Target system configuration");

                ImGui::GetFont()->Scale = oldsize;
                ImGui::PopFont();
            }

            ImGui::Text("The target triplet will be : ");
            ImGui::SameLine();
            ImGui::Text(this->toolchain->GetTriplet("target").c_str());

            int target_archs_items_current = 0;

            for (size_t i = 0; i < archs_items.size(); ++i)
            {
                if (std::strcmp(this->m_currentSave->target_arch, archs_items[i]) == 0)
                {
                    target_archs_items_current = i;
                    break;
                }
            }
            if (ImGui::BeginCombo("Target architecture (final target system)", archs_items[target_archs_items_current]))
            {
                for (int i = 0; i < archs_items.size(); ++i)
                {
                    bool is_selected = (target_archs_items_current == i);
                    if (ImGui::Selectable(archs_items[i], is_selected))
                    {
                        std::strcpy(this->m_currentSave->target_arch, archs_items[i]);

                        target_archs_items_current = i;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus(); // Met en surbrillance l'élément sélectionné
                        // Set the save build arch
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::InputText("Target vendor", this->m_currentSave->target_vendor, 128);
            ImGui::InputText("Target platform", this->m_currentSave->target_platform, 128);

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

        // Project Settings
        if (selected == 2)
        {

            ImGui::BeginGroup();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(9.0f, 9.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::Text("Dist configurations");
            ImGui::Separator();

            ImGui::InputText("AR Value", this->m_currentDistSave->AR_value, 128);
            ImGui::InputText("AS Value", this->m_currentDistSave->AS_value, 128);
            ImGui::InputText("CC Value", this->m_currentDistSave->CC_value, 128);
            ImGui::InputText("CXX Value", this->m_currentDistSave->CXX_value, 128);
            ImGui::InputText("LD Value", this->m_currentDistSave->LD_value, 128);
            ImGui::InputText("RANLIB Value", this->m_currentDistSave->RANLIB_value, 128);
            ImGui::InputText("STRIP Value", this->m_currentDistSave->STRIP_value, 128);

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
                //VortexMaker::DeleteToolchain(this->toolchain);
                //this->opened = false;
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
