#include "ModuleDetails.h"

using namespace VortexMaker;

ModuleDetails::ModuleDetails(VxContext *ctx, std::shared_ptr<ModuleInterface> module)
{
    this->m_ctx = ctx;
    this->m_module = module;

    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_error, icons::i_error_size, w, h);
        m_ErrorIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_warn, icons::i_warn_size, w, h);
        m_WarningIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_unknow, icons::i_unknow_size, w, h);
        m_UnknowIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_success, icons::i_success_size, w, h);
        m_SuccessIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }

    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_save, icons::i_save_size, w, h);
        m_SaveIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }

    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_logs, icons::i_logs_size, w, h);
        m_FlipBookIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_eye, icons::i_eye_size, w, h);
        m_EyeIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_folder, icons::i_folder_size, w, h);
        m_FolderIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_tasklist, icons::i_tasklist_size, w, h);
        m_TaskListIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_settings, icons::i_settings_size, w, h);
        m_SettingsIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ToolchainIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_database, icons::i_database_size, w, h);
        m_DatabaseIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_pen, icons::i_pen_size, w, h);
        m_EditIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_build, icons::i_build_size, w, h);
        m_BuildIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_package, icons::i_package_size, w, h);
        m_PackageIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = Walnut::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA, data);
        free(data);
    }
};

void ModuleDetails::close()
{
    this->opened = false;
}

std::string ModuleDetails::render()
{
    static ImTextureID toolchainIcon = this->m_ToolchainIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        static ImTextureID errorIcon = this->m_ErrorIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID warnIcon = this->m_WarningIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID successIcon = this->m_SuccessIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID flipIcon = this->m_FlipBookIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID unknowIcon = this->m_UnknowIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (opened)
    {
        // Mainwindow with dockspace
        std::string label = "Module details" + this->m_module->m_proper_name + "###" + this->m_module->m_name + "-mod";
        if (ImGui::Begin(label.c_str(), &flipIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            this->menubar();

            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module :");
            ImGui::SameLine();
            ImGui::Text(this->m_module->m_proper_name.c_str());


            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module State :");
            ImGui::SameLine();
            ImGui::Text("?");

            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module name : ");
            ImGui::SameLine();
            ImGui::Text(this->m_module->m_name.c_str());
      
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Instances", &errorIcon, NULL))
            {
            }
            if (ImGui::CollapsingHeader("Events", &errorIcon, NULL))
            {
            }
            if (ImGui::CollapsingHeader("Functions", &errorIcon, NULL))
            {
                                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    if (ImGui::BeginTable("table_", 4, flags))
                    {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Directive", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        for (int row = 0; row < this->m_module->m_functions.size(); row++)
                        {

                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(this->m_module->m_functions[row]->m_name.c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    
                }
           
            }
            if (ImGui::CollapsingHeader("Logs", &errorIcon, NULL))
            {
            }

        }
        ImGui::End();

        return "rendering";
    }
    else
    {

        return "closed";
    }
}

void ModuleDetails::menubar()
{
    if (ImGui::BeginMenuBar())
    {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Details of module :\"");
            ImGui::Text(this->m_module->m_proper_name.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\" with name : \"");
            ImGui::Text(this->m_module->m_name.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\"");
    
    ImGui::EndMenuBar();
    }
}
