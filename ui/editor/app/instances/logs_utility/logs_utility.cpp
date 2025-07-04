#include "logs_utility.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;

namespace VortexEditor {
LogsUtilityAppWindow::LogsUtilityAppWindow(const std::string &name) {
  std::cout << "LogsUtilityAppWindow" << std::endl;
  m_AppWindow = std::make_shared<AppWindow>(name, name);
  m_AppWindow->SetInternalPaddingX(8.0f);
  m_AppWindow->SetInternalPaddingY(8.0f);
  std::shared_ptr<AppWindow> win = m_AppWindow;

  m_AppWindow->m_Closable = true;
  m_AppWindow->SetCloseCallback(
      [this]() { Cherry::DeleteAppWindow(m_AppWindow); });

  m_AppWindow->SetLeftMenubarCallback([this]() {
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));

    {
      CherryGUI::SetNextItemWidth(500.0f);
      CherryKit::InputString("Search for logs", &m_LogSearchValue);

      /*CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
      auto input = Application::Get().CreateComponent<SimpleStringInput>(
          "keyvaldouble_1", m_LogSearchValue,
          "###Search for logs###SearchLogs");

      input->Render("input");
      CherryGUI::PopStyleVar();*/
    }

    if (CherryKit::ButtonImageDropdown(
            "resources/imgs/icons/misc/icon_settings.png")
            .GetDataAs<bool>("isClicked")) {
      ImVec2 mousePos = CherryGUI::GetMousePos();
      ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
      ImVec2 popupSize(150, 100);

      if (mousePos.x + popupSize.x > displaySize.x) {
        mousePos.x -= popupSize.x;
      }
      if (mousePos.y + popupSize.y > displaySize.y) {
        mousePos.y -= popupSize.y;
      }

      CherryGUI::SetNextWindowPos(mousePos);
      CherryGUI::OpenPopup("FilterMenu");
    }

    /*{
      static std::shared_ptr<Cherry::CustomDrowpdownImageButtonSimple> btn =
          std::make_shared<Cherry::CustomDrowpdownImageButtonSimple>(
              "filter_buttons", "Filters");
      btn->SetScale(0.85f);
      btn->SetInternalMarginX(10.0f);
      btn->SetLogoSize(15, 15);

      btn->SetDropDownImage(
          Application::CookPath("resources/imgs/icons/misc/icon_down.png"));
      btn->SetImagePath(
          Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"));
      if (btn->Render("LogicContentManager")) {
        ImVec2 mousePos = CherryGUI::GetMousePos();
        ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
        ImVec2 popupSize(150, 100);

        if (mousePos.x + popupSize.x > displaySize.x) {
          mousePos.x -= popupSize.x;
        }
        if (mousePos.y + popupSize.y > displaySize.y) {
          mousePos.y -= popupSize.y;
        }

        CherryGUI::SetNextWindowPos(mousePos);
        CherryGUI::OpenPopup("FilterMenu");
      }
    }*/

    if (CherryGUI::BeginPopup("FilterMenu")) {
      // CherryGUI::Checkbox("Show Filter pannel", &m_ShowFilterPannel);
      // CherryGUI::Checkbox("Show Thumbnail pannel",
      // &m_ShowThumbnailVisualizer);

      CherryGUI::EndPopup();
    }
    CherryGUI::PopStyleColor();
  });

  m_AppWindow->SetRightMenubarCallback([this]() {
    if (CherryKit::ButtonImageDropdown(
            "resources/imgs/icons/misc/icon_settings.png")
            .GetDataAs<bool>("isClicked")) {
      ImVec2 mousePos = CherryGUI::GetMousePos();
      ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
      ImVec2 popupSize(150, 100);

      if (mousePos.x + popupSize.x > displaySize.x) {
        mousePos.x -= popupSize.x;
      }
      if (mousePos.y + popupSize.y > displaySize.y) {
        mousePos.y -= popupSize.y;
      }

      CherryGUI::SetNextWindowPos(mousePos);
      CherryGUI::OpenPopup("OptionMenu");
    }

    /*{
      static std::shared_ptr<Cherry::CustomDrowpdownImageButtonSimple> btn =
          std::make_shared<Cherry::CustomDrowpdownImageButtonSimple>(
              "option_buttons", "Options");
      btn->SetScale(0.85f);
      btn->SetInternalMarginX(10.0f);
      btn->SetLogoSize(15, 15);

      btn->SetDropDownImage(
          Application::CookPath("resources/imgs/icons/misc/icon_down.png"));
      btn->SetImagePath(
          Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"));
      if (btn->Render("LogicContentManager")) {
        ImVec2 mousePos = CherryGUI::GetMousePos();
        ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
        ImVec2 popupSize(150, 100);

        if (mousePos.x + popupSize.x > displaySize.x) {
          mousePos.x -= popupSize.x;
        }
        if (mousePos.y + popupSize.y > displaySize.y) {
          mousePos.y -= popupSize.y;
        }

        CherryGUI::SetNextWindowPos(mousePos);
        CherryGUI::OpenPopup("OptionMenu");
      }
    }*/

    if (CherryGUI::BeginPopup("OptionMenu")) {
      // CherryGUI::Checkbox("Show Filter pannel", &m_ShowFilterPannel);
      // CherryGUI::Checkbox("Show Thumbnail pannel",
      // &m_ShowThumbnailVisualizer);

      CherryGUI::EndPopup();
    }
  });

  m_AppWindow->SetLeftBottombarCallback([this]() {
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 6.0f);

    CherryKit::ButtonImageText(
        "Send",
        Application::CookPath("resources/imgs/icons/misc/icon_send.png"));

    /*{
      std::shared_ptr<ImageTextButtonSimple> btn =
          Application::Get().CreateComponent<ImageTextButtonSimple>(
              "send_button", "Send",
              Application::CookPath("resources/imgs/icons/misc/icon_send.png"));
      btn->SetScale(0.85f);
      btn->SetInternalMarginX(10.0f);
      btn->SetLogoSize(15, 15);

      if (btn->Render()) {
      }
    }*/
    {
      CherryGUI::SetNextItemWidth(500.0f);
      CherryKit::InputString("Search for logs", &m_CmdInputValue);
    }
  });

  this->ctx = VortexMaker::GetCurrentContext();
  std::cout << "LogsUtilityAppWindow END" << std::endl;
}

std::shared_ptr<Cherry::AppWindow> &LogsUtilityAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<LogsUtilityAppWindow>
LogsUtilityAppWindow::Create(const std::string &name) {
  auto instance =
      std::shared_ptr<LogsUtilityAppWindow>(new LogsUtilityAppWindow(name));
  instance->SetupRenderCallback();
  return instance;
}

void LogsUtilityAppWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void LogsUtilityAppWindow::Render() {
  std::cout << "Render" << std::endl;
  CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));

  static ImGuiTableFlags flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable |
      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame;

  const float TEXT_BASE_WIDTH = CherryGUI::CalcTextSize("A").x;

  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

  if (CherryGUI::BeginTable("LogsTable", 4, flags)) {
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 6.0f));

    CherryGUI::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed,
                                TEXT_BASE_WIDTH * 6.0f);
    CherryGUI::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed,
                                TEXT_BASE_WIDTH * 9.0f);
    CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed,
                                TEXT_BASE_WIDTH * 15.0f);
    CherryGUI::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);

    CherryGUI::TableHeadersRow();

    CherryGUI::PopStyleVar();
    for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
      CherryGUI::TableNextRow();

      if ((log->m_level == spdlog::level::critical && !FatalFilter) ||
          (log->m_level == spdlog::level::err && !ErrorFilter) ||
          (log->m_level == spdlog::level::warn && !WarnFilter) ||
          (log->m_level == spdlog::level::info && !InfoFilter)) {
        continue;
      }

      bool is_hovered = CherryGUI::IsItemHovered();
      if (is_hovered) {
        CherryGUI::PushStyleColor(ImGuiCol_Text,
                                  ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      }

      if (CherryGUI::IsMouseClicked(0) && is_hovered) {
        std::string content_to_copy =
            log->m_timestamp + " | " + log->m_filter + " | " + log->m_message;
        ImGui::SetClipboardText(content_to_copy.c_str());
      }

      for (int i = 0; i <= 3; i++) {
        CherryGUI::TableSetColumnIndex(i);
        if (i == 0) {
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

          ImVec2 pos = CherryGUI::GetCursorScreenPos();
          float width = CherryGUI::GetFontSize() * 0.4f;
          float height = CherryGUI::GetFontSize() * 1.2f;
          float rounding = 3.0f;
          ImVec2 rect_min = pos;
          ImVec2 rect_max = ImVec2(pos.x + width, pos.y + height);

          CherryGUI::GetWindowDrawList()->AddRectFilled(
              rect_min, rect_max, CherryGUI::GetColorU32(color), rounding);

          switch (log->m_level) {
          case spdlog::level::critical:
            CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 10.0f);
            CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);
            CherryGUI::Text("FATAL");
            break;
          case spdlog::level::err:
            CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 10.0f);
            CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);
            CherryGUI::Text("Error");
            break;
          case spdlog::level::warn:
            CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 10.0f);
            CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);
            CherryGUI::Text("Warning");
            break;
          case spdlog::level::info:
            CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 4.0f);
            CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);
            CherryGUI::Text("Info");
            break;
          default:
            CherryGUI::Text("");
            break;
          }
        } else if (i == 1) {
          float column_width = CherryGUI::GetColumnWidth();
          float text_width = CherryGUI::CalcTextSize(log->m_filter.c_str()).x;
          CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f),
                                 log->m_filter.c_str());
        } else if (i == 2) {
          float column_width = CherryGUI::GetColumnWidth();
          float text_width =
              CherryGUI::CalcTextSize(log->m_timestamp.c_str()).x;
          CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f),
                                 log->m_timestamp.c_str());
        } else if (i == 3) {
          ImVec4 message_color;
          if (log->m_level == spdlog::level::critical) {
            message_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
          } else if (log->m_level == spdlog::level::err) {
            message_color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
          } else if (log->m_level == spdlog::level::warn) {
            message_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
          } else {
            message_color = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
          }

          CherryGUI::PushTextWrapPos(CherryGUI::GetCursorPosX() +
                                     CherryGUI::GetColumnWidth());
          CherryGUI::TextColored(message_color, log->m_message.c_str());
          CherryGUI::PopTextWrapPos();
        }
      }

      if (is_hovered) {
        CherryGUI::PopStyleColor();
      }
    }

    CherryGUI::EndTable();
  }

  CherryGUI::PopStyleVar(2);
  CherryGUI::PopStyleColor();
  std::cout << "Render END" << std::endl;
}

void LogsUtilityAppWindow::OnImGuiRender() {}

void LogsUtilityAppWindow::menubar() {
  //
}

} // namespace VortexEditor