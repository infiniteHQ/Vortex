#include "ModuleDetails.h"

using namespace VortexMaker;

ModuleDetails::ModuleDetails(VxContext *ctx, std::shared_ptr<ModuleInterface> module)
{
    this->m_ctx = ctx;
    this->m_module = module;

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_error, icons::i_error_size, w, h);
        m_ErrorIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_warn, icons::i_warn_size, w, h);
        m_WarningIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_unknow, icons::i_unknow_size, w, h);
        m_UnknowIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_success, icons::i_success_size, w, h);
        m_SuccessIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_save, icons::i_save_size, w, h);
        m_SaveIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }

    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_logs, icons::i_logs_size, w, h);
        m_FlipBookIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_eye, icons::i_eye_size, w, h);
        m_EyeIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_add, icons::i_add_size, w, h);
        m_AddIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_folder, icons::i_folder_size, w, h);
        m_FolderIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_tasklist, icons::i_tasklist_size, w, h);
        m_TaskListIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_settings, icons::i_settings_size, w, h);
        m_SettingsIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_list, icons::i_list_size, w, h);
        m_ToolchainIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_database, icons::i_database_size, w, h);
        m_DatabaseIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_pen, icons::i_pen_size, w, h);
        m_EditIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_refresh, icons::i_refresh_size, w, h);
        m_RefreshIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_build, icons::i_build_size, w, h);
        m_BuildIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_package, icons::i_package_size, w, h);
        m_PackageIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
        free(data);
    }
    {
        uint32_t w, h;
        void *data = UIKit::Image::Decode(icons::i_trash, icons::i_trash_size, w, h);
        m_TrashIcon = std::make_shared<UIKit::Image>(w, h, UIKit::ImageFormat::RGBA, data);
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
            if (this->m_module->m_state == "running")
            {
                ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 0.8f), "Running");
            }
            if (this->m_module->m_state == "failed")
            {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 0.8f), "Failed");
            }
            if (this->m_module->m_state == "unknow" || this->m_module->m_state == "stopped")
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Not running");
            }


            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module name : ");
            ImGui::SameLine();
            ImGui::Text(this->m_module->m_name.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module version : ");
            ImGui::SameLine();
            ImGui::Text(this->m_module->m_version.c_str());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Compatible with Vortex version(s) : ");
            ImGui::SameLine();
            static std::string version;
            version.clear();
            for(auto ver : this->m_module->m_supported_versions){
                version += "[" + ver + "]";
            }
            ImGui::Text(version.c_str());

            ImGui::Separator();
            if (ImGui::CollapsingHeader("Instances"))
            {
                // Behavior
            }
            if (ImGui::CollapsingHeader("Input events"))
            {
                    for (int row = 0; row < this->m_module->m_input_events.size(); row++)
                    {

            if (ImGui::TreeNode(this->m_module->m_input_events[row]->m_name.c_str()))
            {
                ImGui::Text("Name : ");
                ImGui::SameLine();
                ImGui::Text(this->m_module->m_input_events[row]->m_name.c_str());
                ImGui::Text("Description : ");
                ImGui::SameLine();
                ImGui::Text(this->m_module->m_input_events[row]->m_description.c_str());

                if(this->m_module->m_input_events[row]->m_devflag == DevFlag::READY)
                {
                                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
                }
                else if(this->m_module->m_input_events[row]->m_devflag == DevFlag::DEPRECIATED)
                {
                                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Depreciated");
                }

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("table_input_event_args", 3, flags))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("C++ type", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    for (int param = 0; param < this->m_module->m_input_events[row]->m_params.size(); param++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                ImGui::Text(std::get<0>(this->m_module->m_input_events[row]->m_params[param]).c_str());
                            }
                            if (column == 1)
                            {
                                ImGui::Text(std::get<1>(this->m_module->m_input_events[row]->m_params[param]).c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text(std::get<2>(this->m_module->m_input_events[row]->m_params[param]).c_str());
                            }
                        }
                    }
                    ImGui::EndTable();
                }

            }
                            
                            }
                        
                    

            }
            if (ImGui::CollapsingHeader("Functions"))
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
            if (ImGui::CollapsingHeader("History"))
            {
                // Events boostrapped, with informations, args, return ,etc...
                // Behavior
                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("table_", 4, flags))
                {
                    ImGui::TableSetupColumn("Trigger", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    for (int i = 0; i < this->m_module->m_input_events.size(); i++)
                    {
                        for (int row = 0; row < this->m_module->m_input_events[i]->m_happenings.size(); row++)
                        {
                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(this->m_module->m_input_events[i]->m_happenings[row]->m_trigger_name.c_str());
                                }
                                else if (column == 1)
                                {
                                    if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::FATAL)
                                    {
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
                                    }
                                    else if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::ERROR)
                                    {
                                        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
                                    }
                                    else if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::WARNING)
                                    {
                                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
                                    }
                                    else if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::INFO)
                                    {
                                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
                                    }
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(this->m_module->m_input_events[i]->m_happenings[row]->m_timestamp.c_str());
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(this->m_module->m_input_events[i]->m_happenings[row]->m_log.c_str());
                                }
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            }
            if (ImGui::CollapsingHeader("Logs"))
            {
                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                static std::vector<std::shared_ptr<VxSystemLog>> logs;

                logs.clear();
                for (int i = 0; i < this->m_ctx->registered_logs.size(); i++)
                {
                    if (this->m_ctx->registered_logs[i]->m_filter == this->m_module->m_name)
                    {
                        logs.push_back(this->m_ctx->registered_logs[i]);
                    }
                }

                if (ImGui::BeginTable("table_", 4, flags))
                {
                    ImGui::TableSetupColumn("Trigger", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    for (int i = 0; i < logs.size(); i++)
                    {
                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                if (column == 0)
                                {
                                    if (logs[i]->m_level == spdlog::level::critical)
                                    {
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
                                    }
                                    else if (logs[i]->m_level == spdlog::level::err)
                                    {
                                        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
                                    }
                                    else if (logs[i]->m_level == spdlog::level::warn)
                                    {
                                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
                                    }
                                    else if (logs[i]->m_level == spdlog::level::info)
                                    {
                                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
                                    }
                                }
                                else if (column == 1)
                                {
                                    ImGui::Text(logs[i]->m_timestamp.c_str());
                                }
                                else if (column == 2)
                                {
                                    ImGui::Text(logs[i]->m_filter.c_str());
                                }
                                else if (column == 3)
                                {
                                    ImGui::Text(logs[i]->m_message.c_str());
                                }
                            }
                        
                    }
                    ImGui::EndTable();
                }
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
