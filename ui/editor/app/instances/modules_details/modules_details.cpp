#include "modules_details.hpp"

using namespace Cherry;

ModuleDetails::ModuleDetails(const std::string &name, const std::shared_ptr<ModuleInterface> &module)
{
    this->m_module = module;

    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
    m_AppWindow->SetClosable(false);

    m_AppWindow->SetInternalPaddingX(10.0f);
    m_AppWindow->SetInternalPaddingY(10.0f);
};

std::shared_ptr<Cherry::AppWindow> &ModuleDetails::GetAppWindow()
{
    return m_AppWindow;
}

std::shared_ptr<ModuleDetails> ModuleDetails::Create(const std::string &name, const std::shared_ptr<ModuleInterface> &module)
{
    auto instance = std::shared_ptr<ModuleDetails>(new ModuleDetails(name, module));
    instance->SetupRenderCallback();
    return instance;
}

void ModuleDetails::SetupRenderCallback()
{
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]()
                                   {
            if (self) {
                self->Render();
            } });
}

void ModuleDetails::close()
{
    this->opened = false;
}

void ModuleDetails::Render()
{
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
    for (auto ver : this->m_module->m_supported_versions)
    {
        version += "[" + ver + "]";
    }
    ImGui::Text(version.c_str());

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Dependancies"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table_deps", 4, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            for (int i = 0; i < this->m_module->m_dependencies.size(); i++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (column == 0)
                    {
                        ImGui::Text(this->m_module->m_dependencies[i]->name.c_str());
                    }
                    else if (column == 1)
                    {
                        ImGui::Text(this->m_module->m_dependencies[i]->type.c_str());
                    }
                    else if (column == 2)
                    {
                        static std::string versions;
                        versions.clear();
                        for (auto ver : this->m_module->m_dependencies[i]->supported_versions)
                        {
                            version += "[" + ver + "] ";
                        }

                        ImGui::Text(versions.c_str());
                    }
                    else if (column == 3)
                    {

                        if (this->m_module->m_dependencies[i]->satisfied)
                        {
                            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Satisfied");
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not satisfied");
                        }
                    }
                }
            }
            ImGui::EndTable();
        }
    }
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

                ImGui::BeginChildFrame(ImGui::GetID(this->m_module->m_input_events[row]->m_name.c_str()), ImVec2(0, 400));
                ImGui::Text("Name : ");
                ImGui::SameLine();
                ImGui::Text(this->m_module->m_input_events[row]->m_name.c_str());
                ImGui::Text("Description : ");
                ImGui::SameLine();
                ImGui::Text(this->m_module->m_input_events[row]->m_description.c_str());

                if (this->m_module->m_input_events[row]->m_devflag == DevFlag::READY)
                {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
                }
                else if (this->m_module->m_input_events[row]->m_devflag == DevFlag::DEPRECIATED)
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
                ImGui::EndChildFrame();
                ImGui::TreePop();
            }
        }
    }
    if (ImGui::CollapsingHeader("Functions"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        for (int row = 0; row < this->m_module->m_functions.size(); row++)
        {
            if (ImGui::TreeNode(this->m_module->m_functions[row]->m_name.c_str()))
            {
                ImGui::BeginChildFrame(ImGui::GetID(this->m_module->m_functions[row]->m_name.c_str()), ImVec2(0, 400));
                ImGui::Text("Name : ");
                ImGui::SameLine();
                ImGui::Text(this->m_module->m_functions[row]->m_name.c_str());
                ImGui::Text("Description : ");
                ImGui::SameLine();
                ImGui::Text(this->m_module->m_functions[row]->m_description.c_str());

                /*if (this->m_module->m_functions[row]->m_devflag == DevFlag::READY)
                {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
                }
                else if (this->m_module->m_functions[row]->m_devflag == DevFlag::DEPRECIATED)
                {
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Depreciated");
                }*/

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("table_input_event_args", 3, flags))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("C++ type", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    for (int param = 0; param < this->m_module->m_functions[row]->m_params_def.size(); param++)
                    {
                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                ImGui::Text(std::get<0>(this->m_module->m_functions[row]->m_params_def[param]).c_str());
                            }
                            if (column == 1)
                            {
                                ImGui::Text(std::get<1>(this->m_module->m_functions[row]->m_params_def[param]).c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text(std::get<2>(this->m_module->m_functions[row]->m_params_def[param]).c_str());
                            }
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChildFrame();
                ImGui::TreePop();
            }
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
                        ImGui::TextWrapped(logs[i]->m_message.c_str());
                    }
                }
            }
            ImGui::EndTable();
        }
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
