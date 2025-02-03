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
        m_AppWindow->SetInternalPaddingX(8.0f);
        m_AppWindow->SetInternalPaddingY(8.0f);
        std::shared_ptr<AppWindow> win = m_AppWindow;
        m_CmdInputValue = std::make_shared<std::string>("");

        m_AppWindow->m_Closable = true;
        m_AppWindow->SetCloseCallback([this]()
                                      { Cherry::DeleteAppWindow(m_AppWindow); });

        m_AppWindow->SetLeftMenubarCallback([]()
                                            {
                                                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));

                                                {
                                                    std::shared_ptr<ImageTextButtonSimple> btn = Application::Get().CreateComponent<ImageTextButtonSimple>("add_button", "Clear", Application::CookPath("resources/imgs/icons/misc/icon_add.png"));
                                                    btn->SetScale(0.85f);
                                                    btn->SetInternalMarginX(10.0f);
                                                    btn->SetLogoSize(15, 15);

                                                    if (btn->Render())
                                                    {
                                                    }
                                                }
                                                ImGui::PopStyleColor(); });

        m_AppWindow->SetLeftBottombarCallback([this]()
                                              {
                                                {
                                                    std::shared_ptr<ImageTextButtonSimple> btn = Application::Get().CreateComponent<ImageTextButtonSimple>("add_button", "Options.add", Application::CookPath("resources/imgs/icons/misc/icon_add.png"));
                                                    btn->SetScale(0.85f);
                                                    btn->SetInternalMarginX(10.0f);
                                                    btn->SetLogoSize(15, 15);

                                                    if (btn->Render())
                                                    {
                                                    }
                                                }
                                                  {
                                                      ImGui::SetNextItemWidth(300.0f);
                                                      auto input = Application::Get().CreateComponent<SimpleStringInput>("keyvaldouble_1", m_CmdInputValue, "Simple string value");
                                                      input->Render("input");
                                                  } });

        this->ctx = VortexMaker::GetCurrentContext();
    }

    std::shared_ptr<Cherry::AppWindow> &LogsUtilityAppWindow::GetAppWindow()
    {
        return m_AppWindow;
    }

    std::shared_ptr<LogsUtilityAppWindow> LogsUtilityAppWindow::Create(const std::string &name)
    {
        auto instance = std::shared_ptr<LogsUtilityAppWindow>(new LogsUtilityAppWindow(name));
        instance->SetupRenderCallback();
        return instance;
    }

    void LogsUtilityAppWindow::SetupRenderCallback()
    {
        auto self = shared_from_this();
        m_AppWindow->SetRenderCallback([self]()
                                       {
            if (self) {
                self->Render();
            } });
    }

    void LogsUtilityAppWindow::Render()
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));

        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame;

        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

        if (ImGui::BeginTable("LogsTable", 4, flags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 6.0f));

            ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 6.0f);
            ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 9.0f);
            ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableHeadersRow();

            ImGui::PopStyleVar();
            for (auto log : VortexMaker::GetCurrentContext()->registered_logs)
            {
                ImGui::TableNextRow();

                if ((log->m_level == spdlog::level::critical && !FatalFilter) ||
                    (log->m_level == spdlog::level::err && !ErrorFilter) ||
                    (log->m_level == spdlog::level::warn && !WarnFilter) ||
                    (log->m_level == spdlog::level::info && !InfoFilter))
                {
                    continue;
                }

                bool is_hovered = ImGui::IsItemHovered();
                if (is_hovered)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                }

                if (ImGui::IsMouseClicked(0) && is_hovered)
                {
                    std::string content_to_copy = log->m_timestamp + " | " + log->m_filter + " | " + log->m_message;
                    ImGui::SetClipboardText(content_to_copy.c_str());
                }

                for (int i = 0; i <= 3; i++)
                {
                    ImGui::TableSetColumnIndex(i);
                    if (i == 0)
                    {
                        ImVec4 color;
                        switch (log->m_level) // Add "User input" in blue
                        {
                        case spdlog::level::critical:
                            color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                            break;
                        case spdlog::level::err:
                            color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
                            break;
                        case spdlog::level::warn:
                            color = ImVec4(0.8f, 0.8f, 0.0f, 1.0f);
                            break;
                        case spdlog::level::info:
                            color = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
                            break;
                        default:
                            color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                            break;
                        }

                        ImVec2 pos = ImGui::GetCursorScreenPos();
                        float width = ImGui::GetFontSize() * 0.4f;
                        float height = ImGui::GetFontSize() * 1.2f;
                        float rounding = 3.0f;
                        ImVec2 rect_min = pos;
                        ImVec2 rect_max = ImVec2(pos.x + width, pos.y + height);

                        ImGui::GetWindowDrawList()->AddRectFilled(rect_min, rect_max, ImGui::GetColorU32(color), rounding);

                        switch (log->m_level)
                        {
                        case spdlog::level::critical:
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
                            ImGui::Text("FATAL");
                            break;
                        case spdlog::level::err:
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
                            ImGui::Text("Error");
                            break;
                        case spdlog::level::warn:
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
                            ImGui::Text("Warning");
                            break;
                        case spdlog::level::info:
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
                            ImGui::Text("Info");
                            break;
                        default:
                            ImGui::Text("");
                            break;
                        }
                    }
                    else if (i == 1)
                    {
                        float column_width = ImGui::GetColumnWidth();
                        float text_width = ImGui::CalcTextSize(log->m_filter.c_str()).x;
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->m_filter.c_str());
                    }
                    else if (i == 2)
                    {
                        float column_width = ImGui::GetColumnWidth();
                        float text_width = ImGui::CalcTextSize(log->m_timestamp.c_str()).x;
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->m_timestamp.c_str());
                    }
                    else if (i == 3)
                    {
                        ImVec4 message_color;
                        if (log->m_level == spdlog::level::critical)
                        {
                            message_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
                        }
                        else if (log->m_level == spdlog::level::err)
                        {
                            message_color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
                        }
                        else if (log->m_level == spdlog::level::warn)
                        {
                            message_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                        }
                        else
                        {
                            message_color = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
                        }

                        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetColumnWidth());
                        ImGui::TextColored(message_color, log->m_message.c_str());
                        ImGui::PopTextWrapPos();
                    }
                }

                if (is_hovered)
                {
                    ImGui::PopStyleColor();
                }
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleColor();
    }

    void LogsUtilityAppWindow::OnImGuiRender()
    {
    }

    void LogsUtilityAppWindow::menubar()
    {
        //
    }

}