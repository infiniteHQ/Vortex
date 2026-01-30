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
    float oldsize = CherryGUI::GetFont()->Scale;
    CherryGUI::GetFont()->Scale *= 1.3;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module :");
    CherryGUI::SameLine();
    CherryGUI::Text(this->m_module->m_proper_name.c_str());

    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module State :");
    CherryGUI::SameLine();
    if (this->m_module->m_state == "running")
    {
        CherryGUI::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 0.8f), "Running");
    }
    if (this->m_module->m_state == "failed")
    {
        CherryGUI::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 0.8f), "Failed");
    }
    if (this->m_module->m_state == "unknow" || this->m_module->m_state == "stopped")
    {
        CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Not running");
    }

    CherryGUI::GetFont()->Scale = oldsize;
    CherryGUI::PopFont();

    CherryGUI::Separator();

    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module name : ");
    CherryGUI::SameLine();
    CherryGUI::Text(this->m_module->m_name.c_str());

    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module version : ");
    CherryGUI::SameLine();
    CherryGUI::Text(this->m_module->m_version.c_str());

    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Compatible with Vortex version(s) : ");
    CherryGUI::SameLine();
    static std::string version;
    version.clear();
    for (auto ver : this->m_module->m_supported_versions)
    {
        version += "[" + ver + "]";
    }
    CherryGUI::Text(version.c_str());

    CherryGUI::Separator();
    if (CherryGUI::CollapsingHeader("Dependancies"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (CherryGUI::BeginTable("table_deps", 4, flags))
        {
            CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableHeadersRow();
            for (int i = 0; i < this->m_module->m_dependencies.size(); i++)
            {
                CherryGUI::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    CherryGUI::TableSetColumnIndex(column);

                    if (column == 0)
                    {
                        CherryGUI::Text(this->m_module->m_dependencies[i]->name.c_str());
                    }
                    else if (column == 1)
                    {
                        CherryGUI::Text(this->m_module->m_dependencies[i]->type.c_str());
                    }
                    else if (column == 2)
                    {
                        static std::string versions;
                        versions.clear();
                        for (auto ver : this->m_module->m_dependencies[i]->supported_versions)
                        {
                            version += "[" + ver + "] ";
                        }

                        CherryGUI::Text(versions.c_str());
                    }
                    else if (column == 3)
                    {

                        if (this->m_module->m_dependencies[i]->satisfied)
                        {
                            CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Satisfied");
                        }
                        else
                        {
                            CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not satisfied");
                        }
                    }
                }
            }
            CherryGUI::EndTable();
        }
    }
    if (CherryGUI::CollapsingHeader("Instances"))
    {
        // Behavior
    }
    if (CherryGUI::CollapsingHeader("Input events"))
    {
        for (int row = 0; row < this->m_module->m_input_events.size(); row++)
        {
            if (CherryGUI::TreeNode(this->m_module->m_input_events[row]->m_name.c_str()))
            {

                CherryGUI::BeginChildFrame(CherryGUI::GetID(this->m_module->m_input_events[row]->m_name.c_str()), ImVec2(0, 400));
                CherryGUI::Text("Name : ");
                CherryGUI::SameLine();
                CherryGUI::Text(this->m_module->m_input_events[row]->m_name.c_str());
                CherryGUI::Text("Description : ");
                CherryGUI::SameLine();
                CherryGUI::Text(this->m_module->m_input_events[row]->m_description.c_str());

                if (this->m_module->m_input_events[row]->m_devflag == DevFlag::READY)
                {
                    CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
                }
                else if (this->m_module->m_input_events[row]->m_devflag == DevFlag::DEPRECIATED)
                {
                    CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Depreciated");
                }

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (CherryGUI::BeginTable("table_input_event_args", 3, flags))
                {
                    CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    CherryGUI::TableSetupColumn("C++ type", ImGuiTableColumnFlags_WidthFixed);
                    CherryGUI::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                    CherryGUI::TableHeadersRow();
                    for (int param = 0; param < this->m_module->m_input_events[row]->m_params.size(); param++)
                    {
                        CherryGUI::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            CherryGUI::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                CherryGUI::Text(std::get<0>(this->m_module->m_input_events[row]->m_params[param]).c_str());
                            }
                            if (column == 1)
                            {
                                CherryGUI::Text(std::get<1>(this->m_module->m_input_events[row]->m_params[param]).c_str());
                            }
                            if (column == 2)
                            {
                                CherryGUI::Text(std::get<2>(this->m_module->m_input_events[row]->m_params[param]).c_str());
                            }
                        }
                    }
                    CherryGUI::EndTable();
                }
                CherryGUI::EndChildFrame();
                CherryGUI::TreePop();
            }
        }
    }
    if (CherryGUI::CollapsingHeader("Functions"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        for (int row = 0; row < this->m_module->m_functions.size(); row++)
        {
            if (CherryGUI::TreeNode(this->m_module->m_functions[row]->m_name.c_str()))
            {
                CherryGUI::BeginChildFrame(CherryGUI::GetID(this->m_module->m_functions[row]->m_name.c_str()), ImVec2(0, 400));
                CherryGUI::Text("Name : ");
                CherryGUI::SameLine();
                CherryGUI::Text(this->m_module->m_functions[row]->m_name.c_str());
                CherryGUI::Text("Description : ");
                CherryGUI::SameLine();
                CherryGUI::Text(this->m_module->m_functions[row]->m_description.c_str());

                /*if (this->m_module->m_functions[row]->m_devflag == DevFlag::READY)
                {
                    CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
                }
                else if (this->m_module->m_functions[row]->m_devflag == DevFlag::DEPRECIATED)
                {
                    CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Depreciated");
                }*/

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (CherryGUI::BeginTable("table_input_event_args", 3, flags))
                {
                    CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                    CherryGUI::TableSetupColumn("C++ type", ImGuiTableColumnFlags_WidthFixed);
                    CherryGUI::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                    CherryGUI::TableHeadersRow();
                    for (int param = 0; param < this->m_module->m_functions[row]->m_params_def.size(); param++)
                    {
                        CherryGUI::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            CherryGUI::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                CherryGUI::Text(std::get<0>(this->m_module->m_functions[row]->m_params_def[param]).c_str());
                            }
                            if (column == 1)
                            {
                                CherryGUI::Text(std::get<1>(this->m_module->m_functions[row]->m_params_def[param]).c_str());
                            }
                            if (column == 2)
                            {
                                CherryGUI::Text(std::get<2>(this->m_module->m_functions[row]->m_params_def[param]).c_str());
                            }
                        }
                    }
                    CherryGUI::EndTable();
                }
                CherryGUI::EndChildFrame();
                CherryGUI::TreePop();
            }
        }
    }
    if (CherryGUI::CollapsingHeader("History"))
    {
        // Events boostrapped, with informations, args, return ,etc...
        // Behavior
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (CherryGUI::BeginTable("table_", 4, flags))
        {
            CherryGUI::TableSetupColumn("Trigger", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableHeadersRow();
            for (int i = 0; i < this->m_module->m_input_events.size(); i++)
            {
                for (int row = 0; row < this->m_module->m_input_events[i]->m_happenings.size(); row++)
                {
                    CherryGUI::TableNextRow();
                    for (int column = 0; column < 4; column++)
                    {
                        CherryGUI::TableSetColumnIndex(column);

                        if (column == 0)
                        {
                            CherryGUI::Text(this->m_module->m_input_events[i]->m_happenings[row]->m_trigger_name.c_str());
                        }
                        else if (column == 1)
                        {
                            if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::FATAL)
                            {
                                CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
                            }
                            else if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::ERR)
                            {
                                CherryGUI::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
                            }
                            else if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::WARNING)
                            {
                                CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
                            }
                            else if (this->m_module->m_input_events[i]->m_happenings[row]->m_state == HappeningState::INFO)
                            {
                                CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
                            }
                        }
                        else if (column == 2)
                        {
                            CherryGUI::Text(this->m_module->m_input_events[i]->m_happenings[row]->m_timestamp.c_str());
                        }
                        else if (column == 2)
                        {
                            CherryGUI::Text(this->m_module->m_input_events[i]->m_happenings[row]->m_log.c_str());
                        }
                    }
                }
            }
            CherryGUI::EndTable();
        }
    }
    if (CherryGUI::CollapsingHeader("Logs"))
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

        if (CherryGUI::BeginTable("table_", 4, flags))
        {
            CherryGUI::TableSetupColumn("Trigger", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
            CherryGUI::TableHeadersRow();
            for (int i = 0; i < logs.size(); i++)
            {
                CherryGUI::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    CherryGUI::TableSetColumnIndex(column);
                    if (column == 0)
                    {
                        if (logs[i]->m_level == spdlog::level::critical)
                        {
                            CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
                        }
                        else if (logs[i]->m_level == spdlog::level::err)
                        {
                            CherryGUI::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
                        }
                        else if (logs[i]->m_level == spdlog::level::warn)
                        {
                            CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
                        }
                        else if (logs[i]->m_level == spdlog::level::info)
                        {
                            CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
                        }
                    }
                    else if (column == 1)
                    {
                        CherryGUI::Text(logs[i]->m_timestamp.c_str());
                    }
                    else if (column == 2)
                    {
                        CherryGUI::Text(logs[i]->m_filter.c_str());
                    }
                    else if (column == 3)
                    {
                        CherryGUI::TextWrapped(logs[i]->m_message.c_str());
                    }
                }
            }
            CherryGUI::EndTable();
        }
    }
}

void ModuleDetails::menubar()
{
    if (CherryGUI::BeginMenuBar())
    {
        CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Details of module :\"");
        CherryGUI::Text(this->m_module->m_proper_name.c_str());
        CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\" with name : \"");
        CherryGUI::Text(this->m_module->m_name.c_str());
        CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\"");

        CherryGUI::EndMenuBar();
    }
}
