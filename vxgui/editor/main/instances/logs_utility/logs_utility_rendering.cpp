//
//  logs_utility_rendering.cpp
//  Sources of the rendering methods of the logs utility.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "logs_utility.hpp"

namespace vxe {

  void LogsUtility::render_filters_bar() {
    const float header_width = filterbar_width_ - 46.0f;

    CherryStyle::RemoveMarginX(6.0f);
    if (use_warning_filter_) {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FF9F31"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
    }
    CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_warning.png")),
            header_width,
            "Warnings",
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      use_warning_filter_ = !use_warning_filter_;
    }
    CherryGUI::PopStyleColor(4);

    CherryStyle::RemoveMarginX(6.0f);
    if (use_error_filter_) {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FF3831"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
    }
    CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_error.png")),
            header_width,
            "Errors",
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      use_error_filter_ = !use_error_filter_;
    }
    CherryGUI::PopStyleColor(4);

    CherryStyle::RemoveMarginX(6.0f);
    if (use_fatal_filter_) {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#FF316B"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
    }
    CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_fatal.png")),
            header_width,
            "Fatals",
            ImVec2(-FLT_MIN, 0.0f),
            ImVec2(0, 0),
            ImVec2(1, 1),
            -1,
            ImVec4(0, 0, 0, 0),
            ImVec4(1, 1, 1, 1))) {
      use_fatal_filter_ = !use_fatal_filter_;
    }
    CherryGUI::PopStyleColor(4);

    CherryKit::Separator();

    for (auto &[topic, isActive] : topics_filter_states_) {
      CherryStyle::RemoveMarginX(6.0f);

      if (isActive) {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#BBBBBB"));
      } else {
        CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
      }

      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, Cherry::HexToRGBA("#343434"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, Cherry::HexToRGBA("#454545"));

      if (CherryGUI::ImageSizeButtonWithText(
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png")),
              header_width,
              topic.c_str(),
              ImVec2(-FLT_MIN, 0.0f),
              ImVec2(0, 0),
              ImVec2(1, 1),
              -1,
              ImVec4(0, 0, 0, 0),
              ImVec4(1, 1, 1, 1))) {
        isActive = !isActive;
      }

      CherryGUI::PopStyleColor(4);
    }
  }

  void LogsUtility::render() {
    const float splitterWidth = 1.5f;
    const float margin = 10.0f;

    auto &children = childs_;
    ImVec2 availableSize = CherryGUI::GetContentRegionAvail();

    for (size_t i = 0; i < children.size(); ++i) {
      auto &child = children[i];

      if (child.m_Name == "render_filters_bar") {
        child.m_Disabled = !show_filter_pannel_;
      }
    }

    if (show_filter_pannel_ != previous_filter_pannel_state_) {
      childs_initialized_ = false;
    }

    if (!childs_initialized_) {
      float totalAvailableWidth = availableSize.x - (children.size() - 1) * splitterWidth;
      int visibleChildrenCount = 0;

      for (auto &child : children) {
        if (!child.m_Disabled)
          visibleChildrenCount++;
      }

      float defaultSize = visibleChildrenCount > 0 ? totalAvailableWidth / visibleChildrenCount : 0.0f;

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

      childs_initialized_ = true;
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

      if (child.m_Name == "render_filters_bar") {
        filterbar_width_ = child.m_Size;
      }

      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, child.m_BackgroundColor);
      CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

      std::string childname = child.m_Name + "##cbchildnh" + app_window_->m_Name + child.m_Name;
      CherryGUI::BeginChild(childname.c_str(), ImVec2(child.m_Size, availableSize.y), true);

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
        vxe::LogUtilityChild &next_child = children[nextChildIndex];

        if (i + 1 < children.size() && !children[i].m_Disabled && !next_child.m_Disabled) {
          CherryGUI::SameLine();

          std::string lab = child.m_Name + app_window_->m_Name + "####" + child.m_Name;

          CherryStyle::RemoveMarginX(5.0f);

          CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
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

    previous_filter_pannel_state_ = show_filter_pannel_;
  }

  void LogsUtility::render_content_bar() {
    if (search_bar_) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      if (CherryKit::ButtonImageText("Filters", Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"))
              .GetDataAs<bool>("isClicked")) {
        will_show_filter_pannel_ = !will_show_filter_pannel_;
      }

      CherryGUI::SameLine();

      CherryNextComponent.SetProperty("size_x", "240");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("description", "Search log...");
      CherryNextComponent.SetProperty(
          "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
      CherryNextComponent.SetProperty("description_logo_place", "r");
      CherryKit::InputString("", &project_search_);

      CherryKit::Separator();
    }
    if (show_mode_ == ShowMode::Advanced) {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));

      static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                                     ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                     ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame;

      const float TEXT_BASE_WIDTH = CherryGUI::CalcTextSize("A").x;

      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

      if (console_font_) {
        Cherry::PushFont("JetBrainsMono");
        CherryStyle::PushFontSize(0.50f);
      }
      if (CherryGUI::BeginTable("LogsTable", 4, flags)) {
        CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 6.0f));

        CherryGUI::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 6.0f);
        CherryGUI::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 9.0f);
        CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
        CherryGUI::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);

        CherryGUI::TableHeadersRow();

        CherryGUI::PopStyleVar();
        auto oldFilterStates = topics_filter_states_;
        topics_filter_states_.clear();

        for (auto log : vxe::get_current_context()->registered_logs) {
          const std::string &topic = log->filter;

          if (oldFilterStates.find(topic) != oldFilterStates.end()) {
            topics_filter_states_[topic] = oldFilterStates[topic];
          } else {
            topics_filter_states_[topic] = false;
          }
        }

        bool hasActiveFilter = std::any_of(
            topics_filter_states_.begin(), topics_filter_states_.end(), [](const auto &pair) { return pair.second; });

        for (auto log : vxe::get_current_context()->registered_logs) {
          if (oldFilterStates.find(log->filter) != oldFilterStates.end()) {
            topics_filter_states_[log->filter] = oldFilterStates[log->filter];
          } else {
            topics_filter_states_[log->filter] = false;
          }

          if (use_warning_filter_ || use_error_filter_ || use_fatal_filter_ || use_info_filter_) {
            if ((log->level == spdlog::level::critical && !use_fatal_filter_) ||
                (log->level == spdlog::level::err && !use_error_filter_) ||
                (log->level == spdlog::level::warn && !use_warning_filter_) ||
                (log->level == spdlog::level::info && !use_info_filter_)) {
              continue;
            }
          }

          if (hasActiveFilter && !topics_filter_states_[log->filter]) {
            continue;
          }

          if (!project_search_.empty()) {
            if (!has_common_subsequence(log->message, project_search_)) {
              continue;
            }
          }

          CherryGUI::TableNextRow();

          bool is_hovered = CherryGUI::IsItemHovered();
          if (is_hovered) {
            CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
          }

          if (CherryGUI::IsMouseClicked(0) && is_hovered) {
            std::string content_to_copy = log->timestamp + " | " + log->filter + " | " + log->message;
            CherryGUI::SetClipboardText(content_to_copy.c_str());
          }

          for (int i = 0; i <= 3; i++) {
            CherryGUI::TableSetColumnIndex(i);
            if (i == 0) {
              ImVec4 color;
              switch (log->level)  // Add "User input" in blue
              {
                case spdlog::level::critical: color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
                case spdlog::level::err: color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f); break;
                case spdlog::level::warn: color = ImVec4(0.8f, 0.8f, 0.0f, 1.0f); break;
                case spdlog::level::info: color = ImVec4(0.0f, 1.0f, 0.0f, 0.0f); break;
                default: color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break;
              }

              ImVec2 pos = CherryGUI::GetCursorScreenPos();
              float width = CherryGUI::GetFontSize() * 0.4f;
              float height = CherryGUI::GetFontSize() * 1.2f;
              float rounding = 3.0f;
              ImVec2 rect_min = pos;
              ImVec2 rect_max = ImVec2(pos.x + width, pos.y + height);

              CherryGUI::GetWindowDrawList()->AddRectFilled(rect_min, rect_max, CherryGUI::GetColorU32(color), rounding);

              switch (log->level) {
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
                default: CherryGUI::Text(""); break;
              }
            } else if (i == 1) {
              float column_width = CherryGUI::GetColumnWidth();
              float text_width = CherryGUI::CalcTextSize(log->filter.c_str()).x;
              CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->filter.c_str());
            } else if (i == 2) {
              float column_width = CherryGUI::GetColumnWidth();
              float text_width = CherryGUI::CalcTextSize(log->timestamp.c_str()).x;
              CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->timestamp.c_str());
            } else if (i == 3) {
              ImVec4 message_color;
              if (log->level == spdlog::level::critical) {
                message_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
              } else if (log->level == spdlog::level::err) {
                message_color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
              } else if (log->level == spdlog::level::warn) {
                message_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
              } else {
                message_color = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
              }

              CherryGUI::PushTextWrapPos(CherryGUI::GetCursorPosX() + CherryGUI::GetColumnWidth());
              CherryGUI::TextColored(message_color, log->message.c_str());
              CherryGUI::PopTextWrapPos();
            }
          }

          if (is_hovered) {
            CherryGUI::PopStyleColor();
          }
        }

        CherryGUI::EndTable();
      }

      if (console_font_) {
        Cherry::PopFont();
        CherryStyle::PopFontSize();
      }
      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor();
    } else if (show_mode_ == ShowMode::Simple) {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));

      static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                                     ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame;

      const float TEXT_BASE_WIDTH = CherryGUI::CalcTextSize("A").x;

      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

      if (console_font_) {
        Cherry::PushFont("JetBrainsMono");
        CherryStyle::PushFontSize(0.50f);
      }
      if (CherryGUI::BeginTable("LogsTableSimple", 2, flags)) {
        CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 6.0f));

        CherryGUI::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
        CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);

        CherryGUI::PopStyleVar();
        auto oldFilterStates = topics_filter_states_;
        topics_filter_states_.clear();

        for (auto log : vxe::get_current_context()->registered_logs) {
          const std::string &topic = log->filter;

          if (oldFilterStates.find(topic) != oldFilterStates.end()) {
            topics_filter_states_[topic] = oldFilterStates[topic];
          } else {
            topics_filter_states_[topic] = false;
          }
        }

        bool hasActiveFilter = std::any_of(
            topics_filter_states_.begin(), topics_filter_states_.end(), [](const auto &pair) { return pair.second; });

        for (auto log : vxe::get_current_context()->registered_logs) {
          if (oldFilterStates.find(log->filter) != oldFilterStates.end()) {
            topics_filter_states_[log->filter] = oldFilterStates[log->filter];
          } else {
            topics_filter_states_[log->filter] = false;
          }

          if (use_warning_filter_ || use_error_filter_ || use_fatal_filter_ || use_info_filter_) {
            if ((log->level == spdlog::level::critical && !use_fatal_filter_) ||
                (log->level == spdlog::level::err && !use_error_filter_) ||
                (log->level == spdlog::level::warn && !use_warning_filter_) ||
                (log->level == spdlog::level::info && !use_info_filter_)) {
              continue;
            }
          }

          if (hasActiveFilter && !topics_filter_states_[log->filter]) {
            continue;
          }

          if (!project_search_.empty()) {
            if (!has_common_subsequence(log->message, project_search_)) {
              continue;
            }
          }

          CherryGUI::TableNextRow();

          bool is_hovered = CherryGUI::IsItemHovered();
          if (is_hovered) {
            CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
          }

          if (CherryGUI::IsMouseClicked(0) && is_hovered) {
            std::string content_to_copy = log->timestamp + " | " + log->filter + " | " + log->message;
            CherryGUI::SetClipboardText(content_to_copy.c_str());
          }

          for (int i = 0; i <= 1; i++) {
            CherryGUI::TableSetColumnIndex(i);
            if (i == 0) {
              ImVec4 message_color;
              if (log->level == spdlog::level::critical) {
                message_color = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
              } else if (log->level == spdlog::level::err) {
                message_color = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
              } else if (log->level == spdlog::level::warn) {
                message_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
              } else {
                message_color = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
              }

              CherryGUI::PushTextWrapPos(CherryGUI::GetCursorPosX() + CherryGUI::GetColumnWidth());
              CherryGUI::TextColored(message_color, log->message.c_str());
              CherryGUI::PopTextWrapPos();
            }
            if (i == 1) {
              float column_width = CherryGUI::GetColumnWidth();
              float text_width = CherryGUI::CalcTextSize(log->timestamp.c_str()).x;
              CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), log->timestamp.c_str());
            }
          }

          if (is_hovered) {
            CherryGUI::PopStyleColor();
          }
        }

        CherryGUI::EndTable();
      }

      if (console_font_) {
        Cherry::PopFont();
        CherryStyle::PopFontSize();
      }
      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor();
    } else if (show_mode_ == ShowMode::Block) {
      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

      if (CherryGUI::BeginChild("LogTextBlock", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
        auto oldFilterStates = topics_filter_states_;
        topics_filter_states_.clear();

        for (auto log : vxe::get_current_context()->registered_logs) {
          const std::string &topic = log->filter;

          if (oldFilterStates.find(topic) != oldFilterStates.end()) {
            topics_filter_states_[topic] = oldFilterStates[topic];
          } else {
            topics_filter_states_[topic] = false;
          }
        }

        bool hasActiveFilter = std::any_of(
            topics_filter_states_.begin(), topics_filter_states_.end(), [](const auto &pair) { return pair.second; });

        std::string logText;
        for (auto log : vxe::get_current_context()->registered_logs) {
          std::string line = log->timestamp + " | " + log->filter + " | " + log->message + "\n";
          logText += line;
        }

        static std::vector<char> buffer;
        buffer.assign(logText.begin(), logText.end());
        buffer.push_back('\0');

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AllowTabInput;

        if (console_font_) {
          Cherry::PushFont("JetBrainsMono");
          CherryStyle::PushFontSize(0.50f);
        }
        CherryGUI::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.3f));
        CherryGUI::InputTextMultiline("##FullLogText", buffer.data(), buffer.size(), ImVec2(-FLT_MIN, -FLT_MIN), flags);
        CherryGUI::PopStyleColor();
        if (console_font_) {
          Cherry::PopFont();
          CherryStyle::PopFontSize();
        }
      }
      CherryGUI::EndChild();

      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor();
    }
  }

  void LogsUtility::render_left_menubar() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText("Search", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"))
            .GetDataAs<bool>("isClicked")) {
      search_bar_ = !search_bar_;
    }

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText("Filters", Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"))
            .GetDataAs<bool>("isClicked")) {
      will_show_filter_pannel_ = !will_show_filter_pannel_;
    }
  }

  void LogsUtility::render_left_bottombar() {
    CherryStyle::AddMarginX(8.0f);
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 6.5f);

    auto callback = [](ImGuiInputTextCallbackData *data) -> int {
      LogsUtility *self = static_cast<LogsUtility *>(data->UserData);
      std::vector<CommandDef *> internal_matches;
      std::string input(data->Buf);
      for (auto &def : COMMAND_DEFINITIONS) {
        if (!input.empty() && def.name.find(input) == 0)
          internal_matches.push_back(&def);
      }

      switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion: {
          if (!internal_matches.empty()) {
            int idx = (self->selection_idx_ >= 0 && self->selection_idx_ < (int)internal_matches.size())
                          ? self->selection_idx_
                          : 0;

            std::string snippet = internal_matches[idx]->snippet;
            int cursor_pos = -1;

            auto tag = snippet.find("<cursor>");
            if (tag != std::string::npos) {
              cursor_pos = (int)tag;
              snippet.erase(tag, 8);
            }

            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, snippet.c_str());

            if (cursor_pos >= 0)
              data->CursorPos = cursor_pos;

            self->selection_idx_ = -1;
          }
          break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
          bool navigating_history = input.empty() || self->history_pos_ != -1;
          if (navigating_history) {
            int prev_pos = self->history_pos_;
            if (data->EventKey == ImGuiKey_UpArrow) {
              if (self->history_pos_ == -1)
                self->history_pos_ = (int)self->command_history_.size() - 1;
              else if (self->history_pos_ > 0)
                self->history_pos_--;
            } else if (data->EventKey == ImGuiKey_DownArrow) {
              if (self->history_pos_ != -1) {
                if (++self->history_pos_ >= (int)self->command_history_.size())
                  self->history_pos_ = -1;
              }
            }
            if (prev_pos != self->history_pos_) {
              data->DeleteChars(0, data->BufTextLen);
              data->InsertChars(0, self->history_pos_ >= 0 ? self->command_history_[self->history_pos_].c_str() : "");
            }
          } else if (!internal_matches.empty()) {
            if (data->EventKey == ImGuiKey_UpArrow) {
              if (--self->selection_idx_ < 0)
                self->selection_idx_ = (int)internal_matches.size() - 1;
            } else if (data->EventKey == ImGuiKey_DownArrow) {
              if (++self->selection_idx_ >= (int)internal_matches.size())
                self->selection_idx_ = 0;
            }
          }
          break;
        }
      }
      return 0;
    };
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 4.0f);
    ImGuiInputTextFlags input_flags =
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

    std::string current_input(input_buffer_);
    std::vector<CommandDef *> matches;
    if (!current_input.empty() && history_pos_ == -1) {
      for (auto &def : COMMAND_DEFINITIONS) {
        if (def.name.find(current_input) == 0) {
          matches.push_back(&def);
          if (matches.size() >= 12)
            break;
        }
      }
    }

    if (!matches.empty()) {
      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      float line_h = CherryGUI::GetTextLineHeightWithSpacing();
      CherryGUI::SetNextWindowPos(ImVec2(pos.x, pos.y - (line_h * (float)matches.size()) - 20));
      CherryGUI::SetNextWindowSize(ImVec2(CherryGUI::GetContentRegionAvail().x - 60, 0));

      CherryGUI::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.07f, 0.07f, 0.07f, 0.98f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));

      if (CherryGUI::Begin(
              "##Suggestions",
              nullptr,
              ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                  ImGuiWindowFlags_NoFocusOnAppearing)) {
        for (int i = 0; i < (int)matches.size(); i++) {
          bool selected = (i == selection_idx_);
          if (selected) {
            CherryGUI::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), ">");
          } else {
            CherryGUI::Dummy(ImVec2(CherryGUI::GetTextLineHeight(), 1));
          }
          CherryGUI::SameLine();
          CherryGUI::TextColored(ImVec4(0.4f, 0.75f, 1.0f, 1.0f), "%s", matches[i]->name.c_str());
          CherryGUI::SameLine();
          CherryGUI::TextDisabled("(");
          CherryGUI::SameLine();
          CherryGUI::TextColored(ImVec4(0.85f, 0.55f, 0.3f, 1.0f), "%s", matches[i]->params.c_str());
          CherryGUI::SameLine();
          CherryGUI::TextDisabled(")");

          if (selected && !matches[i]->description.empty()) {
            CherryGUI::SameLine();
            CherryGUI::TextDisabled(" // %s", matches[i]->description.c_str());
          }
        }
        CherryGUI::End();
      }

      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor();
    }

    ImTextureID logo = Cherry::Application::Get().GetCurrentRenderedWindow()->GetTexture(
        Cherry::GetPath("resources/imgs/icons/misc/icon_cmd.png"));

    float inputWidth = 400.0f;
    float paddingX = 7.0f;
    float paddingY = 7.0f;
    float logoSize = 16.0f;
    float logoSpacing = 4.0f;

    CherryGUI::SetNextItemWidth(inputWidth);
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(paddingX + logoSize + logoSpacing, paddingY));

    ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();
    ImVec2 framePadding = CherryGUI::GetStyle().FramePadding;

    if (CherryGUI::InputTextWithHint(
            "##ConsoleInput",
            "Execute command...",
            input_buffer_,
            IM_ARRAYSIZE(input_buffer_),
            input_flags,
            callback,
            this)) {
      std::string cmd(input_buffer_);
      if (!cmd.empty()) {
        send_command(cmd);
        if (command_history_.empty() || command_history_.back() != cmd) {
          command_history_.push_back(cmd);
          if (command_history_.size() > 100)
            command_history_.erase(command_history_.begin());
        }
        input_buffer_[0] = '\0';
        history_pos_ = -1;
        selection_idx_ = -1;
        scroll_to_bottom_ = true;
      }
      CherryGUI::SetKeyboardFocusHere(-1);
    }

    CherryGUI::PopStyleVar();

    if (logo) {
      float logoY = cursorPos.y + framePadding.y + (CherryGUI::GetFontSize() - logoSize) * 0.5f;
      float logoX = cursorPos.x + framePadding.x - logoSize - logoSpacing;

      ImVec4 tint = CherryGUI::IsItemFocused() ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

      CherryGUI::GetWindowDrawList()->AddImage(
          logo,
          ImVec2(logoX, logoY),
          ImVec2(logoX + logoSize, logoY + logoSize),
          ImVec2(0, 0),
          ImVec2(1, 1),
          CherryGUI::ColorConvertFloat4ToU32(tint));
    }
  }

  void LogsUtility::render_right_menubar() {
    {
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");

      if (CherryKit::ButtonImageTextDropdown("Settings", Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
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
      if (CherryKit::ButtonImageTextDropdown("View", Cherry::GetPath("resources/imgs/icons/misc/icon_eye.png"))
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
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);               // TODO : Props
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);      // TODO : Props
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // TODO : Props
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);        // TODO : Props

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
    if (will_show_filter_pannel_ != show_filter_pannel_) {
      show_filter_pannel_ = will_show_filter_pannel_;
    }
    if (CherryGUI::BeginPopup("ViewMenuPopup")) {
      CherryKit::SeparatorText("Pannels");
      CherryKit::CheckboxText("Show filters pannel", &show_filter_pannel_);
      CherryKit::CheckboxText("Console font", &console_font_);

      int default_index = 0;
      if (show_mode_ == ShowMode::Simple) {
        default_index = 0;
      } else if (show_mode_ == ShowMode::Block) {
        default_index = 1;
      } else if (show_mode_ == ShowMode::Advanced) {
        default_index = 2;
      }

      CherryKit::SeparatorText("View mode");
      switch (CherryNextComponent.SetProperty("size_x", 150.0f);
              CherryKit::ComboImageText(
                  "",
                  { { "Simple", Cherry::GetPath("resources/imgs/icons/misc/icon_eye.png") },
                    { "Block", Cherry::GetPath("resources/imgs/icons/misc/icon_eye.png") },
                    { "Advanced", Cherry::GetPath("resources/imgs/icons/misc/icon_eye.png") } },
                  default_index)
                  .GetPropertyAs<int>("selected")) {
        case 0: {
          show_mode_ = ShowMode::Simple;
          break;
        }
        case 1: {
          show_mode_ = ShowMode::Block;
          break;
        }
        case 2: {
          show_mode_ = ShowMode::Advanced;
          break;
        }
        default: {
          break;
        }
      }

      CherryGUI::EndPopup();
    }
    if (will_show_filter_pannel_ != show_filter_pannel_) {
      will_show_filter_pannel_ = show_filter_pannel_;
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
      if (btn->render("LogicContentManager")) {
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
      // CherryGUI::Checkbox("Show Filter pannel", &show_filter_pannel_);
      // CherryGUI::Checkbox("Show Thumbnail pannel",
      // &m_ShowThumbnailVisualizer);

      CherryGUI::EndPopup();
    }
  }

}  // namespace vxe