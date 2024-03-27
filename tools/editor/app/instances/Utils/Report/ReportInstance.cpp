#include "ReportInstance.h"

using namespace VortexMaker;

ReportInstance::ReportInstance(VxContext *ctx, std::shared_ptr<Task> _task)
{
    this->m_ctx = ctx;
    this->task = _task;

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

void ReportInstance::close()
{
    this->opened = false;
}

std::string ReportInstance::render()
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
        std::string label = "Reporting task" + this->task->id + "###" + this->task->id + "-reportingtask";
        if (ImGui::Begin(label.c_str(), &flipIcon, &this->opened, ImGuiWindowFlags_MenuBar))
        {
            this->menubar();

            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.3;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Task :");
            ImGui::SameLine();
            ImGui::Text(this->task->tasktype.c_str());


            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Task component :");
            ImGui::SameLine();
            ImGui::Text(this->task->component.c_str());

            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Task Identifier : ");
            ImGui::SameLine();
            ImGui::Text(this->task->id.c_str());


            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Number of checks : ");
            ImGui::SameLine();
            ImGui::Text(std::to_string(this->task->checkList.size()).c_str());

            
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Started : ");
            ImGui::SameLine();
            ImGui::Text(this->task->startTime().c_str());
            ImGui::Separator();

                            std::string errorButtonID = std::to_string(this->task->failCounter) + "###" + task->id + "-error";
                            if (ImGui::ImageButtonWithText(errorIcon, errorButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }
                            ImGui::SameLine();

                            std::string warnButtonID = std::to_string(this->task->warningCounter) + "###" +  task->id + "-error";
                            if (ImGui::ImageButtonWithText(warnIcon, warnButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }
                            ImGui::SameLine();

                            std::string successButtonID = std::to_string(this->task->successCounter) + "###" + task->id + "-error";
                            if (ImGui::ImageButtonWithText(successIcon, successButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }

                            ImGui::SameLine();

                            std::string unknowButtonID = std::to_string(this->task->unknowCounter) + "###" + task->id + "-error";
                            if (ImGui::ImageButtonWithText(unknowIcon, unknowButtonID.c_str(), ImVec2(this->m_ErrorIcon->GetWidth(), this->m_ErrorIcon->GetHeight())))
                            {
                            }
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Checks report"))
            {
                ImGuiID id = ImGui::GetID(label.c_str());
                ImGui::BeginChildFrame(id, ImVec2(0, 450), true);

                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::CollapsingHeader("Errors"))
                {
                    if (ImGui::BeginTable("table_", 4, flags))
                    {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Directive", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        std::vector<std::shared_ptr<Check>> errors;
                        for (auto check : this->task->checkList)
                        {
                            if (check->checkResult == "failed")
                            {
                                errors.push_back(check);
                            }
                        }

                        for (int row = 0; row < errors.size(); row++)
                        {

                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(errors[row]->checkID.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errors[row]->checkResult.c_str());
                                }
                                if (column == 2)
                                {
                                    
                                    ImGui::TextWrapped(errors[row]->checkLog.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::TextWrapped(errors[row]->checkDirective.c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    }
                }
           
                if (ImGui::CollapsingHeader("Warnings"))
                {
                    if (ImGui::BeginTable("table_", 4, flags))
                    {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Directive", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                         std::vector<std::shared_ptr<Check>> warnings;
                        for (auto check : this->task->checkList)
                        {
                            if (check->checkResult == "warning")
                            {
                                warnings.push_back(check);
                            }
                        }

                        for (int row = 0; row < warnings.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(warnings[row]->checkID.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.0f, 1.0f), warnings[row]->checkResult.c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::TextWrapped(warnings[row]->checkLog.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::TextWrapped(warnings[row]->checkDirective.c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    }
                }  
                
                if (ImGui::CollapsingHeader("Unknow"))
                {
                    if (ImGui::BeginTable("table_", 4, flags))
                    {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Directive", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        std::vector<std::shared_ptr<Check>> unkonws;
                        for (auto check : this->task->checkList)
                        {
                            if (check->checkResult == "unknow")
                            {
                                unkonws.push_back(check);
                            }
                        }

                        for (int row = 0; row < unkonws.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(unkonws[row]->checkID.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), unkonws[row]->checkResult.c_str());
                                }
                                if (column == 2)
                                {
                                    
                                    ImGui::TextWrapped(unkonws[row]->checkLog.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::TextWrapped(unkonws[row]->checkDirective.c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    }
                }
           
                
                if (ImGui::CollapsingHeader("Successes"))
                {
                    if (ImGui::BeginTable("table_", 4, flags))
                    {
                        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Result", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Directive", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();
                        std::vector<std::shared_ptr<Check>> successes;
                        for (auto check : this->task->checkList)
                        {
                            if (check->checkResult == "success")
                            {
                                successes.push_back(check);
                            }
                        }


                        for (int row = 0; row < successes.size(); row++)
                        {
                        std::cout << label << " << " << successes[row] << std::endl;
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 4; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(successes[row]->checkID.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), successes[row]->checkResult.c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::TextWrapped(successes[row]->checkLog.c_str());
                                }
                                if (column == 3)
                                {
                                    ImGui::TextWrapped(successes[row]->checkDirective.c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    }
                }
          
                ImGui::EndChildFrame();
            }

            if (ImGui::CollapsingHeader("Used Variables"))
            {                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    if (ImGui::BeginTable("table_used_variables", 3, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Owner", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < this->task->used_variables.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 3; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(std::get<0>(this->task->used_variables[row]).c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::Text(std::get<1>(this->task->used_variables[row]).c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::Text(std::get<2>(this->task->used_variables[row]).c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    
                }
            }

            if (ImGui::CollapsingHeader("Created Variables"))
            {                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    if (ImGui::BeginTable("table_created_variables", 3, flags))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Owner", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < this->task->created_variables.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 3; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(std::get<0>(this->task->created_variables[row]).c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::TextColored(ImVec4(0.2f, 0.2f , 1.0f, 1.0f),std::get<1>(this->task->created_variables[row]).c_str());
                                }
                                if (column == 2)
                                {
                                    ImGui::Text(std::get<2>(this->task->created_variables[row]).c_str());
                                }
                            }
                        }

                        ImGui::EndTable();
                    
                }
            }
            
            

            if (ImGui::CollapsingHeader("Props"))
            {                static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                    if (ImGui::BeginTable("prop_name", 3, flags))
                    {
                        ImGui::TableSetupColumn("Prop", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Name/Label", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (int row = 0; row < this->task->depsChecks.size(); row++)
                        {
                            static std::pair<char[128], char[128]> newItem;
                            static char label[128];

                            ImGui::TableNextRow();
                            for (int column = 0; column < 3; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text(this->task->depsChecks[row].first.c_str());
                                }
                                if (column == 1)
                                {
                                    if(this->task->depsChecks[row].second == "missing"){
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "missing");
                                    }
                                    
                                    if(this->task->depsChecks[row].second == "satisfied"){
                                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "satisfied");
                                    }
                                    //ImGui::Text(this->task->depsChecks[row].second.c_str());
                                }
                                if (column == 2)
                                {
                                    bool finded = false;
                                    for(auto envProp : this->task->depsChecksSpec){
                                        if(envProp.first == this->task->depsChecks[row].first){
                                            finded = true;
                                            ImGui::Text(envProp.second.c_str());
                                        }
                                    }
                                    
                                    if(!finded){
                                            ImGui::Text("???");
                                    }
                                }
                            }
                        }

                        ImGui::EndTable();
                    
                }
            }

            
            
            
            if (ImGui::CollapsingHeader("Metrics"))
            {
                static int i0 = 123;
                ImGui::InputInt("input int", &i0);

                static float f0 = 0.001f;
                ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

                static double d0 = 999999.00000001;
                ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

                static float f1 = 1.e10f;
                ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
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

void ReportInstance::menubar()
{
    if (ImGui::BeginMenuBar())
    {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Reporting task :\"");
            ImGui::Text(this->task->component.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\" with id : \"");
            ImGui::Text(this->task->id.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\"");
    
    ImGui::EndMenuBar();
    }
}
