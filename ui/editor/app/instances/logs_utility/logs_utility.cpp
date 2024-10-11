#include "logs_utility.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;

namespace VortexEditor
{
    LogsUtilityAppWindow::LogsUtilityAppWindow(const std::string &name)
    {
        m_AppWindow = std::make_shared<AppWindow>(name, name);
        m_AppWindow->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
        std::shared_ptr<AppWindow> win = m_AppWindow;

        this->ctx = VortexMaker::GetCurrentContext();
    }

    void LogsUtilityAppWindow::RefreshRender(const std::shared_ptr<LogsUtilityAppWindow> &instance)
    {
        m_AppWindow->SetRenderCallback([instance]()
                                       {
                                           float oldsize = ImGui::GetFont()->Scale;
                                           ImGui::GetFont()->Scale *= 1.3;
                                           ImGui::PushFont(ImGui::GetFont());

                                           ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Project contents of : ");
                                           ImGui::SameLine();
                                           // ImGui::Text(this->ctx->name.c_str());

                                           ImGui::GetFont()->Scale = oldsize;
                                           ImGui::PopFont();

                                           ImGui::Separator();

                                           static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
                                           const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

                                           if (ImGui::BeginTable("3ways", 4, flags))
                                           {
                                               // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                                               ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_NoHide);
                                               ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
                                               ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
                                               ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
                                               ImGui::TableHeadersRow();

                                               for (auto log : VortexMaker::GetCurrentContext()->registered_logs)
                                               {
                                                   if (log->m_level == spdlog::level::critical && !FatalFilter)
                                                       continue;
                                                   if (log->m_level == spdlog::level::err && !ErrorFilter)
                                                       continue;
                                                   if (log->m_level == spdlog::level::warn && !WarnFilter)
                                                       continue;
                                                   if (log->m_level == spdlog::level::info && !InfoFilter)
                                                       continue;

                                                   ImGui::TableNextRow();
                                                   for (int i = 0; i <= 3; i++)
                                                   {
                                                       ImGui::TableSetColumnIndex(i);
                                                       if (i == 0)
                                                       {
                                                           if (log->m_level == spdlog::level::critical)
                                                           {
                                                               ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
                                                           }
                                                           else if (log->m_level == spdlog::level::err)
                                                           {
                                                               ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
                                                           }
                                                           else if (log->m_level == spdlog::level::warn)
                                                           {
                                                               ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
                                                           }
                                                           else if (log->m_level == spdlog::level::info)
                                                           {
                                                               ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
                                                           }
                                                       }
                                                       else if (i == 1)
                                                       {
                                                           ImGui::Text(log->m_timestamp.c_str());
                                                       }
                                                       else if (i == 2)
                                                       {
                                                           ImGui::Text(log->m_filter.c_str());
                                                       }
                                                       else if (i == 3)
                                                       {
                                                           ImGui::Text(log->m_message.c_str());
                                                       }
                                                   }
                                               }
                                               ImGui::EndTable();
                                           }
                                       });
    }

    void LogsUtilityAppWindow::OnImGuiRender()
    {
    }

    void LogsUtilityAppWindow::menubar()
    {
        static ImTextureID refreshIcon = this->m_RefreshIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        static ImTextureID addIcon = this->m_AddIcon->GetImGuiTextureID(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::ImageButtonWithText(refreshIcon, "Refresh", ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
            {
            }
            if (ImGui::ImageButtonWithText(addIcon, "Add", ImVec2(this->m_AddIcon->GetWidth(), this->m_AddIcon->GetHeight())))
            {
                ImGui::OpenPopup("CreationMenu");
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Filters"))
            {
                ImGui::Checkbox("Show informations", &InfoFilter);
                ImGui::Checkbox("Show fatal errors", &FatalFilter);
                ImGui::Checkbox("Show errors", &ErrorFilter);
                ImGui::Checkbox("Show warnings", &WarnFilter);
                if (ImGui::MenuItem("Build/Rebuild single parts"))
                {
                    // Behavior
                }
                if (ImGui::MenuItem("Global build"))
                {
                    // Behavior
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

}