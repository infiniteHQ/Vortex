//
//  content_browser_rendering.cpp
//  Renders, views and window UI of the content browser
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "content_browser.hpp"

namespace vxe {

  void ContentBrowser::render() {
    const float splitterWidth = 1.5f;
    const float margin = 10.0f;

    auto &children = childs_;
    ImVec2 availableSize = CherryGUI::GetContentRegionAvail();

    for (size_t i = 0; i < children.size(); ++i) {
      auto &child = children[i];

      if (child.name == "render_filters_bar") {
        child.m_Disabled = !show_filter_pannel_;
      }

      if (child.name == "render_details_bar") {
        child.m_Disabled = !show_thumbnail_visualizer_;
      }
    }

    if (show_filter_pannel_ != previous_filter_pannel_state_ ||
        show_thumbnail_visualizer_ != previous_thumbnail_visualizer_state_) {
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
    } else if (totalChildSize < availableSize.x) {
      float extraSpace = availableSize.x - totalChildSize;

      float totalResizable = 0.0f;
      for (auto &child : children) {
        if (!child.m_Disabled) {
          totalResizable += child.m_Size;
        }
      }

      for (auto &child : children) {
        if (!child.m_Disabled) {
          float ratio = child.m_Size / totalResizable;
          child.m_Size += ratio * extraSpace;
        }
      }
    }

    for (size_t i = 0; i < children.size(); ++i) {
      auto &child = children[i];

      if (child.m_Disabled) {
        continue;
      }

      if (child.name == "render_side_bar") {
        sidebar_width_ = child.m_Size;
      }
      if (child.name == "render_filters_bar") {
        filterbar_width_ = child.m_Size;
      }

      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, child.m_BackgroundColor);
      CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

      std::string childname = child.name + "##cbchildnh" + app_window_->m_Name + child.name;
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
        vxe::ContentBrowserChild &next_child = children[nextChildIndex];

        if (i + 1 < children.size() && !children[i].m_Disabled && !next_child.m_Disabled) {
          CherryGUI::SameLine();

          std::string lab = child.name + app_window_->m_Name + "####" + child.name;

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
    previous_thumbnail_visualizer_state_ = show_thumbnail_visualizer_;
  }

  void ContentBrowser::render_side_bar() {
    const float header_width = sidebar_width_ - 46.0f;

    CherryStyle::RemoveMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", header_width);
    CherryNextComponent.SetProperty("size_y", 4.0f);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
    CherryKit::HeaderImageText("Favorite", Cherry::GetPath("resources/imgs/icons/misc/icon_star.png"), [this]() {
      for (auto custom_dir : favorite_folders_) {
        draw_hierarchy(custom_dir, true);
      }
    });

    CherryStyle::RemoveMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", header_width);
    CherryNextComponent.SetProperty("size_y", 4.0f);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
    CherryKit::HeaderImageText("Main", Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"), [this]() {
      draw_hierarchy(base_directory_, true, "Main");
    });

    CherryStyle::RemoveMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", header_width);
    CherryNextComponent.SetProperty("size_y", 4.0f);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
    CherryKit::HeaderImageText(
        "Pools & Collections", Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"), [this]() {
          CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 2.0f));
          CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));
          if (!pool_add_mode_) {
            if (CherryGUI::ImageButtonWithText(
                    Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")),
                    "Add pool",
                    ImVec2(0, 0),
                    ImVec2(0, 0),
                    ImVec2(1, 1),
                    -1,
                    ImVec4(0, 0, 0, 0),
                    ImVec4(1, 1, 1, 1))) {
              pool_add_mode_ = true;
            }
          } else {
            CherryGUI::Text("Please enter a path");
            CherryGUI::SetNextItemWidth(-FLT_MIN);
            CherryGUI::Text("Name ");
            CherryGUI::SameLine();
            CherryGUI::InputText("###AddPoolName", pool_add_name_, sizeof(pool_add_name_));

            CherryGUI::Text("Path ");
            CherryGUI::SameLine();
            CherryGUI::InputText("###AddPoolPath", pool_add_path_, sizeof(pool_add_path_));
            if (CherryGUI::ImageButtonWithText(
                    Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")),
                    "Add",
                    ImVec2(0, 0),
                    ImVec2(0, 0),
                    ImVec2(1, 1),
                    -1,
                    ImVec4(0, 0, 0, 0),
                    ImVec4(1, 1, 1, 1))) {
              vxe::publish_pool(pool_add_path_, pool_add_name_);
              refresh_pools();
              pool_add_mode_ = false;
            }
            CherryGUI::SameLine();
            if (CherryGUI::ImageButtonWithText(
                    Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_return.png")),
                    "Cancel",
                    ImVec2(0, 0),
                    ImVec2(0, 0),
                    ImVec2(1, 1),
                    -1,
                    ImVec4(0, 0, 0, 0),
                    ImVec4(1, 1, 1, 1))) {
              pool_add_mode_ = false;
            }
          }

          for (auto pool : pools_) {
            draw_hierarchy(pool.first, true, pool.second);
          }

          CherryGUI::PopStyleVar();
          CherryGUI::PopStyleColor();
        });
  }

  void ContentBrowser::render_filters_bar() {
    const float header_width = filterbar_width_ - 46.0f;

    CherryStyle::RemoveMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", header_width);
    CherryNextComponent.SetProperty("size_y", 4.0f);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
    CherryKit::HeaderImageText("Basic filters", Cherry::GetPath("resources/imgs/icons/misc/icon_star.png"), [this]() {
      CherryKit::CheckboxText("All files", &show_files_);
      CherryKit::CheckboxText("All folders", &show_folders_);
      CherryKit::CheckboxText("All items", &show_items_);
      CherryKit::CheckboxText("Hidden contents", &show_hidden_);
    });

    CherryKit::SeparatorText("Advanced filters");

    CherryStyle::RemoveMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", header_width);
    CherryNextComponent.SetProperty("size_y", 4.0f);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");

    CherryKit::HeaderImageText("Extensions filters", Cherry::GetPath("resources/imgs/icons/misc/icon_star.png"), [this]() {
      std::vector<std::pair<std::string, std::vector<std::string>>> categories = {
        { "Configuration", { ".cfg", ".ini", ".config" } }, { "Text based formats", { ".txt", ".json" } }
      };

      for (auto &category : categories) {
        CherryKit::SeparatorText(category.first);

        for (auto &ext : category.second) {
          bool state = has_extension(ext);
          CherryKit::CheckboxText(ext.c_str(), &state);
          toggle_extension(ext, &state);
        }
      }
    });
  }

  void ContentBrowser::render_menubar() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");

    if (CherryKit::ButtonImageText(
            GetLocale("loc.window.content.content_browser.add"), GetPath("resources/imgs/icons/misc/icon_add.png"))
            .GetDataAs<bool>("isClicked")) {
      spawn_add_window();
    }

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryKit::ButtonImageText(
        GetLocale("loc.window.content.content_browser.save"), GetPath("resources/imgs/icons/misc/icon_save.png"));

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryKit::ButtonImageText(
        GetLocale("loc.window.content.content_browser.import"), GetPath("resources/imgs/icons/misc/icon_import.png"));

    CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#444444AA"));
    CherryGUI::Separator();
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 12));

    static bool wasButtonX1Pressed = false;
    static bool wasButtonX2Pressed = false;

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 1.0f);
    if (back_history_.empty()) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_l_disabled.png"))
              .GetDataAs<bool>("isClicked")) {
        //
      }
    } else {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_l_enabled.png"))
              .GetDataAs<bool>("isClicked")) {
        go_back();
      }

      Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
      bool isButtonX1Pressed = mouseState & SDL_BUTTON(SDL_BUTTON_X1);
      if (isButtonX1Pressed && !wasButtonX1Pressed) {
        go_back();
      }
      wasButtonX1Pressed = isButtonX1Pressed;
    }

    if (forward_history_.empty()) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_r_disabled.png"))
              .GetDataAs<bool>("isClicked")) {
      }

    } else {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_r_enabled.png"))
              .GetDataAs<bool>("isClicked")) {
        go_forward();
      }

      Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
      bool isButtonX2Pressed = mouseState & SDL_BUTTON(SDL_BUTTON_X2);
      if (isButtonX2Pressed && !wasButtonX2Pressed) {
        go_forward();
      }
      wasButtonX2Pressed = isButtonX2Pressed;
    }

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 1.5f);
    CherryGUI::PopStyleVar();

    CherryGUI::Separator();
    CherryGUI::PopStyleColor(2);

    this->draw_path_bar(current_directory_.string());
  }

  void ContentBrowser::render_right_menubar() {
    CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));

    {
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");

      if (CherryKit::ButtonImageText("Search", GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"))
              .GetDataAs<bool>("isClicked")) {
        search_bar_ = !search_bar_;
      }

      /*{
        static std::shared_ptr<Cherry::ImageTextButtonSimple> btn =
            std::make_shared<Cherry::ImageTextButtonSimple>(
                "LogicContentManager.FindModules.Filter", "Search");
        btn->SetScale(0.85f);
        btn->SetInternalMarginX(10.0f);
        btn->SetLogoSize(15, 15);
        btn->SetImagePath(Cherry::GetPath(
            "resources/imgs/icons/misc/icon_magnifying_glass.png"));
        if (btn->render("LogicContentManager")) {
          search_bar_ = !search_bar_;

          if (!search_bar_) {
            memset(project_search_, 0, sizeof project_search_);
            btn->SetLabel("Search");
          } else {
            btn->SetLabel("Stop search");
          }
        }
      }*/

      {
        CherryNextComponent.SetProperty("padding_y", "6.0f");
        CherryNextComponent.SetProperty("padding_x", "10.0f");
        CherryNextComponent.SetProperty("disable_callback", "true");
        if (CherryKit::ButtonImageTextDropdown("Settings", GetPath("resources/imgs/icons/misc/icon_settings.png"))
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

          CherryGUI::SetNextWindowSize(ImVec2(150, 100), ImGuiCond_Appearing);
          CherryGUI::SetNextWindowPos(mousePos, ImGuiCond_Appearing);
          CherryGUI::OpenPopup("SettingsMenuPopup");
        }
        if (CherryGUI::BeginPopup("SettingsMenuPopup")) {
          CherryKit::CheckboxText("Show SSET pannel", &show_filter_pannel_);
          CherryGUI::EndPopup();
        }

        CherryNextComponent.SetProperty("padding_y", "6.0f");
        CherryNextComponent.SetProperty("padding_x", "10.0f");
        CherryNextComponent.SetProperty("disable_callback", "true");
        if (CherryKit::ButtonImageTextDropdown("View", GetPath("resources/imgs/icons/misc/icon_eye.png"))
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
          CherryKit::CheckboxText("Show Filter pannel", &show_filter_pannel_);
          CherryKit::CheckboxText("Show Thumbnail pannel", &show_thumbnail_visualizer_);
          CherryKit::SeparatorText("Content bar view");

          int default_index = 0;

          if (show_mode_ == ContentShowMode::Thumbmails) {
            default_index = 0;
          } else if (show_mode_ == ContentShowMode::List) {
            default_index = 1;
          } else if (show_mode_ == ContentShowMode::Objects) {
            default_index = 2;
          }

          switch (CherryNextComponent.SetProperty("size_x", 150.0f);
                  CherryKit::ComboImageText(
                      "",
                      { { "Thumbnails", GetPath("resources/imgs/icons/misc/icon_thumbnails.png") },
                        { "List", GetPath("resources/imgs/icons/misc/icon_lines.png") },
                        { "Objects", GetPath("resources/imgs/icons/misc/icon_objects.png") } },
                      default_index)
                      .GetPropertyAs<int>("selected")) {
            case 0: {
              show_mode_ = ContentShowMode::Thumbmails;
              break;
            }
            case 1: {
              show_mode_ = ContentShowMode::List;
              break;
            }
            case 2: {
              show_mode_ = ContentShowMode::Objects;
              break;
            }
            default: {
              show_mode_ = ContentShowMode::Thumbmails;
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
      }
    }

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 3.0f);

    CherryGUI::PopStyleColor();
    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 1.5f);
  }

  void ContentBrowser::render_content_bar() {
    ImGuiStyle &style = CherryGUI::GetStyle();
    ImVec4 originalChildBgColor = style.Colors[ImGuiCol_ChildBg];
    ImVec4 originalBorderColor = style.Colors[ImGuiCol_Border];
    ImVec4 originalBorderShadowColor = style.Colors[ImGuiCol_BorderShadow];
    float oldfontsize = CherryGUI::GetFont()->Scale;

    style.Colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    float cellSize = thumbnail_size + padding;

    float panelWidth = CherryGUI::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
      columnCount = 1;
    if (search_bar_) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      if (CherryKit::ButtonImageText("Filters", GetPath("resources/imgs/icons/misc/icon_filter.png"))
              .GetDataAs<bool>("isClicked")) {
        will_show_filter_pannel_ = !will_show_filter_pannel_;
      }

      CherryGUI::SameLine();

      CherryNextComponent.SetProperty("size_x", "240");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("description", "Search content...");
      CherryNextComponent.SetProperty("description_logo", GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
      CherryNextComponent.SetProperty("description_logo_place", "r");
      CherryKit::InputString("", &project_search_);

      CherryKit::Separator();
    }

    CherryGUI::Spacing();

    std::vector<std::filesystem::directory_entry> directories;
    std::vector<std::filesystem::directory_entry> files;

    recognized_modules_items_.clear();

    for (auto &directoryEntry : std::filesystem::directory_iterator(current_directory_)) {
      bool isItem = false;
      for (auto em : vxe::get_current_context()->IO.em) {
        for (auto item : em->get_content_browser_item_identifiers()) {
          std::string path = directoryEntry.path().string();
          if (item->f_Detect(path)) {
            recognized_modules_items_.push_back({ item, path });
            isItem = true;
          }
        }
      }

      for (auto ep : vxe::get_current_context()->IO.ep) {
        for (auto item : ep->get_content_browser_item_identifiers()) {
          std::string path = directoryEntry.path().string();
          if (item->f_Detect(path)) {
            recognized_modules_items_.push_back({ item, path });
            isItem = true;
          }
        }
      }

      if (isItem) {
        continue;
      }

      if (directoryEntry.is_directory()) {
        directories.push_back(directoryEntry);
      } else {
        files.push_back(directoryEntry);
      }
    }

    std::sort(directories.begin(), directories.end(), [](const auto &a, const auto &b) {
      return a.path().filename().string() < b.path().filename().string();
    });

    bool isWindowFocused = CherryGUI::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    bool ctrl = CherryGUI::GetIO().KeyCtrl;
    bool vPressed = CherryGUI::IsKeyPressed(ImGuiKey_V);

    if (isWindowFocused && ctrl && vPressed) {
      if (paste_paths_callback_) {
        paste_paths_callback_({ current_directory_.string() });
      }
    }

    if (show_mode_ == ContentShowMode::Thumbmails) {
      CherryGUI::Columns(columnCount, 0, false);

      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

      CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

      CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

      if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
        selected_.clear();
        CherryGUI::OpenPopup("EmptySpacePopup");
      }

      if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Left)) {
        selected_.clear();
      }

      if (CherryGUI::BeginPopup("EmptySpacePopup")) {
        CherryGUI::GetFont()->Scale = 0.9;
        CherryGUI::PushFont(CherryGUI::GetFont());

        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

        CherryKit::SeparatorText("Add");
        if (CherryGUI::MenuItem(
                "Add",
                "Add a component",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")),
                NULL)) {
          spawn_add_window();
        }
        if (CherryGUI::MenuItem(
                "Import",
                "Import a component",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_import.png")),
                NULL)) {
        }

        CherryKit::SeparatorText("Create");
        if (CherryGUI::MenuItem(
                "Create new folder",
                "Create a folder here",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_folder.png")),
                NULL)) {
          create_folder();
        }
        if (CherryGUI::MenuItem(
                "Create new file",
                "Create empty file here",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_file.png")),
                NULL)) {
          create_file();
        }

        CherryKit::SeparatorText("Actions");

        if (CherryGUI::MenuItem(
                "Paste selection",
                "Paste selection here",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_down.png")),
                NULL)) {
          if (paste_paths_callback_) {
            paste_paths_callback_({ current_directory_.string() });
          }
        }

        if (CherryGUI::MenuItem(
                "Select All",
                "Select eveything on this directory",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
                NULL)) {
        }

        CherryGUI::GetFont()->Scale = oldfontsize;
        CherryGUI::PopFont();

        CherryGUI::EndPopup();
      }

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(2);

      if (show_folders_) {
        for (auto &directoryEntry : directories) {
          const auto &path = directoryEntry.path();
          std::string filenameString = path.filename().string();

          bool selected = false;

          if (std::find(selected_.begin(), selected_.end(), path) != selected_.end()) {
            selected = true;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            CherryGUI::PushID(filenameString.c_str());

            bool rename = false;
            if (path.string() == path_to_rename_) {
              rename = true;
            }

            if (selected) {
              CherryGUI::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 1.0f, 0.5f));
              CherryGUI::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 1.0f, 0.5f));
              CherryGUI::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 1.0f, 0.5f));
            }

            if (selected) {
              CherryGUI::PopStyleColor(3);
            }
            float reducedthumbnail_size = thumbnail_size * 0.9f;
            float availableWidth = CherryGUI::GetContentRegionAvail().x;
            float imageOffsetX = (availableWidth - reducedthumbnail_size) * 0.5f;

            CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            // Old folder logo
            /*CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + imageOffsetX);
            addTexture(Folder_Logo, Folder_Logo);
            CherryGUI::ImageButton(getTexture(Folder_Logo), {reducedthumbnail_size,
            reducedthumbnail_size}, {-1, 0}, {0, 1});

            if (CherryGUI::IsItemHovered() &&
            CherryGUI::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                change_directory(path);.string()
            }*/
            CherryStyle::RemoveMarginX(5.0f);
            ImVec2 folderSize(reducedthumbnail_size, reducedthumbnail_size);
            float folderPosX = (availableWidth - folderSize.x) * 0.5f;
            CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + folderPosX);

            // get_content_browser_folder_color(path.string();
            std::string folder_color = "#c2a24c";

            if (selected) {
              folder_color = "#2424CC";
            }

            if (current_editing_folder_.first == path.string()) {
              draw_folder_button(
                  "draw_folder_icon", folderSize, path.string(), Cherry::ImU32ToHex(current_editing_folder_.second));
            } else {
              draw_folder_button(
                  "draw_folder_icon", folderSize, path.string(), get_content_browser_folder_color(path.string()));
            }

            float oldsize = CherryGUI::GetFont()->Scale;

            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

            CherryGUI::PopStyleVar(2);
            CherryGUI::PopStyleColor(3);

            ImVec2 cursorPos = CherryGUI::GetCursorPos();
            float textWidth = CherryGUI::CalcTextSize(filenameString.c_str()).x;
            float textX = folderPosX + (folderSize.x - textWidth) * 0.5f;
            CherryGUI::SetCursorPos(ImVec2(textX + CherryGUI::GetCursorPosX(), cursorPos.y));
            // CherryGUI::TextWrapped(filenameString.c_str());

            ImDrawList *drawList = CherryGUI::GetWindowDrawList();

            ImU32 textColor = IM_COL32(255, 255, 255, 255);
            ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
            ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
            static std::unordered_map<std::string, char[256]> renameBuffers;

            if (path_to_rename_ == path.string()) {
              CherryGUI::SetItemAllowOverlap();
              CherryGUI::PushID(path.c_str());

              auto &buffer = renameBuffers[path.string()];
              if (buffer[0] == '\0') {
                std::string filename = path.string().substr(path.string().find_last_of("/\\") + 1);
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
                  drawList,
                  CherryGUI::GetCursorScreenPos(),
                  filenameString.c_str(),
                  project_search_.c_str(),
                  highlightColor,
                  textColor,
                  highlightTextColor);
            }

            if (CherryGUI::BeginPopupContextItem("ContextPopup")) {
              CherryKit::SeparatorText("Main");

              if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
                path_to_rename_ = path.string();
              }

              {
                std::string label = "Cut (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + X")) {
                  if (cut_paths_callback_) {
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
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                  if (cut_paths_callback_) {
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
                std::string label = "Copy (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
                  if (copy_paths_callback_) {
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
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                  if (copy_paths_callback_) {
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
                  delete_path_callback_(path.string());
                }
                selected_.clear();
                CherryGUI::CloseCurrentPopup();
              }

              if (selected_.size() > 1) {
                std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
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

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

              static bool EditingColor = false;
              static bool ColorChanged = false;

              current_editing_folder_is_favorite_ = is_path_favorite(directoryEntry.path().string());

              if (CherryGUI::BeginMenu("Change color")) {
                if (!EditingColor) {
                  current_editing_folder_ = { directoryEntry.path().string(), Cherry::HexToImU32(folder_color) };

                  current_editing_folder_ = { directoryEntry.path().string(),
                                              HexToImU32(get_content_browser_folder_color(path.string())) };

                  current_editing_folder_is_favorite_ = is_path_favorite(directoryEntry.path().string());
                }

                EditingColor = true;

                static bool alpha_preview = true;
                static bool alpha_half_preview = false;
                static bool drag_and_drop = true;
                static bool options_menu = true;
                static bool hdr = false;

                draw_color_picker_3U32("MyColor", &current_editing_folder_.second);

                if (current_editing_folder_.second != Cherry::HexToImU32(folder_color)) {
                  ColorChanged = true;
                }

                CherryGUI::EndMenu();
              } else {
                EditingColor = false;
                current_editing_folder_ = { "", ImU32() };
              }

              if (CherryGUI::MenuItem("Mark as favorite", "", current_editing_folder_is_favorite_)) {
                current_editing_folder_ = { directoryEntry.path().string(), current_editing_folder_.second };

                current_editing_folder_is_favorite_ = !current_editing_folder_is_favorite_;
                set_colored_folder(current_editing_folder_.first, ImU32ToHex(current_editing_folder_.second));

                vxe::publish_content_browser_custom_folder(
                    current_editing_folder_.first,
                    Cherry::ImU32ToHex(current_editing_folder_.second),
                    current_editing_folder_is_favorite_);

                if (current_editing_folder_is_favorite_) {
                  favorite_folders_.push_back(current_editing_folder_.first);
                } else {
                  auto it = std::find(favorite_folders_.begin(), favorite_folders_.end(), current_editing_folder_.first);
                  if (it != favorite_folders_.end()) {
                    favorite_folders_.erase(it);
                  }
                }
              }

              CherryGUI::EndPopup();
            }

            CherryGUI::PopID();
            CherryGUI::NextColumn();
          }
        }
      }

      if (show_items_) {
        for (auto &itemEntry : recognized_modules_items_) {
          const auto &path = itemEntry.second;
          std::filesystem::path fsPath(path);
          std::string filenameString = fsPath.filename().string();

          bool selected = false;

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto &ext : extentions_filters_) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }

            if (!match) {
              continue;
            }
          }

          if (std::find(selected_.begin(), selected_.end(), path) != selected_.end()) {
            selected = true;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            std::uintmax_t folderSize = get_directory_size(path);
            std::string folderSizeString = format_file_size(folderSize);
            CherryGUI::PushID(filenameString.c_str());
            ItemCardContent content{
              .name = filenameString,
              .path = path,
              .description = itemEntry.first->m_Description,
              .size = folderSizeString,
              .logo = itemEntry.first->m_LogoPath,
              .item_ident = itemEntry.first,
            };

            ItemCardStyle style{
              .bg_color = IM_COL32(56, 56, 56, 150),
              .border_color = IM_COL32(50, 50, 50, 255),
              .line_color = Cherry::HexToImU32(itemEntry.first->m_LineColor),
              .max_text_width = 100.0f,
              .border_radius = 5.0f,
            };

            if (draw_item_card({ .content = content, .style = style, .selected = selected })) {
              if (CherryGUI::IsMouseDoubleClicked(0)) {
                // itemEntry.first->f_execute(path);
                // VXERROR("te", "tyr");
              }
              if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
                selected_.push_back(path);
              } else {
                selected_.clear();
                selected_.push_back(path);
              }
            }

            CherryGUI::PopID();
            CherryGUI::NextColumn();
          }
        }
      }

      if (show_files_) {
        for (auto &fileEntry : files) {
          const auto &path = fileEntry.path();
          std::string filenameString = path.filename().string();

          bool selected = false;

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto &ext : extentions_filters_) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }

            if (!match) {
              continue;
            }
          }

          if (std::find(selected_.begin(), selected_.end(), path) != selected_.end()) {
            selected = true;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            std::error_code ec;
            size_t fileSize = 0;

            if (fileEntry.is_regular_file(ec)) {
              fileSize += std::filesystem::file_size(fileEntry.path(), ec);
            }

            std::string fileSizeString = format_file_size(fileSize);
            CherryGUI::PushID(filenameString.c_str());

            FileTypes fileType = detect_file(path.string());

            auto it = FILE_TYPE_INFOS.find(fileType);
            const FileTypeInfo &info = (it != FILE_TYPE_INFOS.end()) ? it->second : DEFAULT_TYPE_INFO;

            ItemCardContent content{
              .name = filenameString,
              .path = path.string(),
              .description = info.label,
              .size = fileSizeString,
              .logo = Cherry::GetPath(info.icon),
              .item_ident = nullptr,
            };

            ItemCardStyle style{
              .bg_color = IM_COL32(56, 56, 56, 150),
              .border_color = IM_COL32(50, 50, 50, 255),
              .line_color = Cherry::HexToImU32(info.color),
              .max_text_width = 100.0f,
              .border_radius = 5.0f,
            };

            if (draw_item_card({ .content = content, .style = style, .selected = selected })) {
              if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
                selected_.push_back(path.string());
              } else {
                selected_.clear();
                selected_.push_back(path.string());
              }
            }

            float oldsize = CherryGUI::GetFont()->Scale;

            CherryGUI::PopID();
            CherryGUI::NextColumn();
          }
        }
      }
      CherryGUI::Columns(1);
    }

    if (show_mode_ == ContentShowMode::List) {
      static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                     ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

      if (CherryGUI::BeginTable("ContentBrowserTable", 5, flags)) {
        CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        CherryGUI::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        CherryGUI::TableSetupColumn("Last Updated", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        CherryGUI::TableHeadersRow();
        auto drawRow = [&](const std::string &filename,
                           const fs::path &path,
                           bool isFolder,
                           size_t size = 0,
                           std::time_t cftime = 0,
                           ImTextureID tex = nullptr,
                           const std::shared_ptr<ItemIdentifierInterface> &item_ident = nullptr) {
          std::string filenameString = path.filename().string();
          float rowHeight = 20.0f;
          CherryGUI::TableNextRow(ImGuiTableRowFlags_None, rowHeight);

          CherryGUI::TableSetColumnIndex(0);

          bool selected = std::find(selected_.begin(), selected_.end(), path.string()) != selected_.end();

          if (CherryGUI::Selectable(
                  ("##row_" + path.string()).c_str(),
                  selected,
                  ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick,
                  ImVec2(0, rowHeight))) {
            if (CherryGUI::GetIO().KeyCtrl) {
              if (!selected)
                selected_.push_back(path.string());
            } else {
              selected_.clear();
              selected_.push_back(path.string());
            }

            if (CherryGUI::IsMouseClicked(0)) {
              if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
                selected_.push_back(path.string());
              } else {
                selected_.clear();
                selected_.push_back(path.string());
              }
            }

            if (CherryGUI::IsMouseDoubleClicked(0)) {
              if (!isFolder) {
                // Default action
              } else {
                change_directory(path);
              }
              //
            }
          }

          item_context_menu_opened_ = false;
          if (!isFolder) {
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
              path_to_rename_ = path.string();
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
            if (CherryGUI::BeginPopupContextItem(("context_" + path.string()).c_str())) {
              CherryGUI::GetFont()->Scale = 0.9;
              CherryGUI::PushFont(CherryGUI::GetFont());

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

              ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
              ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

              item_context_menu_opened_ = true;

              FileTypes fileType = detect_file(path.string());

              if (item_handles_.empty()) {
                if (item_ident) {
                  item_handles_ = vxe::get_all_item_handlers_for(item_ident->m_Name);
                } else {
                  item_handles_ = vxe::get_all_item_handlers_for(get_file_type_str(fileType));
                }
              }

              if (!item_handles_.empty()) {
                CherryKit::SeparatorText("Actions");
              }

              for (auto ih : item_handles_) {
                if (CherryGUI::MenuItem(ih->title.c_str(), ih->description.c_str(), Cherry::GetTexture(ih->logo), false)) {
                  ih->handler(path.string());
                }
              }

              CherryKit::SeparatorText("Main");

              CherryGUI::GetFont()->Scale = oldfontsize;
              CherryGUI::PopFont();

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

              if (item_ident) {
                if (CherryGUI::MenuItem(
                        "Open item",
                        "",
                        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_foldersearch.png")),
                        NULL)) {
                  change_directory(path);
                  itepath_s_.push_back({ path.string(), item_ident->m_LineColor });
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (CherryGUI::MenuItem(
                      "Copy",
                      "Ctrl + C",
                      Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")),
                      NULL)) {
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
                      "Cut",
                      "Ctrl + X",
                      Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")),
                      NULL)) {
                change_directory(path);
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

          } else {
            if (CherryGUI::BeginPopupContextItem(("context_" + path.string()).c_str())) {
              CherryKit::SeparatorText("Main");

              if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
                path_to_rename_ = path.string();
              }

              {
                std::string label = "Cut (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + X")) {
                  if (cut_paths_callback_) {
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
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                  if (cut_paths_callback_) {
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
                std::string label = "Copy (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
                  if (copy_paths_callback_) {
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
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                  if (copy_paths_callback_) {
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
                  delete_path_callback_(path.string());
                }
                selected_.clear();
                CherryGUI::CloseCurrentPopup();
              }

              if (selected_.size() > 1) {
                std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
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
          }

          if (!item_context_menu_opened_) {
            item_handles_.clear();
          }

          CherryGUI::SameLine();
          FileTypes fileType = detect_file(path.string());

          if (!isFolder) {
            if (item_ident) {
              tex = Cherry::GetTexture(item_ident->m_LogoPath);
            } else {
              auto it = FILE_ICONS.find(fileType);
              if (it != FILE_ICONS.end()) {
                tex = Cherry::GetTexture(Cherry::GetPath(it->second));
              } else {
                tex = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/files/icon_unknow_file.png"));
              }
            }

            if (tex) {
              CherryGUI::Image(tex, ImVec2(24, 24));
              CherryGUI::SameLine();
            }
          } else {
            if (current_editing_folder_.first == path.string()) {
              draw_folder_button(
                  "draw_folder_icon", ImVec2(16, 16), path.string(), get_content_browser_folder_color(path.string()));
            } else {
              draw_folder_button(
                  "draw_folder_icon", ImVec2(16, 16), path.string(), get_content_browser_folder_color(path.string()));
            }
            CherryGUI::SameLine();
          }
          // CherryGUI::TextUnformatted(filename.c_str());

          ImU32 textColor = IM_COL32(255, 255, 255, 255);
          ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
          ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
          static std::unordered_map<std::string, char[256]> renameBuffers;

          if (path_to_rename_ == path.string()) {
            CherryGUI::SetItemAllowOverlap();
            CherryGUI::PushID(path.string().c_str());

            auto &buffer = renameBuffers[path.string()];
            if (buffer[0] == '\0') {
              std::string filename = path.string().substr(path.string().find_last_of("/\\") + 1);
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
            if (filename == "...") {
              filenameString = "...";
            }

            ImDrawList *drawList = CherryGUI::GetWindowDrawList();
            draw_highlighted_text_(
                drawList,
                CherryGUI::GetCursorScreenPos(),
                filenameString.c_str(),
                project_search_.c_str(),
                highlightColor,
                textColor,
                highlightTextColor);
          }

          CherryGUI::TableSetColumnIndex(1);
          if (item_ident) {
            CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), item_ident->m_Description.c_str());
          } else {
            CherryGUI::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), isFolder ? "Folder" : "File");
          }

          CherryGUI::TableSetColumnIndex(2);
          CherryGUI::TextUnformatted(isFolder ? "---" : format_file_size(size).c_str());

          CherryGUI::TableSetColumnIndex(3);
          if (cftime != 0) {
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&cftime));
            CherryGUI::TextUnformatted(buffer);
          } else {
            CherryGUI::TextUnformatted("---");
          }
        };

        fs::path projectRoot = vxe::get_current_context()->projectDataPath;

        if (!current_directory_.empty()) {
          auto rel = fs::relative(current_directory_, projectRoot);

          if (!rel.empty() && *rel.begin() != std::filesystem::path("..") && current_directory_ != projectRoot) {
            drawRow("...", current_directory_.parent_path(), true);
          }
        }

        if (show_folders_) {
          for (auto &directoryEntry : directories) {
            const auto &path = directoryEntry.path();
            std::string filename = path.filename().string();

            if (!are_strings_similar(filename, project_search_, threshold_) &&
                !is_only_spaces_or_empty(project_search_.c_str()))
              continue;

            ImTextureID tex = nullptr;
            FileTypes type = detect_file(path.string());
            switch (type) {
              case FileTypes::File_PICTURE:
                tex = Application::GetCurrentRenderedWindow()->get_texture(
                    Cherry::GetPath("resources/imgs/icons/files/icon_picture_file.png"));
                break;
              default:
                tex = Application::GetCurrentRenderedWindow()->get_texture(
                    Cherry::GetPath("resources/imgs/icons/files/icon_default_file.png"));
                break;
            }
            std::error_code ec;
            auto ftime = std::filesystem::last_write_time(path, ec);
            std::time_t cftime = 0;
            if (!ec) {
              auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                  ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
              cftime = std::chrono::system_clock::to_time_t(sctp);
            }

            drawRow(filename, path, true, 0, cftime, tex);
          }
        }

        if (show_items_) {
          for (auto &itemEntry : recognized_modules_items_) {
            const auto &path = itemEntry.second;
            std::filesystem::path fsPath(path);
            std::filesystem::directory_entry fsDirectoryEntry(path);
            std::string filenameString = fsPath.filename().string();

            if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
              continue;
            }

            if (!extentions_filters_.empty()) {
              bool match = false;
              for (const auto &ext : extentions_filters_) {
                if (filenameString.size() >= ext.size() &&
                    filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                  match = true;
                  break;
                }
              }

              if (!match) {
                continue;
              }
            }

            if (!are_strings_similar(filenameString, project_search_, threshold_) &&
                !is_only_spaces_or_empty(project_search_.c_str()))
              continue;

            std::uintmax_t folderSize = get_directory_size(path);
            std::string folderSizeString = format_file_size(folderSize);
            CherryGUI::PushID(filenameString.c_str());

            ImTextureID tex = Application::GetCurrentRenderedWindow()->get_texture(
                Cherry::GetPath("resources/imgs/icons/files/icon_default_file.png"));

            std::error_code ec;
            size_t fsize = fsDirectoryEntry.is_regular_file(ec) ? std::filesystem::file_size(path, ec) : 0;
            auto ftime = std::filesystem::last_write_time(path, ec);
            std::time_t cftime = 0;
            if (!ec) {
              auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                  ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
              cftime = std::chrono::system_clock::to_time_t(sctp);
            }

            drawRow(filenameString, path, false, fsize, cftime, tex, itemEntry.first);
          }
        }

        if (show_files_) {
          for (auto &fileEntry : files) {
            const auto &path = fileEntry.path();
            std::string filename = path.filename().string();

            if (!show_hidden_ && !filename.empty() && filename[0] == '.') {
              continue;
            }

            if (!extentions_filters_.empty()) {
              bool match = false;
              for (const auto &ext : extentions_filters_) {
                if (filename.size() >= ext.size() && filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0) {
                  match = true;
                  break;
                }
              }

              if (!match) {
                continue;
              }
            }

            if (!are_strings_similar(filename, project_search_, threshold_) &&
                !is_only_spaces_or_empty(project_search_.c_str()))
              continue;

            ImTextureID tex = Application::GetCurrentRenderedWindow()->get_texture(
                Cherry::GetPath("resources/imgs/icons/files/icon_default_file.png"));
            FileTypes type = detect_file(path.string());
            std::error_code ec;
            size_t fsize = fileEntry.is_regular_file(ec) ? std::filesystem::file_size(path, ec) : 0;
            auto ftime = std::filesystem::last_write_time(path, ec);
            std::time_t cftime = 0;
            if (!ec) {
              auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                  ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
              cftime = std::chrono::system_clock::to_time_t(sctp);
            }

            drawRow(filename, path.string(), false, fsize, cftime, tex);
          }
        }
        CherryGUI::EndTable();
      }
    }

    if (show_mode_ == ContentShowMode::Objects) {
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
      ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
      ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

      CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

      CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

      if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
        selected_.clear();
        CherryGUI::OpenPopup("EmptySpacePopup");
      }

      if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Left)) {
        selected_.clear();
      }

      if (CherryGUI::BeginPopup("EmptySpacePopup")) {
        CherryGUI::GetFont()->Scale = 0.9;
        CherryGUI::PushFont(CherryGUI::GetFont());

        CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

        CherryKit::SeparatorText("Add");
        if (CherryGUI::MenuItem(
                "Add",
                "Add a component",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")),
                NULL)) {
          spawn_add_window();
        }
        if (CherryGUI::MenuItem(
                "Import",
                "Import a component",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_import.png")),
                NULL)) {
        }

        CherryKit::SeparatorText("Create");
        if (CherryGUI::MenuItem(
                "Create new folder",
                "Create a folder here",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_folder.png")),
                NULL)) {
          create_folder();
        }
        if (CherryGUI::MenuItem(
                "Create new file",
                "Create empty file here",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_file.png")),
                NULL)) {
          create_file();
        }

        CherryKit::SeparatorText("Actions");

        if (CherryGUI::MenuItem(
                "Paste selection",
                "Paste selection here",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_down.png")),
                NULL)) {
          if (paste_paths_callback_) {
            paste_paths_callback_({ current_directory_.string() });
          }
        }

        if (CherryGUI::MenuItem(
                "Select All",
                "Select eveything on this directory",
                Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
                NULL)) {
        }

        CherryGUI::GetFont()->Scale = oldfontsize;
        CherryGUI::PopFont();

        CherryGUI::EndPopup();
      }

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(2);

      if (show_folders_) {
        for (auto &directoryEntry : directories) {
          const auto &path = directoryEntry.path();
          std::string filenameString = path.filename().string();

          bool selected = (std::find(selected_.begin(), selected_.end(), path) != selected_.end());

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            CherryGUI::PushID(filenameString.c_str());

            float padding = 10.0f;
            float logoSize = 40.0f;
            float borderRadius = 5.0f;

            float availableWidth = CherryGUI::GetContentRegionAvail().x;
            float cardHeight = logoSize + padding * 2;
            ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();
            ImVec2 cardSize(availableWidth, cardHeight);

            if (CherryGUI::InvisibleButton(("folder_card_" + filenameString).c_str(), cardSize)) {
              if (CherryGUI::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                std::cout << "[LOG] Double click on: " << path.string() << std::endl;
                change_directory(path);
              } else {
                if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
                  selected_.push_back(path.string());
                } else {
                  selected_.clear();
                  selected_.push_back(path.string());
                }
              }
            }

            if (CherryGUI::IsItemHovered()) {
              CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);

              if (CherryGUI::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                change_directory(path);
              }
            }

            if (CherryGUI::BeginPopupContextItem("ContextPopup")) {
              CherryKit::SeparatorText("Main");

              if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
                path_to_rename_ = path.string();
              }

              {
                std::string label = "Cut (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + X")) {
                  if (cut_paths_callback_) {
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
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                  if (cut_paths_callback_) {
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
                std::string label = "Copy (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
                  if (copy_paths_callback_) {
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
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                  if (copy_paths_callback_) {
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
                  delete_path_callback_(path.string());
                }
                selected_.clear();
                CherryGUI::CloseCurrentPopup();
              }

              if (selected_.size() > 1) {
                std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
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

            ImU32 bgColor = selected ? IM_COL32(80, 80, 240, 255) : IM_COL32(40, 40, 40, 255);
            ImU32 borderColor = selected ? IM_COL32(150, 150, 255, 255) : IM_COL32(90, 90, 90, 255);

            ImDrawList *drawList = CherryGUI::GetWindowDrawList();

            drawList->AddRectFilled(
                cursorPos, ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y), bgColor, borderRadius);
            drawList->AddRect(
                cursorPos, ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y), borderColor, borderRadius);

            ImVec2 logoPos(cursorPos.x + padding, cursorPos.y + (cardHeight - logoSize) * 0.5f);
            CherryGUI::SetCursorScreenPos(logoPos);
            draw_folder_button(
                "draw_folder_icon",
                ImVec2(logoSize, logoSize),
                path.string(),
                get_content_browser_folder_color(path.string()));

            ImVec2 logoEnd(logoPos.x + logoSize, logoPos.y + logoSize);

            CherryGUI::PushFont(CherryGUI::GetFont());
            CherryGUI::GetFont()->Scale = 0.9f;

            ImVec2 textPos(logoEnd.x + padding, cursorPos.y + padding);
            CherryGUI::SetCursorScreenPos(textPos);

            ImU32 textColor = IM_COL32(255, 255, 255, 255);
            ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
            ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
            static std::unordered_map<std::string, char[256]> renameBuffers;

            if (path_to_rename_ == path.string()) {
              CherryGUI::PushID(path.c_str());
              auto &buffer = renameBuffers[path.string()];
              if (buffer[0] == '\0') {
                std::strncpy(buffer, filenameString.c_str(), sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                CherryGUI::SetKeyboardFocusHere();
              }
              if (CherryGUI::InputText("##rename_input", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                vxe::submit_rename(path_to_rename_, buffer);
                renameBuffers.erase(path_to_rename_);
                path_to_rename_.clear();
              }
              CherryGUI::PopID();
            } else {
              draw_highlighted_text_(
                  drawList,
                  textPos,
                  filenameString.c_str(),
                  project_search_.c_str(),
                  highlightColor,
                  textColor,
                  highlightTextColor);
            }

            CherryGUI::GetFont()->Scale = oldfontsize;
            CherryGUI::PopFont();

            std::string folderSizeStr = "[dir]";
            std::string lastModifiedStr = "???";

            ImVec2 rightTextPos(cursorPos.x + cardSize.x - padding - 200, cursorPos.y + padding);
            CherryGUI::SetCursorScreenPos(rightTextPos);
            CherryGUI::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
            CherryGUI::Text("%s | %s", folderSizeStr.c_str(), lastModifiedStr.c_str());
            CherryGUI::PopStyleColor();

            float lineOffset = 15.0f;
            ImVec2 lineStart(cursorPos.x + cardSize.x - lineOffset, cursorPos.y + padding);
            ImVec2 lineEnd(cursorPos.x + cardSize.x - lineOffset, cursorPos.y + cardSize.y - padding);
            drawList->AddLine(lineStart, lineEnd, IM_COL32(255, 200, 0, 255), 3.0f);

            float versionBoxWidth = 10.0f, versionBoxHeight = 20.0f;
            ImVec2 versionPos(
                cursorPos.x + cardSize.x - versionBoxWidth - padding, cursorPos.y + cardSize.y - versionBoxHeight - padding);
            drawList->AddRectFilled(
                versionPos,
                ImVec2(versionPos.x + versionBoxWidth, versionPos.y + versionBoxHeight),
                IM_COL32(0, 0, 0, 255),
                borderRadius);

            CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + padding));

            CherryGUI::PopID();
          }
        }
      }

      if (show_items_) {
        for (auto &itemEntry : recognized_modules_items_) {
          const auto &path = itemEntry.second;
          std::filesystem::path fsPath(path);
          std::string filenameString = fsPath.filename().string();

          bool selected = false;

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto &ext : extentions_filters_) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }

            if (!match) {
              continue;
            }
          }

          if (std::find(selected_.begin(), selected_.end(), path) != selected_.end()) {
            selected = true;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            std::uintmax_t folderSize = get_directory_size(path);
            std::string folderSizeString = format_file_size(folderSize);
            CherryGUI::PushID(filenameString.c_str());

            ItemCardContent content{
              .name = filenameString,
              .path = path,
              .description = itemEntry.first->m_Description,
              .size = folderSizeString,
              .logo = itemEntry.first->m_LogoPath,
              .item_ident = itemEntry.first,
            };

            ItemCardStyle style{
              .bg_color = IM_COL32(56, 56, 56, 150),
              .border_color = IM_COL32(50, 50, 50, 255),
              .line_color = Cherry::HexToImU32(itemEntry.first->m_LineColor),
              .max_text_width = 100.0f,
              .border_radius = 5.0f,
            };

            if (draw_horizontal_draw_item_card(
                    { .content = content, .style = style, .selected = selected, .isFolder = false })) {
              if (CherryGUI::IsMouseDoubleClicked(0)) {
                // itemEntry.first->f_execute(path);
                // VXERROR("te", "tyr");
              }
              if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
                selected_.push_back(path);
              } else {
                selected_.clear();
                selected_.push_back(path);
              }
            }
            CherryGUI::PopID();
            CherryGUI::NextColumn();
          }
        }
      }

      if (show_files_) {
        for (auto &fileEntry : files) {
          const auto &path = fileEntry.path();
          std::string filenameString = path.filename().string();

          bool selected = false;
          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto &ext : extentions_filters_) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }

            if (!match) {
              continue;
            }
          }

          if (std::find(selected_.begin(), selected_.end(), path) != selected_.end()) {
            selected = true;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            std::error_code ec;
            size_t fileSize = 0;

            if (fileEntry.is_regular_file(ec)) {
              fileSize += std::filesystem::file_size(fileEntry.path(), ec);
            }

            std::string fileSizeString = format_file_size(fileSize);
            CherryGUI::PushID(filenameString.c_str());

            FileTypes fileType = detect_file(path.string());

            auto it = FILE_TYPE_INFOS.find(fileType);
            const FileTypeInfo &info = (it != FILE_TYPE_INFOS.end()) ? it->second : DEFAULT_TYPE_INFO;

            ItemCardContent content{
              .name = filenameString,
              .path = path.string(),
              .description = info.label,
              .size = fileSizeString,
              .logo = Cherry::GetPath(info.icon),
              .item_ident = nullptr,
            };

            ItemCardStyle style{
              .bg_color = IM_COL32(56, 56, 56, 150),
              .border_color = IM_COL32(50, 50, 50, 255),
              .line_color = Cherry::HexToImU32(info.color),
              .max_text_width = 200.f,
              .border_radius = 8.f,
            };

            if (draw_horizontal_draw_item_card({ .content = content, .style = style, .selected = selected })) {
              if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
                selected_.push_back(path.string());
              } else {
                selected_.clear();
                selected_.push_back(path.string());
              }
            }

            float oldsize = CherryGUI::GetFont()->Scale;

            CherryGUI::PopID();
          }
        }
      }
    }
  }

  void ContentBrowser::add_child(const ContentBrowserChild &child) {
    childs_.push_back(child);
  }

  void ContentBrowser::spawn_add_window() {
    add_windows_counter_++;
    Cherry::ApplicationSpecification spec;

    std::string name = "?loc:loc.window_names.add_content" + std::to_string(add_windows_counter_);
    auto new_win = vxe::AddWindow::create(name, current_directory_.string());
    new_win->get_app_window()->SetVisibility(true);
    new_win->set_create_file_callback([this]() { this->create_file(); });
    new_win->set_create_folder_callback([this]() { this->create_folder(); });
    new_win->set_import_content_callback([this]() { this->spawn_import_window(); });

    std::string label = "Add content";
    spec.Name = label;
    spec.MinHeight = 300;
    spec.MinWidth = 175;
    spec.Height = 600;
    spec.DisableLogo = true;
    spec.DisableResize = true;
    spec.Width = 400;
    spec.CustomTitlebar = true;
    spec.DisableWindowManagerTitleBar = true;
    spec.WindowOnlyClosable = true;
    spec.RenderMode = Cherry::WindowRenderingMethod::SimpleWindow;
    spec.UniqueAppWindowName = new_win->get_app_window()->m_Name;
    spec.FramebarCallback = []() { };
    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, new_win]() { Cherry::DeleteAppWindow(new_win->get_app_window()); };

    spec.MenubarCallback = []() { };
    spec.WindowSaves = false;
    new_win->get_app_window()->AttachOnNewWindow(spec);
    Cherry::AddAppWindow(new_win->get_app_window());
  }

  void ContentBrowser::spawn_import_window() {
    // TODO
  }
}  // namespace vxe