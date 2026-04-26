//
//  content_browser_widgets.cpp
//  Widgets of the content browser window and interfaces.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "content_browser.hpp"

namespace vxe {

  void ContentBrowser::draw_path_bar(const std::string &path) {
    std::string homePath = vxe::get_current_context()->projectDataPath.string();
    bool FirstPathPartIsHome = false;
    std::string displayPath = path;

    if (path.rfind(homePath, 0) == 0) {
      FirstPathPartIsHome = true;
      displayPath = path.substr(homePath.length());
      if (!displayPath.empty() && (displayPath[0] == '\\' || displayPath[0] == '/')) {
        displayPath.erase(0, 1);
      }
    }

#ifdef _WIN32
    const std::string separator = "\\";
    const char separator_c = '\\';
#else
    const std::string separator = "/";
    const char separator_c = '/';
#endif

    std::vector<std::string> elements;
    std::stringstream ss(displayPath);
    std::string segment;
    while (std::getline(ss, segment, separator_c)) {
      if (!segment.empty())
        elements.push_back(segment);
    }

    float availableWidth = CherryGUI::GetContentRegionAvail().x;
    float synthStart = 0.5f;
    float maxWidth = (std::max)(availableWidth * synthStart, 100.0f);

    float totalWidth = 0.0f;
    const float sepWidth = CherryGUI::CalcTextSize("/").x;
    const float ellipsisWidth = CherryGUI::CalcTextSize("...").x;

    std::vector<float> widths;
    for (const auto &el : elements)
      widths.push_back(CherryGUI::CalcTextSize(el.c_str()).x);

    for (float w : widths)
      totalWidth += w + sepWidth;

    std::vector<std::string> displayElements;
    std::vector<size_t> elementIndices;

    bool useSynthView = totalWidth > maxWidth && elements.size() > 4;

    if (useSynthView) {
      // displayElements.push_back(elements[0]);
      // elementIndices.push_back(0);

      displayElements.push_back("...");
      elementIndices.push_back(static_cast<size_t>(-1));

      int start = (int)elements.size() - 3;
      if (start < 1)
        start = 1;

      for (int i = start; i < (int)elements.size(); ++i) {
        displayElements.push_back(elements[i]);
        elementIndices.push_back(i);
      }
    } else {
      displayElements = elements;
      for (size_t i = 0; i < elements.size(); ++i)
        elementIndices.push_back(i);
    }

    CherryStyle::AddMarginX(5.0f);
    if (FirstPathPartIsHome) {
      CherryStyle::AddMarginY(8.0f);
      CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png")), ImVec2(15.0f, 15.0f));
      CherryStyle::RemoveMarginY(11.0f);
      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
      CherryGUI::TextUnformatted(separator.c_str());
      CherryGUI::PopStyleColor();
      CherryStyle::AddMarginY(11.0f);
      CherryStyle::RemoveMarginY(8.0f);
    }

    for (size_t i = 0; i < displayElements.size(); ++i) {
      const std::string &el = displayElements[i];
      CherryNextComponent.SetProperty("padding_y", "0.0f");
      CherryNextComponent.SetProperty("padding_x", "0.0f");
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryStyle::AddMarginY(8.0f);

      if (el == "...") {
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        CherryStyle::RemoveMarginY(12.0f);
        CherryGUI::TextUnformatted("...");
        CherryStyle::AddMarginY(12.0f);
        CherryGUI::PopStyleColor();

        if (CherryGUI::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
          std::string hidden;
          for (size_t k = 1; k < elements.size() - 3; ++k) {
            if (k != 1)
              hidden += separator;
            hidden += elements[k];
          }
          CherryGUI::SetTooltip("%s", hidden.c_str());
        }
      } else {
        auto buttonId = this->get_app_window()->m_IdName + el + std::to_string(i);
        if (CherryKit::ButtonText(CherryID(buttonId), el).GetData("isClicked") == "true") {
          std::string return_path = FirstPathPartIsHome ? homePath : "";

          size_t clickedElementIndex = elementIndices[i];
          if (clickedElementIndex != static_cast<size_t>(-1)) {
            for (size_t j = 0; j <= clickedElementIndex; ++j)
              return_path += separator + elements[j];

            change_directory(return_path);
          }
        }

        {
          std::string test_path = FirstPathPartIsHome ? homePath : "";
          size_t clickedElementIndex = elementIndices[i];
          if (clickedElementIndex != static_cast<size_t>(-1)) {
            for (size_t j = 0; j <= clickedElementIndex; ++j)
              test_path += separator + elements[j];
          }

          for (auto &item : item_paths_) {
            if (test_path == item.first) {
              ImVec2 min = CherryGUI::GetItemRectMin();
              ImVec2 max = CherryGUI::GetItemRectMax();

              ImVec2 p1(min.x, max.y + 1.0f);
              ImVec2 p2(max.x, max.y + 1.0f);

              ImU32 color = Cherry::HexToImU32(item.second);

              CherryGUI::GetWindowDrawList()->AddLine(p1, p2, color, 2.0f);
              break;
            }
          }
        }
      }

      CherryStyle::RemoveMarginY(8.0f);
      CherryStyle::RemoveMarginY(3.0f);
      if (i < displayElements.size() - 1) {
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1));
        CherryGUI::TextUnformatted(separator.c_str());
        CherryGUI::PopStyleColor();
      }
      CherryStyle::AddMarginY(3.0f);
    }

    CherryStyle::AddMarginY(2.0f);
  }

  void ContentBrowser::draw_folder_icon(ImVec2 pos, ImVec2 size, ImU32 color) {
    ImDrawList *drawList = CherryGUI::GetWindowDrawList();

    float folderFlapHeight = size.y * 0.2f;
    float flapSlopeWidth = size.x * 0.15f;
    float borderRadius = size.y * 0.1f;

    ImVec2 flapTopLeft = pos;
    ImVec2 flapBottomRight = ImVec2(pos.x + size.x * 0.6f, pos.y + folderFlapHeight);
    ImVec2 flapSlopeEnd = ImVec2(flapBottomRight.x + flapSlopeWidth, flapBottomRight.y);

    ImVec2 bodyTopLeft = ImVec2(pos.x, pos.y + folderFlapHeight);
    ImVec2 bodyBottomRight = ImVec2(pos.x + size.x, pos.y + size.y);

    drawList->AddRectFilled(bodyTopLeft, bodyBottomRight, color);

    ImVec2 centerRectTopLeft = ImVec2(pos.x + size.x * 0.2f, pos.y + 0.2f + size.y * 0.2f + 6.8f);
    ImVec2 centerRectBottomRight = ImVec2(pos.x + size.x * 0.8f, pos.y + size.y * 0.8f - 2.8f);
    drawList->AddRectFilled(centerRectTopLeft, centerRectBottomRight, IM_COL32_WHITE, 0.0f, 0);

    ImVec2 secondRectTopLeft = ImVec2(pos.x + size.x * 0.2f, pos.y + 0.2f + size.y * 0.2f + 11.8f);
    ImVec2 secondRectBottomRight = ImVec2(pos.x + size.x, pos.y + size.y);
    drawList->AddRectFilled(secondRectTopLeft, secondRectBottomRight, color, 0.0f, 0);

    drawList->AddRectFilled(flapTopLeft, flapBottomRight, color, borderRadius, ImDrawFlags_RoundCornersTopLeft);
    drawList->AddTriangleFilled(flapBottomRight, flapSlopeEnd, ImVec2(flapBottomRight.x - 3, flapTopLeft.y), color);
  }

  bool ContentBrowser::draw_item_card(const ItemCardParams &p) {
    bool pressed = false;

    float logoSize = 60.0f;
    float extraHeight = 80.0f;
    float padding = 10.0f;
    float separatorHeight = 2.0f;
    float textOffsetY = 5.0f;
    float versionBoxWidth = 10.0f;
    float versionBoxHeight = 20.0f;
    float thumbnailIconOffsetY = 30.0f;

    float oldfontsize = CherryGUI::GetFont()->Scale;

    if (p.selected) {
      p.style.bg_color = IM_COL32(80, 80, 240, 255);
      p.style.border_color = IM_COL32(150, 150, 255, 255);
    }

    ImVec2 squareSize(logoSize, logoSize);

    const char *originalText = p.content.name.c_str();
    std::string truncatedText = p.content.name;

    if (CherryGUI::CalcTextSize(originalText).x > p.style.max_text_width) {
      truncatedText = p.content.name.substr(0, 20);
      if (CherryGUI::CalcTextSize(truncatedText.c_str()).x > p.style.max_text_width) {
        truncatedText = p.content.name.substr(0, 10) + "\n" + p.content.name.substr(10, 10);
      }
    } else {
      truncatedText = p.content.name + "\n";
    }

    ImVec2 fixedSize(p.style.max_text_width + padding * 2, logoSize + extraHeight + padding * 2);

    ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

    std::string button_id = p.content.name + "squareButtonWithText" + p.content.name;
    if (CherryGUI::InvisibleButton(button_id.c_str(), fixedSize)) {
      pressed = true;
    }

    if (CherryGUI::IsItemHovered()) {
      CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    static bool open_deletion_modal = false;

    static bool delete_single_file = false;
    static std::string delete_single_file_path = "";

    if (open_deletion_modal) {
      CherryGUI::SetNextWindowSize(ImVec2(300, 200));

      static ImGuiTableFlags window_flags = ImGuiWindowFlags_NoResize;
      if (CherryGUI::BeginPopupModal("Delete file(s)", 0, window_flags)) {
        static char path_input_all[512];

        if (delete_single_file) {
          CherryGUI::TextWrapped("WARNING, one file");
        } else {
          CherryGUI::TextWrapped(
              "WARNING, if you click on the Delete button, the "
              "project will be erase forever.");
        }

        CherryGUI::SetItemDefaultFocus();

        if (CherryGUI::Button("Cancel", ImVec2(120, 0))) {
          open_deletion_modal = false;
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::SameLine();
        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
        if (CherryGUI::Button("Delete", ImVec2(120, 0))) {
          if (delete_single_file) {
            if (delete_path_callback_) {
              delete_path_callback_(delete_single_file_path);
            }
            delete_single_file_path = "";
            delete_single_file = false;
          } else {
            for (auto item : selected_) {
              delete_path_callback_(item);
            }
          }

          open_deletion_modal = false;
          CherryGUI::CloseCurrentPopup();
        }
        CherryGUI::PopStyleColor(3);
        CherryGUI::EndPopup();
      }
    }
    if (open_deletion_modal)
      CherryGUI::OpenPopup("Delete file(s)");

    if (CherryGUI::IsItemHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
      selected_.push_back(p.content.path);
    }

    bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
    bool alt = CherryApp.IsKeyPressed(CherryKey::ALT);
    bool shift = CherryApp.IsKeyPressed(CherryKey::SHIFT);

    bool shortcutRename = ctrl && CherryGUI::IsKeyPressed(ImGuiKey_R);
    bool shortcutCut = ctrl && CherryGUI::IsKeyPressed(ImGuiKey_X);
    bool shortcutCutAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::X);
    bool shortcutCopy = ctrl && CherryApp.IsKeyPressed(CherryKey::C);
    bool shortcutCopyAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::C);
    bool shortcutDelete = CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);
    bool shortcutDeleteMulti = alt && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);

    if (shortcutRename && !path_to_rename_.empty()) {
      path_to_rename_ = p.content.path;
    }

    if (shortcutCut && !selected_.empty()) {
      if (cut_paths_callback_) {
        copy_selection_.clear();
        cut_selection_.clear();
        cut_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          cut_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutCutAdd && !selected_.empty()) {
      if (cut_paths_callback_) {
        copy_selection_.clear();
        cut_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          cut_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutCopy && !selected_.empty()) {
      if (copy_paths_callback_) {
        copy_selection_.clear();
        cut_selection_.clear();
        copy_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          copy_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutCopyAdd && !selected_.empty()) {
      if (copy_paths_callback_) {
        cut_selection_.clear();
        copy_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          copy_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutDelete && selected_.size() == 1) {
      if (delete_path_callback_)
        delete_path_callback_(selected_[0]);
      selected_.clear();
    }

    if (shortcutDeleteMulti && selected_.size() > 1) {
      if (delete_path_callback_) {
        for (const auto &path : selected_)
          delete_path_callback_(path);
      }
      selected_.clear();
    }
    item_context_menu_opened_ = false;
    if (CherryGUI::BeginPopupContextItem("ThumbmailsItemContextPopup")) {
      CherryGUI::GetFont()->Scale = 0.9;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

      item_context_menu_opened_ = true;

      FileTypes fileType = detect_file(p.content.path);

      if (item_handles_.empty()) {
        if (p.content.item_ident) {
          item_handles_ = vxe::get_all_item_handlers_for(p.content.item_ident->m_Name);
        } else {
          item_handles_ = vxe::get_all_item_handlers_for(get_file_type_str(fileType));
        }
      }

      if (!item_handles_.empty()) {
        CherryKit::SeparatorText("Actions");
      }

      for (auto ih : item_handles_) {
        if (CherryGUI::MenuItem(ih->title.c_str(), ih->description.c_str(), Cherry::GetTexture(ih->logo), false)) {
          ih->handler(p.content.path);
        }
      }

      CherryKit::SeparatorText("Main");

      CherryGUI::GetFont()->Scale = oldfontsize;
      CherryGUI::PopFont();

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (p.content.item_ident) {
        if (CherryGUI::MenuItem(
                "Open item",
                "",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png")),
                NULL)) {
          change_directory(p.content.path);
          item_paths_.push_back({ p.content.path, p.content.item_ident->m_LineColor });
          CherryGUI::CloseCurrentPopup();
        }
      }

      if (CherryGUI::MenuItem(
              "Copy", "Ctrl + C", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")), NULL)) {
        if (copy_paths_callback_) {
          copy_paths_callback_(selected_, false);
        }

        selected_.clear();
        CherryGUI::CloseCurrentPopup();
      }

      if (selected_.size() > 0) {
        std::string label = "Copy in addition (" + std::to_string(selected_.size()) + " copies)";
        if (CherryGUI::MenuItem(
                label.c_str(),
                "Ctrl + C",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")),
                NULL)) {
          if (copy_paths_callback_) {
            copy_paths_callback_(selected_, true);
          }

          selected_.clear();
          CherryGUI::CloseCurrentPopup();
        }
      }
      if (CherryGUI::MenuItem(
              "Cut", "Ctrl + X", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")), NULL)) {
        change_directory(p.content.path);
        CherryGUI::CloseCurrentPopup();
      }

      CherryKit::SeparatorText("Customize");

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::MenuItem(
              "Change color",
              "Ctrl + X",
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_palette.png")),
              NULL)) {
        //
      }
      if (CherryGUI::MenuItem(
              "Mark as favorite",
              "Ctrl + X",
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_heart.png")),
              NULL)) {
        //
      }

      CherryGUI::EndPopup();
    }

    if (!item_context_menu_opened_) {
      item_handles_.clear();
    }
    /*if (CherryGUI::BeginPopupContextItem("ContextPopup")) {
      CherryKit::SeparatorText("Main");

      if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
        path_to_rename_ = path;
      }

      {
        std::string label =
            "Cut (" + std::to_string(selected_.size()) + ") selected";

        if (CherryGUI::MenuItem(label.c_str(), "Ctrl + X")) {
          if (cut_paths_callback_) {
            copy_selection_.clear();
            cut_selection_.clear();
            cut_paths_callback_(selected_, true);
            for (auto &path : selected_) {
              cut_selection_.push_back(path);
            }
          }

          selected_.clear();

          CherryGUI::CloseCurrentPopup();
        }
      }

      if (cut_selection_.size() > 0) {
        std::string label = "Cut in addition (" +
                            std::to_string(cut_selection_.size()) + " copies)";
        if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
          if (cut_paths_callback_) {
            copy_selection_.clear();
            cut_paths_callback_(selected_, true);

            for (auto &path : selected_) {
              cut_selection_.push_back(path);
            }
          }
          selected_.clear();
          CherryGUI::CloseCurrentPopup();
        }
      }

      {
        std::string label =
            "Copy (" + std::to_string(selected_.size()) + ") selected";

        if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
          if (copy_paths_callback_) {
            copy_selection_.clear();
            cut_selection_.clear();
            copy_paths_callback_(selected_, true);
            for (auto &path : selected_) {
              copy_selection_.push_back(path);
            }
          }

          selected_.clear();

          CherryGUI::CloseCurrentPopup();
        }
      }

      if (copy_selection_.size() > 0) {
        std::string label = "Copy in addition (" +
                            std::to_string(copy_selection_.size()) + " copies)";
        if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
          if (copy_paths_callback_) {
            cut_selection_.clear();
            copy_paths_callback_(selected_, true);

            for (auto &path : selected_) {
              copy_selection_.push_back(path);
            }
          }
          selected_.clear();
          CherryGUI::CloseCurrentPopup();
        }
      }

      if (CherryGUI::MenuItem("Delete", "Suppr")) {
        if (delete_path_callback_) {
          delete_path_callback_(path);
        }
        selected_.clear();
        CherryGUI::CloseCurrentPopup();
      }

      if (selected_.size() > 1) {
        std::string label =
            "Delete (" + std::to_string(selected_.size()) + " selected)";
        if (CherryGUI::MenuItem(label.c_str(), "Alt + Suppr")) {
          if (delete_path_callback_) {
            for (auto &path : selected_) {
              delete_path_callback_(path);
            }
          }
          selected_.clear();

          CherryGUI::CloseCurrentPopup();
        }
      }

      CherryKit::SeparatorText("Customizations");

      CherryGUI::EndPopup();
    }
  */

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);

    ImDrawList *drawList = CherryGUI::GetWindowDrawList();

    drawList->AddRectFilled(
        cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), p.style.bg_color, p.style.border_radius);
    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y),
        IM_COL32(26, 26, 26, 255),
        p.style.border_radius,
        ImDrawFlags_RoundCornersTop);
    drawList->AddRect(
        cursorPos,
        ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
        p.style.border_color,
        p.style.border_radius,
        0,
        1.0f);

    ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + padding);

    ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 20 + textOffsetY);
    CherryGUI::SetCursorScreenPos(sizePos);

    drawList->AddImage(
        Application::GetCurrentRenderedWindow()->get_texture(p.content.logo),
        logoPos,
        ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

    CherryGUI::GetFont()->Scale = 0.7;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(p.style.max_text_width);
    CherryGUI::TextWrapped(p.content.size.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 lineStart = ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
    ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
    drawList->AddLine(lineStart, lineEnd, p.style.line_color, separatorHeight);

    CherryGUI::GetFont()->Scale = 0.9;
    CherryGUI::PushFont(CherryGUI::GetFont());

    ImVec2 textPos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
    CherryGUI::SetCursorScreenPos(textPos);
    CherryGUI::PushItemWidth(p.style.max_text_width);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
    static std::unordered_map<std::string, char[256]> renameBuffers;

    if (path_to_rename_ == p.content.path) {
      CherryGUI::SetItemAllowOverlap();
      CherryGUI::PushID(p.content.path.c_str());

      auto &buffer = renameBuffers[p.content.path];
      if (buffer[0] == '\0') {
        std::string filename = p.content.path.substr(p.content.path.find_last_of("/\\") + 1);
        std::strncpy(buffer, filename.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        CherryGUI::SetKeyboardFocusHere();
      }

      ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
      bool renamed = CherryGUI::InputText("##rename_input", buffer, sizeof(buffer), flags);

      bool renameConfirmed = false;
      bool renameCancelled = false;

      if (renamed) {
        renameConfirmed = true;
      }

      if (!CherryGUI::IsItemActive() && CherryGUI::IsItemDeactivatedAfterEdit()) {
        renameConfirmed = true;
      }

      if (CherryGUI::IsItemFocused() && CherryGUI::IsKeyPressed(ImGuiKey_Escape)) {
        renameCancelled = true;
      }

      if (renameConfirmed) {
        vxe::submit_rename(path_to_rename_, buffer);
        renameBuffers.erase(path_to_rename_);
        path_to_rename_.clear();
      }

      if (renameCancelled) {
        renameBuffers.erase(path_to_rename_);
        path_to_rename_.clear();
      }

      CherryGUI::PopID();
    } else {
      draw_highlighted_text_(
          drawList, textPos, truncatedText.c_str(), project_search_.c_str(), highlightColor, textColor, highlightTextColor);
    }

    CherryGUI::PopItemWidth();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 descriptionPos =
        ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
    CherryGUI::SetCursorScreenPos(descriptionPos);

    CherryGUI::GetFont()->Scale = 0.7;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(p.style.max_text_width);
    CherryGUI::TextWrapped(p.content.description.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();

    CherryGUI::GetFont()->Scale = oldfontsize;
    CherryGUI::PopFont();

    ImVec2 smallRectPos = ImVec2(
        cursorPos.x + fixedSize.x - versionBoxWidth - padding, cursorPos.y + fixedSize.y - versionBoxHeight - padding);
    drawList->AddRectFilled(
        smallRectPos,
        ImVec2(smallRectPos.x + versionBoxWidth, smallRectPos.y + versionBoxHeight),
        IM_COL32(0, 0, 0, 255),
        p.style.border_radius);

    float windowVisibleX2 = CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
    if (cursorPos.x + fixedSize.x < windowVisibleX2)
      CherryGUI::SameLine();

    CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

    return pressed;
  }

  bool ContentBrowser::draw_horizontal_draw_item_card(const ItemCardParams &p) {
    bool pressed = false;

    float logoSize = 40.0f;
    float padding = 10.0f;
    float separatorHeight = 2.0f;
    float versionBoxWidth = 10.0f;
    float versionBoxHeight = 20.0f;

    float oldFontScale = CherryGUI::GetFont()->Scale;
    ImFont *oldFont = CherryGUI::GetFont();

    if (p.selected) {
      p.style.bg_color = IM_COL32(80, 80, 240, 255);
      p.style.border_color = IM_COL32(150, 150, 255, 255);
    }

    ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();
    float contentWidth = CherryGUI::GetContentRegionAvail().x;
    ImVec2 cardSize(contentWidth, logoSize + padding * 2 + 10);

    std::string button_id = "HorizontalCard_" + p.content.path;
    if (CherryGUI::InvisibleButton(button_id.c_str(), cardSize)) {
      pressed = true;
    }

    if (CherryGUI::IsItemHovered()) {
      CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);

      if (CherryGUI::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (p.isFolder) {
          change_directory(p.content.path);
        } else {
          // default file action
        }
      }
    }

    bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
    bool alt = CherryApp.IsKeyPressed(CherryKey::ALT);
    bool shift = CherryApp.IsKeyPressed(CherryKey::SHIFT);

    bool shortcutRename = ctrl && CherryGUI::IsKeyPressed(ImGuiKey_R);
    bool shortcutCut = ctrl && CherryGUI::IsKeyPressed(ImGuiKey_X);
    bool shortcutCutAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::X);
    bool shortcutCopy = ctrl && CherryApp.IsKeyPressed(CherryKey::C);
    bool shortcutCopyAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::C);
    bool shortcutDelete = CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);
    bool shortcutDeleteMulti = alt && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);

    if (shortcutRename && !path_to_rename_.empty()) {
      path_to_rename_ = p.content.path;
    }

    if (shortcutCut && !selected_.empty()) {
      if (cut_paths_callback_) {
        copy_selection_.clear();
        cut_selection_.clear();
        cut_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          cut_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutCutAdd && !selected_.empty()) {
      if (cut_paths_callback_) {
        copy_selection_.clear();
        cut_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          cut_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutCopy && !selected_.empty()) {
      if (copy_paths_callback_) {
        copy_selection_.clear();
        cut_selection_.clear();
        copy_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          copy_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutCopyAdd && !selected_.empty()) {
      if (copy_paths_callback_) {
        cut_selection_.clear();
        copy_paths_callback_(selected_, true);
        for (const auto &path : selected_)
          copy_selection_.push_back(path);
      }
      selected_.clear();
    }

    if (shortcutDelete && selected_.size() == 1) {
      if (delete_path_callback_)
        delete_path_callback_(selected_[0]);
      selected_.clear();
    }

    if (shortcutDeleteMulti && selected_.size() > 1) {
      if (delete_path_callback_) {
        for (const auto &path : selected_)
          delete_path_callback_(path);
      }
      selected_.clear();
    }
    if (CherryGUI::BeginPopupContextItem(("context_" + p.content.path).c_str())) {
      CherryGUI::GetFont()->Scale = 0.9;
      CherryGUI::PushFont(CherryGUI::GetFont());

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

      item_context_menu_opened_ = true;

      FileTypes fileType = detect_file(p.content.path);

      if (item_handles_.empty()) {
        if (p.content.item_ident) {
          item_handles_ = vxe::get_all_item_handlers_for(p.content.item_ident->m_Name);
        } else {
          item_handles_ = vxe::get_all_item_handlers_for(get_file_type_str(fileType));
        }
      }

      if (!item_handles_.empty()) {
        CherryKit::SeparatorText("Actions");
      }

      for (auto ih : item_handles_) {
        if (CherryGUI::MenuItem(ih->title.c_str(), ih->description.c_str(), Cherry::GetTexture(ih->logo), false)) {
          ih->handler(p.content.path);
        }
      }

      CherryKit::SeparatorText("Main");

      CherryGUI::GetFont()->Scale = oldFontScale;
      CherryGUI::PopFont();

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (p.content.item_ident) {
        if (CherryGUI::MenuItem(
                "Open item",
                "",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png")),
                NULL)) {
          change_directory(p.content.path);
          item_paths_.push_back({ p.content.path, p.content.item_ident->m_LineColor });
          CherryGUI::CloseCurrentPopup();
        }
      }

      if (CherryGUI::MenuItem(
              "Copy", "Ctrl + C", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")), NULL)) {
        if (copy_paths_callback_) {
          copy_paths_callback_(selected_, false);
        }

        selected_.clear();
        CherryGUI::CloseCurrentPopup();
      }

      if (selected_.size() > 0) {
        std::string label = "Copy in addition (" + std::to_string(selected_.size()) + " copies)";
        if (CherryGUI::MenuItem(
                label.c_str(),
                "Ctrl + C",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")),
                NULL)) {
          if (copy_paths_callback_) {
            copy_paths_callback_(selected_, true);
          }

          selected_.clear();
          CherryGUI::CloseCurrentPopup();
        }
      }
      if (CherryGUI::MenuItem(
              "Cut", "Ctrl + X", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")), NULL)) {
        change_directory(p.content.path);
        CherryGUI::CloseCurrentPopup();
      }

      CherryKit::SeparatorText("Customize");

      CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

      if (CherryGUI::MenuItem(
              "Change color",
              "Ctrl + X",
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_palette.png")),
              NULL)) {
        //
      }
      if (CherryGUI::MenuItem(
              "Mark as favorite",
              "Ctrl + X",
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_heart.png")),
              NULL)) {
        //
      }

      CherryGUI::EndPopup();
    }

    ImDrawList *drawList = CherryGUI::GetWindowDrawList();

    drawList->AddRectFilled(
        cursorPos, ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y), p.style.bg_color, p.style.border_radius);
    drawList->AddRect(
        cursorPos,
        ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
        p.style.border_color,
        p.style.border_radius,
        0,
        1.0f);

    ImVec2 logoPos(cursorPos.x + padding, cursorPos.y + padding);
    ImVec2 logoEnd(logoPos.x + logoSize, logoPos.y + logoSize);
    static ImTextureID logotexture = Application::GetCurrentRenderedWindow()->get_texture(p.content.logo);
    drawList->AddImage(logotexture, logoPos, logoEnd);

    ImVec2 textPos(logoEnd.x + padding, cursorPos.y + padding);
    float textWidth = contentWidth - logoSize - padding * 3 - versionBoxWidth;

    CherryGUI::SetCursorScreenPos(textPos);
    CherryGUI::GetFont()->Scale = 0.9;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    CherryGUI::PushItemWidth(textWidth);

    static std::unordered_map<std::string, char[256]> renameBuffers;
    if (path_to_rename_ == p.content.path) {
      CherryGUI::PushID(p.content.path.c_str());
      auto &buffer = renameBuffers[p.content.path];
      if (buffer[0] == '\0') {
        std::strncpy(buffer, p.content.name.c_str(), sizeof(buffer));
      }
      if (CherryGUI::InputText("##rename_input", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        vxe::submit_rename(path_to_rename_, buffer);
        renameBuffers.erase(path_to_rename_);
        path_to_rename_.clear();
      }
      CherryGUI::PopID();
    } else {
      CherryGUI::TextWrapped("%s", p.content.name.c_str());
    }

    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();
    CherryGUI::GetFont()->Scale = oldFontScale;
    CherryGUI::PopFont();

    ImVec2 descPos(textPos.x, textPos.y + 20);
    CherryGUI::SetCursorScreenPos(descPos);
    CherryGUI::GetFont()->Scale = 0.7;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
    CherryGUI::PushItemWidth(textWidth);
    CherryGUI::TextWrapped("%s", p.content.description.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();
    CherryGUI::GetFont()->Scale = oldFontScale;
    CherryGUI::PopFont();

    float lineOffset = versionBoxWidth + padding * 1.5f;
    ImVec2 lineStart(cursorPos.x + cardSize.x - lineOffset, cursorPos.y + padding);
    ImVec2 lineEnd(cursorPos.x + cardSize.x - lineOffset, cursorPos.y + cardSize.y - padding);
    drawList->AddLine(lineStart, lineEnd, p.style.line_color, 3.0f);

    ImVec2 versionPos(
        cursorPos.x + cardSize.x - versionBoxWidth - padding, cursorPos.y + cardSize.y - versionBoxHeight - padding);
    drawList->AddRectFilled(
        versionPos,
        ImVec2(versionPos.x + versionBoxWidth, versionPos.y + versionBoxHeight),
        IM_COL32(0, 0, 0, 255),
        p.style.border_radius);

    CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + padding));
    return pressed;
  }

  void ContentBrowser::draw_highlighted_text_(
      ImDrawList *drawList,
      ImVec2 textPos,
      const char *text,
      const char *search,
      ImU32 highlightColor,
      ImU32 textColor,
      ImU32 highlightTextColor) {
    if (!text || !search || !*search) {
      drawList->AddText(textPos, textColor, text);
      return;
    }

    const char *start = text;
    const char *found = strstr(start, search);
    while (found) {
      if (found > start) {
        std::string preText(start, found);
        drawList->AddText(textPos, textColor, preText.c_str());
        textPos.x += CherryGUI::CalcTextSize(preText.c_str()).x;
      }

      ImVec2 highlightPos = textPos;
      ImVec2 highlightSize = CherryGUI::CalcTextSize(search);
      drawList->AddRectFilled(
          highlightPos, ImVec2(highlightPos.x + highlightSize.x, highlightPos.y + highlightSize.y), highlightColor);
      drawList->AddText(textPos, highlightTextColor, search);
      textPos.x += highlightSize.x;

      start = found + strlen(search);
      found = strstr(start, search);
    }

    if (*start) {
      drawList->AddText(textPos, textColor, start);
    }
  }

  void ContentBrowser::draw_folder(const std::string &colorHex, float width, float height, ImVec2 pos) {
    ImDrawList *drawList = CherryGUI::GetWindowDrawList();
    ImU32 color = HexToImU32(colorHex);

    float flapHeight = height * 0.2f;
    float flapWidth = width * 0.4f;
    float borderRadius = height * 0.05f;

    ImVec4 colVec = CherryGUI::ColorConvertU32ToFloat4(color);
    ImU32 flapColor =
        CherryGUI::ColorConvertFloat4ToU32(ImVec4(colVec.x * 0.8f, colVec.y * 0.8f, colVec.z * 0.8f, colVec.w));
    ImU32 shadowColor =
        CherryGUI::ColorConvertFloat4ToU32(ImVec4(colVec.x * 0.7f, colVec.y * 0.7f, colVec.z * 0.7f, colVec.w * 0.2f));

    ImVec2 flapTopLeft = ImVec2(pos.x + width * 0.001f, pos.y - 1.0f);
    ImVec2 flapBottomRight = ImVec2(flapTopLeft.x + flapWidth, pos.y + flapHeight);

    ImVec2 flapBarTopLeft = ImVec2(pos.x, pos.y + flapHeight - height * 0.14f);
    ImVec2 flapBarBottomRight = ImVec2(pos.x + width, pos.y + flapHeight + height * 0.07f);

    ImVec2 bodyTopLeft = ImVec2(pos.x, pos.y + flapHeight);
    ImVec2 bodyBottomRight = ImVec2(pos.x + width, pos.y + height);

    drawList->AddRectFilled(flapTopLeft, flapBottomRight, flapColor, borderRadius, ImDrawFlags_RoundCornersTopLeft);
    drawList->AddRectFilled(flapBarTopLeft, flapBarBottomRight, flapColor, borderRadius);
    drawList->AddRectFilled(bodyTopLeft, bodyBottomRight, color, borderRadius);

    float lineOffset = height * 0.05f;
    for (int i = 1; i <= 3; ++i) {
      float yPos = bodyBottomRight.y - (lineOffset * i);
      drawList->AddLine(
          ImVec2(bodyTopLeft.x + width * 0.01f, yPos), ImVec2(bodyBottomRight.x - width * 0.01f, yPos), shadowColor, 2.0f);
    }
  }

  bool ContentBrowser::draw_color_picker_3U32(const char *label, ImU32 *color, ImGuiColorEditFlags flags) {
    float col[3];
    col[0] = (float)((*color >> 0) & 0xFF) / 255.0f;
    col[1] = (float)((*color >> 8) & 0xFF) / 255.0f;
    col[2] = (float)((*color >> 16) & 0xFF) / 255.0f;

    bool result = CherryGUI::ColorPicker3(label, col, flags);

    if (result) {
      *color = ((ImU32)(col[0] * 255.0f)) | ((ImU32)(col[1] * 255.0f) << 8) | ((ImU32)(col[2] * 255.0f) << 16) |
               ((ImU32)(255) << 24);
    }

    ImVec4 buttonColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    ImVec4 buttonHoveredColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 buttonActiveColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 cancelButtonColor = ImVec4(0.4f, 0.2f, 0.2f, 1.0f);

    CherryGUI::PushStyleColor(ImGuiCol_Button, buttonColor);
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);

    if (CherryGUI::Button("Cancel", ImVec2(75.0f, 0.0f))) {
      //
    }

    CherryGUI::PopStyleColor(3);

    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f));

    if (CherryGUI::Button("Done", ImVec2(75.0f, 0.0f))) {
      *color = ImColor(col[0], col[1], col[2]);
      vxe::publish_content_browser_custom_folder(
          current_editing_folder_.first, vxe::ImU32ToHex(*color), current_editing_folder_is_favorite_);
    }

    CherryGUI::PopStyleColor(3);

    return result;
  }

}  // namespace vxe