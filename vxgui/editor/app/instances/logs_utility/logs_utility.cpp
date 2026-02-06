#include "logs_utility.hpp"

#include <iostream>

static bool ErrorFilter = true;
static bool WarnFilter = true;
static bool FatalFilter = true;
static bool InfoFilter = true;
static std::string ProjectSearch;

static float c_FilterBarWidth = 250.0f;

namespace VortexEditor {
LogsUtilityAppWindow::LogsUtilityAppWindow(const std::string &name) {
  std::cout << "LogsUtilityAppWindow" << std::endl;
  m_AppWindow = std::make_shared<AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png"));
  std::shared_ptr<AppWindow> win = m_AppWindow;
  m_AppWindow->m_Closable = true;
  m_AppWindow->SetCloseCallback(
      [this]() { Cherry::DeleteAppWindow(m_AppWindow); });

  m_AppWindow->SetLeftMenubarCallback([this]() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText(
            "Search",
            GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"))
            .GetDataAs<bool>("isClicked")) {
      m_SearchBar = !m_SearchBar;
    }

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText(
            "Filters", GetPath("resources/imgs/icons/misc/icon_filter.png"))
            .GetDataAs<bool>("isClicked")) {
      m_WillShowFilterPannel = !m_WillShowFilterPannel;
    }
  });

  m_AppWindow->SetRightMenubarCallback([this]() {
    {
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");

      if (CherryKit::ButtonImageTextDropdown(
              "Settings",
              GetPath("resources/imgs/icons/misc/icon_settings.png"))
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

      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      CherryNextComponent.SetProperty("disable_callback", "true");
      if (CherryKit::ButtonImageTextDropdown(
              "View", GetPath("resources/imgs/icons/misc/icon_eye.png"))
              .GetDataAs<bool>("isClicked")) {
        ImVec2 mousePos = CherryGUI::GetMousePos();
        ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
        ImVec2 popupSize(350, 100);

        if (mousePos.x + popupSize.x > displaySize.x) {
          mousePos.x -= popupSize.x;
        }
        if (mousePos.y + popupSize.y > displaySize.y) {
          mousePos.y -= popupSize.y;
        }

        CherryGUI::SetNextWindowSize(popupSize, ImGuiCond_Appearing);
        CherryGUI::SetNextWindowPos(mousePos, ImGuiCond_Appearing);
        CherryGUI::OpenPopup("ViewMenuPopup");
      }
    }
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);          // TODO : Props
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f); // TODO : Props
    ImVec4 darkBackgroundColor =
        ImVec4(0.15f, 0.15f, 0.15f, 1.0f);                    // TODO : Props
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // TODO : Props

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
    if (m_WillShowFilterPannel != m_ShowFilterPannel) {
      m_ShowFilterPannel = m_WillShowFilterPannel;
    }
    if (CherryGUI::BeginPopup("ViewMenuPopup")) {
      CherryKit::SeparatorText("Pannels");
      CherryKit::CheckboxText("Show Filter pannel", &m_ShowFilterPannel);

      CherryKit::SeparatorText("View mode");
      switch (
          CherryKit::ComboImageText(
              "Display",
              {{"Simple", GetPath("resources/imgs/icons/misc/icon_eye.png")},
               {"Block", GetPath("resources/imgs/icons/misc/icon_eye.png")},
               {"Advanced", GetPath("resources/imgs/icons/misc/icon_eye.png")}},
              0)
              .GetPropertyAs<int>("selected")) {
      case 0: {
        m_ShowMode = ShowMode::Simple;
        break;
      }
      case 1: {
        m_ShowMode = ShowMode::Block;
        break;
      }
      case 2: {
        m_ShowMode = ShowMode::Advanced;
        break;
      }
      default: {
        break;
      }
      }

      CherryGUI::EndPopup();
    }
    if (m_WillShowFilterPannel != m_ShowFilterPannel) {
      m_WillShowFilterPannel = m_ShowFilterPannel;
    }

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);
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
    CherryStyle::AddMarginX(8.0f);
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 6.5f);

    CherryNextComponent.SetProperty("size_x", "350");
    CherryNextComponent.SetProperty("enter_return", "true");
    CherryNextComponent.SetProperty("description", "Enter commands here...");
    CherryNextComponent.SetProperty(
        "description_logo", GetPath("resources/imgs/icons/misc/icon_cmd.png"));
    CherryNextComponent.SetProperty("description_logo_place", "l");
    auto prompt =
        CherryKit::InputString(CherryID("CommandPrompt"), "", &m_CmdInputValue);

    if (prompt.GetDataAs<bool>("submitted")) {
      VortexMaker::Script::GetScriptingEngine().Execute(m_CmdInputValue);
      s_CommandHistory.push_back(m_CmdInputValue);
      s_HistoryPos = -1;
      s_SelectionIdx = -1;
      s_ScrollToBottom = true;
      m_CmdInputValue.clear();
    }
  });

  this->ctx = VortexMaker::GetCurrentContext();

  LogUtilityChild filterbar("RenderFiltersBar",
                            [this]() { RenderFiltersBar(); });
  filterbar.Disable();
  filterbar.m_DefaultSize = c_FilterBarWidth;
  filterbar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");
  AddChild(filterbar);

  LogUtilityChild contentbar("RenderContentBar",
                             [this]() { RenderContentBar(); });
  contentbar.Enable();
  contentbar.m_DefaultSize = 0.0;
  AddChild(LogUtilityChild(contentbar));
}

void LogsUtilityAppWindow::AddChild(const LogUtilityChild &child) {
  m_Childs.push_back(child);
}

void LogsUtilityAppWindow::RenderFiltersBar() {
  const float header_width = c_FilterBarWidth - 46.0f;

  CherryStyle::RemoveMarginX(6.0f);
  if (m_UseWarningFilter) {
    CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#FF9F31"));
  } else {
    CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#343434"));
  }
  CherryGUI::PushStyleColor(ImGuiCol_Button, HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, HexToRGBA("#454545"));
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(
              GetPath("resources/imgs/icons/misc/icon_warning.png")),
          header_width, "Warnings", ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
          ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
    m_UseWarningFilter = !m_UseWarningFilter;
  }
  CherryGUI::PopStyleColor(4);

  CherryStyle::RemoveMarginX(6.0f);
  if (m_UseErrorFilter) {
    CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#FF3831"));
  } else {
    CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#343434"));
  }
  CherryGUI::PushStyleColor(ImGuiCol_Button, HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, HexToRGBA("#454545"));
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(
              GetPath("resources/imgs/icons/misc/icon_error.png")),
          header_width, "Errors", ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
          ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
    m_UseErrorFilter = !m_UseErrorFilter;
  }
  CherryGUI::PopStyleColor(4);

  CherryStyle::RemoveMarginX(6.0f);
  if (m_UseFatalFilter) {
    CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#FF316B"));
  } else {
    CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#343434"));
  }
  CherryGUI::PushStyleColor(ImGuiCol_Button, HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, HexToRGBA("#454545"));
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(
              GetPath("resources/imgs/icons/misc/icon_fatal.png")),
          header_width, "Fatals", ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
          ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
    m_UseFatalFilter = !m_UseFatalFilter;
  }
  CherryGUI::PopStyleColor(4);

  CherryKit::Separator();

  for (auto &[topic, isActive] : m_TopicsFilterStates) {
    CherryStyle::RemoveMarginX(6.0f);

    if (isActive) {
      CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#BBBBBB"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Border, HexToRGBA("#343434"));
    }

    CherryGUI::PushStyleColor(ImGuiCol_Button, HexToRGBA("#232323"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, HexToRGBA("#454545"));

    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(
                GetPath("resources/imgs/icons/misc/icon_journal.png")),
            header_width, topic.c_str(), ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
            ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
      isActive = !isActive;
    }

    CherryGUI::PopStyleColor(4);
  }
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
  const float splitterWidth = 1.5f;
  const float margin = 10.0f;

  auto &children = m_Childs;
  ImVec2 availableSize = CherryGUI::GetContentRegionAvail();

  for (size_t i = 0; i < children.size(); ++i) {
    auto &child = children[i];

    if (child.m_Name == "RenderFiltersBar") {
      child.m_Disabled = !m_ShowFilterPannel;
    }
  }

  if (m_ShowFilterPannel != m_PreviousFilterPannelState) {
    m_ChildSizesInitialized = false;
  }

  if (!m_ChildSizesInitialized) {
    float totalAvailableWidth =
        availableSize.x - (children.size() - 1) * splitterWidth;
    int visibleChildrenCount = 0;

    for (auto &child : children) {
      if (!child.m_Disabled)
        visibleChildrenCount++;
    }

    float defaultSize = visibleChildrenCount > 0
                            ? totalAvailableWidth / visibleChildrenCount
                            : 0.0f;

    for (auto &child : children) {
      if (!child.m_Disabled) {
        if (child.m_DefaultSize == 0.0f) {
          float total_child_size = 0.0f;

          for (auto child : children) {
            if (!child.m_Disabled) {
              total_child_size += child.m_DefaultSize;
            }
          }

          child.m_Size = totalAvailableWidth - total_child_size;
        } else {
          child.m_Size = child.m_DefaultSize;
        }
      } else {
        child.m_Size = 0.0f;
      }
    }

    m_ChildSizesInitialized = true;
  }

  float totalChildSize = 0.0f;
  int visibleChildrenCount = 0;

  for (auto &child : children) {
    if (!child.m_Disabled) {
      totalChildSize += child.m_Size;
      visibleChildrenCount++;
    }
  }

  totalChildSize += (visibleChildrenCount - 1) * splitterWidth;

  if (totalChildSize > availableSize.x) {
    float scaleFactor = availableSize.x / totalChildSize;

    for (auto &child : children) {
      if (!child.m_Disabled) {
        child.m_Size = (std::max)(child.m_Size * scaleFactor, 50.0f);
      }
    }
  }

  for (size_t i = 0; i < children.size(); ++i) {
    auto &child = children[i];

    if (child.m_Disabled) {
      continue;
    }

    if (child.m_Name == "RenderFiltersBar") {
      c_FilterBarWidth = child.m_Size;
    }

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, child.m_BackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    std::string childname =
        child.m_Name + "##cbchildnh" + m_AppWindow->m_Name + child.m_Name;
    CherryGUI::BeginChild(childname.c_str(),
                          ImVec2(child.m_Size, availableSize.y), true);

    child.m_Child();

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);

    int nextChildIndex = -1;
    for (size_t j = i + 1; j < children.size(); ++j) {
      if (!children[j].m_Disabled) {
        nextChildIndex = j;
        break;
      }
    }

    if (nextChildIndex != -1) {
      VortexEditor::LogUtilityChild &next_child = children[nextChildIndex];

      if (i + 1 < children.size() && !children[i].m_Disabled &&
          !next_child.m_Disabled) {
        CherryGUI::SameLine();

        std::string lab =
            child.m_Name + m_AppWindow->m_Name + "####" + child.m_Name;

        CherryStyle::RemoveMarginX(5.0f);

        CherryGUI::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        CherryGUI::Button(lab.c_str(), ImVec2(splitterWidth, -1));
        CherryGUI::PopStyleColor();

        CherryStyle::RemoveMarginX(5.0f);

        if (CherryGUI::IsItemHovered()) {
          CherryGUI::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (CherryGUI::IsItemActive()) {
          float delta = CherryGUI::GetIO().MouseDelta.x;

          if (child.m_Size >= 50.0f || child.m_Size == 0.0f) {
            if (next_child.m_Size >= 50.0f || next_child.m_Size == 0.0f) {
              child.m_Size += delta;
              next_child.m_Size -= delta;
            }
          }

          child.m_Size = (std::max)(child.m_Size, 50.0f);
          next_child.m_Size = (std::max)(next_child.m_Size, 50.0f);
        }

        CherryGUI::SameLine();
      }
    }
  }

  m_PreviousFilterPannelState = m_ShowFilterPannel;
}

void LogsUtilityAppWindow::RenderContentBar() {
  if (m_SearchBar) {

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText(
            "Filters", GetPath("resources/imgs/icons/misc/icon_filter.png"))
            .GetDataAs<bool>("isClicked")) {
      m_WillShowFilterPannel = !m_WillShowFilterPannel;
    }

    CherryGUI::SameLine();

    CherryNextComponent.SetProperty("size_x", "240");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("description", "Search content...");
    CherryNextComponent.SetProperty(
        "description_logo",
        GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
    CherryNextComponent.SetProperty("description_logo_place", "r");
    CherryKit::InputString("", &ProjectSearch);

    CherryKit::Separator();
  }
  if (m_ShowMode == ShowMode::Advanced) {
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
      CherryGUI::TableSetupColumn("Message",
                                  ImGuiTableColumnFlags_WidthStretch);

      CherryGUI::TableHeadersRow();

      CherryGUI::PopStyleVar();
      auto oldFilterStates = m_TopicsFilterStates;
      m_TopicsFilterStates.clear();

      for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
        const std::string &topic = log->m_filter;

        if (oldFilterStates.find(topic) != oldFilterStates.end()) {
          m_TopicsFilterStates[topic] = oldFilterStates[topic];
        } else {
          m_TopicsFilterStates[topic] = false;
        }
      }

      bool hasActiveFilter =
          std::any_of(m_TopicsFilterStates.begin(), m_TopicsFilterStates.end(),
                      [](const auto &pair) { return pair.second; });

      for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
        if (oldFilterStates.find(log->m_filter) != oldFilterStates.end()) {
          m_TopicsFilterStates[log->m_filter] = oldFilterStates[log->m_filter];
        } else {
          m_TopicsFilterStates[log->m_filter] = false;
        }

        if (m_UseWarningFilter || m_UseErrorFilter || m_UseFatalFilter ||
            m_UseInfoFilter) {
          if ((log->m_level == spdlog::level::critical && !m_UseFatalFilter) ||
              (log->m_level == spdlog::level::err && !m_UseErrorFilter) ||
              (log->m_level == spdlog::level::warn && !m_UseWarningFilter) ||
              (log->m_level == spdlog::level::info && !m_UseInfoFilter)) {
            continue;
          }
        }

        if (hasActiveFilter && !m_TopicsFilterStates[log->m_filter]) {
          continue;
        }

        CherryGUI::TableNextRow();

        bool is_hovered = CherryGUI::IsItemHovered();
        if (is_hovered) {
          CherryGUI::PushStyleColor(ImGuiCol_Text,
                                    ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        if (CherryGUI::IsMouseClicked(0) && is_hovered) {
          std::string content_to_copy =
              log->m_timestamp + " | " + log->m_filter + " | " + log->m_message;
          CherryGUI::SetClipboardText(content_to_copy.c_str());
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
  } else if (m_ShowMode == ShowMode::Simple) {
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame;

    const float TEXT_BASE_WIDTH = CherryGUI::CalcTextSize("A").x;

    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    if (CherryGUI::BeginTable("LogsTableSimple", 2, flags)) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 6.0f));

      CherryGUI::TableSetupColumn("Message",
                                  ImGuiTableColumnFlags_WidthStretch);
      CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed,
                                  TEXT_BASE_WIDTH * 15.0f);

      CherryGUI::PopStyleVar();
      auto oldFilterStates = m_TopicsFilterStates;
      m_TopicsFilterStates.clear();

      for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
        const std::string &topic = log->m_filter;

        if (oldFilterStates.find(topic) != oldFilterStates.end()) {
          m_TopicsFilterStates[topic] = oldFilterStates[topic];
        } else {
          m_TopicsFilterStates[topic] = false;
        }
      }

      bool hasActiveFilter =
          std::any_of(m_TopicsFilterStates.begin(), m_TopicsFilterStates.end(),
                      [](const auto &pair) { return pair.second; });

      for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
        if (oldFilterStates.find(log->m_filter) != oldFilterStates.end()) {
          m_TopicsFilterStates[log->m_filter] = oldFilterStates[log->m_filter];
        } else {
          m_TopicsFilterStates[log->m_filter] = false;
        }

        if (m_UseWarningFilter || m_UseErrorFilter || m_UseFatalFilter ||
            m_UseInfoFilter) {
          if ((log->m_level == spdlog::level::critical && !m_UseFatalFilter) ||
              (log->m_level == spdlog::level::err && !m_UseErrorFilter) ||
              (log->m_level == spdlog::level::warn && !m_UseWarningFilter) ||
              (log->m_level == spdlog::level::info && !m_UseInfoFilter)) {
            continue;
          }
        }

        if (hasActiveFilter && !m_TopicsFilterStates[log->m_filter]) {
          continue;
        }

        CherryGUI::TableNextRow();

        bool is_hovered = CherryGUI::IsItemHovered();
        if (is_hovered) {
          CherryGUI::PushStyleColor(ImGuiCol_Text,
                                    ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        if (CherryGUI::IsMouseClicked(0) && is_hovered) {
          std::string content_to_copy =
              log->m_timestamp + " | " + log->m_filter + " | " + log->m_message;
          CherryGUI::SetClipboardText(content_to_copy.c_str());
        }

        for (int i = 0; i <= 1; i++) {
          CherryGUI::TableSetColumnIndex(i);
          if (i == 0) {
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
          if (i == 1) {
            float column_width = CherryGUI::GetColumnWidth();
            float text_width =
                CherryGUI::CalcTextSize(log->m_timestamp.c_str()).x;
            CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f),
                                   log->m_timestamp.c_str());
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
  } else if (m_ShowMode == ShowMode::Block) {
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    if (CherryGUI::BeginChild("LogTextBlock", ImVec2(0, 0), false,
                              ImGuiWindowFlags_HorizontalScrollbar)) {

      auto oldFilterStates = m_TopicsFilterStates;
      m_TopicsFilterStates.clear();

      for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
        const std::string &topic = log->m_filter;

        if (oldFilterStates.find(topic) != oldFilterStates.end()) {
          m_TopicsFilterStates[topic] = oldFilterStates[topic];
        } else {
          m_TopicsFilterStates[topic] = false;
        }
      }

      bool hasActiveFilter =
          std::any_of(m_TopicsFilterStates.begin(), m_TopicsFilterStates.end(),
                      [](const auto &pair) { return pair.second; });

      std::string logText;
      for (auto log : VortexMaker::GetCurrentContext()->registered_logs) {
        std::string line = log->m_timestamp + " | " + log->m_filter + " | " +
                           log->m_message + "\n";
        logText += line;
      }

      static std::vector<char> buffer;
      buffer.assign(logText.begin(), logText.end());
      buffer.push_back('\0');

      ImGuiInputTextFlags flags =
          ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AllowTabInput;

      CherryGUI::PushStyleColor(ImGuiCol_FrameBg,
                                ImVec4(0.1f, 0.1f, 0.1f, 0.3f));
      CherryGUI::InputTextMultiline("##FullLogText", buffer.data(),
                                    buffer.size(), ImVec2(-FLT_MIN, -FLT_MIN),
                                    flags);
      CherryGUI::PopStyleColor();
    }
    CherryGUI::EndChild();

    CherryGUI::PopStyleVar(2);
    CherryGUI::PopStyleColor();
  }
}

void LogsUtilityAppWindow::OnImGuiRender() {}

void LogsUtilityAppWindow::menubar() {
  //
}

} // namespace VortexEditor