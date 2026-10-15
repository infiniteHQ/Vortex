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
    if (!focus_window_applied_) {
      if (!focus_window_.empty()) {
        for (auto& w : CherryApp.GetWindows()) {
          if (w->GetName() == focus_window_) {
            CherryApp.QuickRedock(app_window_->GetName(), w->GetName());
          }
        }
      }
      focus_window_applied_ = true;
    }

    const float splitterWidth = 1.5f;
    const float margin = 10.0f;

    auto& children = childs_;
    ImVec2 availableSize = CherryGUI::GetContentRegionAvail();

    for (size_t i = 0; i < children.size(); ++i) {
      auto& child = children[i];

      if (child.name == "render_side_bar") {
        child.m_Disabled = !show_side_bar_pannel_;
      }

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

      for (auto& child : children) {
        if (!child.m_Disabled)
          visibleChildrenCount++;
      }

      float defaultSize = visibleChildrenCount > 0 ? totalAvailableWidth / visibleChildrenCount : 0.0f;

      for (auto& child : children) {
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

    for (auto& child : children) {
      if (!child.m_Disabled) {
        totalChildSize += child.m_Size;
        visibleChildrenCount++;
      }
    }

    totalChildSize += (visibleChildrenCount - 1) * splitterWidth;

    if (totalChildSize > availableSize.x) {
      float scaleFactor = availableSize.x / totalChildSize;

      for (auto& child : children) {
        if (!child.m_Disabled) {
          child.m_Size = (std::max)(child.m_Size * scaleFactor, 50.0f);
        }
      }
    } else if (totalChildSize < availableSize.x) {
      float extraSpace = availableSize.x - totalChildSize;

      float totalResizable = 0.0f;
      for (auto& child : children) {
        if (!child.m_Disabled) {
          totalResizable += child.m_Size;
        }
      }

      for (auto& child : children) {
        if (!child.m_Disabled) {
          float ratio = child.m_Size / totalResizable;
          child.m_Size += ratio * extraSpace;
        }
      }
    }

    for (size_t i = 0; i < children.size(); ++i) {
      auto& child = children[i];

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
        vxe::ContentBrowserChild& next_child = children[nextChildIndex];

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
    const std::string fav_id = get_app_window()->m_IdName + "Favorite";
    CherryKit::HeaderImageText(
        CherryID(fav_id), "Favorite", Cherry::GetPath("resources/imgs/icons/misc/icon_star.png"), [this]() {
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
    const std::string main_id = get_app_window()->m_IdName + "Main";
    CherryKit::HeaderImageText(
        CherryID(main_id), "Main", Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"), [this]() {
          draw_hierarchy(base_directory_, true, "Main");
        });

    CherryStyle::RemoveMarginX(6.0f);
    CherryNextComponent.SetProperty("size_x", header_width);
    CherryNextComponent.SetProperty("size_y", 4.0f);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
    const std::string pools_id = get_app_window()->m_IdName + "Pools";
    CherryKit::HeaderImageText(
        CherryID(pools_id),
        "Pools & Collections",
        Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"),
        [this]() {
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

            CherryNextComponent.SetProperty("size_x", "150");
            CherryKit::InputString("Name", &pool_add_name_);

            CherryNextComponent.SetProperty("size_x", "150");
            CherryKit::InputString("Path", &pool_add_path_);

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
            bool poolExists = std::filesystem::exists(pool.first);

            if (!poolExists) {
              CherryStyle::RemoveMarginX(6.0f);

              ImVec2 folderIconPos = CherryGUI::GetCursorScreenPos();
              folderIconPos.x += 6.0f;
              folderIconPos.y += 2.0f;
              draw_folder(
                  poolExists ? get_content_browser_folder_color(pool.first) : "#CC2222", 12.0f, 12.0f, folderIconPos);
              CherryGUI::Dummy(ImVec2(20.0f, 0.0f));
              CherryGUI::SameLine();
            }

            if (!poolExists) {
              CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            }

            if (poolExists) {
              draw_hierarchy(pool.first, true, pool.second);
            } else {
              CherryGUI::TextUnformatted(pool.second.c_str());
              CherryGUI::PopStyleColor();
            }

            CherryGUI::SameLine();
            float available = CherryGUI::GetContentRegionAvail().x;
            CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + available - 20.0f);

            std::string trashId = "##trash_pool_" + pool.first + pool.second;
            CherryNextComponent.SetProperty("color_border", "#00000000");
            CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
            CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
            CherryNextComponent.SetProperty("size_x", "18.0f");
            CherryNextComponent.SetProperty("size_y", "18.0f");
            CherryNextComponent.SetProperty("padding_x", "1.0f");
            CherryNextComponent.SetProperty("padding_y", "1.0f");
            if (CherryKit::ButtonImage(CherryID(trashId), Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"))
                    .GetDataAs<bool>("isClicked")) {
              vxe::remove_pool(pool.first, pool.second);
              refresh_pools();
            }
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

      for (auto& category : categories) {
        CherryKit::SeparatorText(category.first);

        for (auto& ext : category.second) {
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
    if (CherryKit::ButtonImageText(
            GetLocale("loc.window.content.content_browser.save_all"), GetPath("resources/imgs/icons/misc/icon_save.png"))
            .GetDataAs<bool>("isClicked")) {
      vxe::trigger_save_all_event();
    }

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText(
            GetLocale("loc.window.content.content_browser.import"), GetPath("resources/imgs/icons/misc/icon_import.png"))
            .GetDataAs<bool>("isClicked")) {
      begin_import_content();
    }

    CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    CherryGUI::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#444444AA"));
    CherryGUI::Separator();
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 12));

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 1.0f);
    if (back_history_.empty()) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_l_disabled.png"));
    } else {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_l_enabled.png"))
              .GetDataAs<bool>("isClicked") ||
          CherryApp.IsNavigateBackRequested()) {
        go_back();
      }
    }

    if (forward_history_.empty()) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_r_disabled.png"));

    } else {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("size_x", "15.0f");
      CherryNextComponent.SetProperty("size_y", "15.0f");
      if (CherryKit::ButtonImage(GetPath("resources/imgs/icons/misc/icon_arrow_r_enabled.png"))
              .GetDataAs<bool>("isClicked") ||
          CherryApp.IsNavigateForwardRequested()) {
        go_forward();
      }
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
          CherryKit::CheckboxText("Show quick access pannel", &show_side_bar_pannel_);
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
    ImGuiStyle& style = CherryGUI::GetStyle();
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

    draw_delete_confirmation_modal();

    std::vector<std::filesystem::directory_entry> directories;
    std::vector<std::filesystem::directory_entry> files;

    recognized_modules_items_.clear();

    if (!std::filesystem::exists(current_directory_)) {
      current_directory_ = base_directory_;
    }

    for (auto& directoryEntry : std::filesystem::directory_iterator(current_directory_)) {
      bool isItem = false;
      for (auto em : vxe::get_current_context()->IO.em) {
        for (auto item : em->get_content_browser_item_identifiers()) {
          std::string path = directoryEntry.path().string();
          if (item->detection_callback(path)) {
            recognized_modules_items_.push_back({ item, path });
            isItem = true;
          }
        }
      }

      for (auto ep : vxe::get_current_context()->IO.ep) {
        for (auto item : ep->get_content_browser_item_identifiers()) {
          std::string path = directoryEntry.path().string();
          if (item->detection_callback(path)) {
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

    std::sort(directories.begin(), directories.end(), [](const auto& a, const auto& b) {
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

    auto select_all_visible = [&]() {
      selected_.clear();

      if (show_folders_) {
        for (auto& directoryEntry : directories) {
          std::string filenameString = directoryEntry.path().filename().string();

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            selected_.push_back(directoryEntry.path().string());
          }
        }
      }

      if (show_items_) {
        for (auto& itemEntry : recognized_modules_items_) {
          const auto& path = itemEntry.second;
          std::string filenameString = std::filesystem::path(path).filename().string();

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.')
            continue;

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto& ext : extentions_filters_) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }
            if (!match)
              continue;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            selected_.push_back(path);
          }
        }
      }

      if (show_files_) {
        for (auto& fileEntry : files) {
          std::string filenameString = fileEntry.path().filename().string();

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.')
            continue;

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto& ext : extentions_filters_) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(), ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }
            if (!match)
              continue;
          }

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            selected_.push_back(fileEntry.path().string());
          }
        }
      }
    };

    bool aPressed = CherryGUI::IsKeyPressed(ImGuiKey_A);

    if (isWindowFocused && ctrl && aPressed) {
      select_all_visible();
    }

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    if (show_mode_ == ContentShowMode::Thumbmails) {
      CherryGUI::Columns(columnCount, 0, false);

      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

      CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

      CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

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
          begin_import_content();
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
          select_all_visible();
        }

        CherryGUI::GetFont()->Scale = oldfontsize;
        CherryGUI::PopFont();

        CherryGUI::EndPopup();
      }

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(2);

      if (show_folders_) {
        for (auto& directoryEntry : directories) {
          const auto& path = directoryEntry.path();
          std::string filenameString = path.filename().string();

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

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

            float textH = CherryGUI::GetTextLineHeightWithSpacing();
            float nameLines = 1.0f;

            if (selected) {
              ImVec2 cellScreenPos = CherryGUI::GetCursorScreenPos();
              ImVec2 cellSize(availableWidth, reducedthumbnail_size + textH * nameLines + 6.0f);
              ImDrawList* bgDrawList = CherryGUI::GetWindowDrawList();
              bgDrawList->AddRectFilled(
                  cellScreenPos,
                  ImVec2(cellScreenPos.x + cellSize.x, cellScreenPos.y + cellSize.y),
                  IM_COL32(40, 90, 220, 80),
                  4.0f);
            }
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
                  "draw_folder_icon",
                  folderSize,
                  path.string(),
                  current_editing_folder_.first == path.string() ? Cherry::ImU32ToHex(current_editing_folder_.second)
                                                                 : get_content_browser_folder_color(path.string()));
            }

            float oldsize = CherryGUI::GetFont()->Scale;

            CherryGUI::PopStyleVar(2);
            CherryGUI::PopStyleColor(3);

            ImVec2 cursorPos = CherryGUI::GetCursorPos();
            float textWidth = CherryGUI::CalcTextSize(truncate_text(filenameString, reducedthumbnail_size).c_str()).x;
            float textX = folderPosX + (folderSize.x - textWidth) * 0.5f;
            CherryGUI::SetCursorPos(ImVec2(textX + CherryGUI::GetCursorPosX(), cursorPos.y));
            // CherryGUI::TextWrapped(filenameString.c_str());

            ImDrawList* drawList = CherryGUI::GetWindowDrawList();

            ImU32 textColor = IM_COL32(255, 255, 255, 255);
            ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
            ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
            static std::unordered_map<std::string, char[256]> renameBuffers;

            if (path_to_rename_ == path.string()) {
              draw_rename_input(path.string(), reducedthumbnail_size);
            } else {
              draw_truncated_name(
                  drawList, CherryGUI::GetCursorScreenPos(), filenameString, reducedthumbnail_size, textColor);
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
                    clear_cut_selection();
                    clear_copy_selection();
                    cut_paths_callback_(selected_, false);
                    for (auto& path : selected_) {
                      cut_selection_.push_back(path);
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (cut_selection_.size() > 0) {
                bool nothing_new_to_cut = !has_new_paths_to_cut(selected_);
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_cut);
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                  if (cut_paths_callback_) {
                    clear_copy_selection();

                    std::vector<std::string> to_cut;
                    for (auto& path : selected_) {
                      if (!is_in_cut_selection(path)) {
                        to_cut.push_back(path);
                      }
                    }

                    if (!to_cut.empty()) {
                      cut_paths_callback_(to_cut, true);
                      for (auto& path : to_cut) {
                        cut_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              {
                std::string label = "Copy (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
                  if (copy_paths_callback_) {
                    clear_copy_selection();
                    clear_cut_selection();
                    copy_paths_callback_(selected_, false);
                    for (auto& path : selected_) {
                      copy_selection_.push_back(path);
                    }
                  }

                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (copy_selection_.size() > 0) {
                bool nothing_new_to_copy = !has_new_paths_to_copy(selected_);
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_copy);
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                  if (copy_paths_callback_) {
                    clear_cut_selection();
                    std::vector<std::string> to_copy;
                    for (auto& path : selected_) {
                      if (!is_in_copy_selection(path)) {
                        to_copy.push_back(path);
                      }
                    }

                    if (!to_copy.empty()) {
                      copy_paths_callback_(to_copy, true);
                      for (auto& path : to_copy) {
                        copy_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              if (CherryGUI::MenuItem("Delete", "Suppr")) {
                request_delete({ path.string() });
                CherryGUI::CloseCurrentPopup();
              }

              if (selected_.size() > 1) {
                std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
                if (CherryGUI::MenuItem(label.c_str(), "Alt + Suppr")) {
                  request_delete(selected_);
                  CherryGUI::CloseCurrentPopup();
                }
              }
              CherryKit::SeparatorText("Customizations");

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

              draw_folder_customization_menu(path.string());

              CherryGUI::EndPopup();
            }

            CherryGUI::Dummy(ImVec2(reducedthumbnail_size, textH * nameLines));

            CherryGUI::PopID();
            CherryGUI::NextColumn();
          }
        }
      }

      if (show_items_) {
        for (auto& itemEntry : recognized_modules_items_) {
          const auto& path = itemEntry.second;
          std::filesystem::path fsPath(path);
          std::string filenameString = fsPath.filename().string();

          bool selected = false;

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto& ext : extentions_filters_) {
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
              .description = itemEntry.first->description,
              .size = folderSizeString,
              .logo = itemEntry.first->logo_path,
              .item_ident = itemEntry.first,
            };

            ItemCardStyle style{
              .bg_color = IM_COL32(56, 56, 56, 150),
              .border_color = IM_COL32(50, 50, 50, 255),
              .line_color = Cherry::HexToImU32(itemEntry.first->line_color),
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
        for (auto& fileEntry : files) {
          const auto& path = fileEntry.path();
          std::string filenameString = path.filename().string();

          bool selected = false;

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto& ext : extentions_filters_) {
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
            const FileTypeInfo& info = (it != FILE_TYPE_INFOS.end()) ? it->second : DEFAULT_TYPE_INFO;

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

            if (!filenameString.empty() && filenameString[0] == '.') {
              style.bg_color = darken_color(style.bg_color, 0.35f);
            }

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

      if (CherryGUI::IsWindowHovered() && !CherryGUI::IsAnyItemHovered() &&
          CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
        selected_.clear();
        CherryGUI::OpenPopup("EmptySpacePopup");
      }

      CherryGUI::Columns(1);
    }

    if (show_mode_ == ContentShowMode::List) {
      static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV |
                                     ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                                     ImGuiTableFlags_NoSavedSettings;
      const float rowHeight = 26.0f;
      CherryGUI::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.09f, 0.09f, 0.09f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.11f, 0.11f, 0.11f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_TableBorderLight, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_TableBorderStrong, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 3.0f));

      if (CherryGUI::BeginTable("ContentBrowserTable", 4, flags)) {
        CherryGUI::TableSetupColumn(
            "Name",
            ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoReorder,
            1.0f);
        CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        CherryGUI::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        CherryGUI::TableSetupColumn("Last Updated", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        CherryGUI::TableHeadersRow();
        auto drawRow = [&](const std::string& filename,
                           const fs::path& path,
                           bool isFolder,
                           size_t size = 0,
                           std::time_t cftime = 0,
                           ImTextureID tex = nullptr,
                           const std::shared_ptr<ItemIdentifierInterface>& item_ident = nullptr) {
          std::string filenameString = path.filename().string();

          CherryGUI::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
          CherryGUI::TableSetColumnIndex(0);

          ImVec2 cell_pos = CherryGUI::GetCursorScreenPos();
          const float name_col_width = CherryGUI::GetContentRegionAvail().x;
          CherryGUI::PushID(path.string().c_str());

          bool selected = std::find(selected_.begin(), selected_.end(), path.string()) != selected_.end();

          CherryGUI::PushStyleColor(ImGuiCol_Header, ImVec4(0.16f, 0.35f, 0.86f, 0.35f));
          CherryGUI::PushStyleColor(
              ImGuiCol_HeaderHovered, selected ? ImVec4(0.16f, 0.35f, 0.86f, 0.45f) : ImVec4(1.0f, 1.0f, 1.0f, 0.06f));
          CherryGUI::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.16f, 0.35f, 0.86f, 0.55f));

          bool row_clicked = CherryGUI::Selectable(
              ("##row_" + path.string()).c_str(),
              selected,
              ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick,
              ImVec2(0, rowHeight));
          if (path_to_rename_ == path.string()) {
            CherryGUI::SetItemAllowOverlap();
          }

          CherryGUI::PopStyleColor(3);

          if (row_clicked && path_to_rename_ != path.string()) {
            if (CherryGUI::IsMouseDoubleClicked(0)) {
              if (isFolder) {
                change_directory(path);
              }
            } else if (CherryGUI::GetIO().KeyCtrl) {
              auto it = std::find(selected_.begin(), selected_.end(), path.string());
              if (it != selected_.end()) {
                selected_.erase(it);
              } else {
                selected_.push_back(path.string());
              }
            } else {
              selected_.clear();
              selected_.push_back(path.string());
            }
          }

          if (CherryGUI::IsItemHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
            bool already_selected = std::find(selected_.begin(), selected_.end(), path.string()) != selected_.end();
            if (!already_selected) {
              selected_.clear();
              selected_.push_back(path.string());
            }
          }

          item_context_menu_opened_ = false;
          if (!isFolder) {
            bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
            bool alt = CherryApp.IsKeyPressed(CherryKey::ALT);
            bool shift = CherryApp.IsKeyPressed(CherryKey::SHIFT);

            bool shortcutRename = isWindowFocused && ctrl && CherryGUI::IsKeyPressed(ImGuiKey_R);
            bool shortcutCut = isWindowFocused && ctrl && CherryGUI::IsKeyPressed(ImGuiKey_X);
            bool shortcutCutAdd = isWindowFocused && ctrl && alt && CherryApp.IsKeyPressed(CherryKey::X);
            bool shortcutCopy = isWindowFocused && ctrl && CherryApp.IsKeyPressed(CherryKey::C);
            bool shortcutCopyAdd = isWindowFocused && ctrl && alt && CherryApp.IsKeyPressed(CherryKey::C);
            bool shortcutDelete = isWindowFocused && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);
            bool shortcutDeleteMulti = isWindowFocused && alt && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);

            if (shortcutRename && !path_to_rename_.empty()) {
              path_to_rename_ = path.string();
            }

            if (shortcutCut && !selected_.empty()) {
              if (cut_paths_callback_) {
                clear_copy_selection();
                clear_cut_selection();
                cut_paths_callback_(selected_, false);
                for (const auto& path : selected_)
                  cut_selection_.push_back(path);
              }
              selected_.clear();
            }

            if (shortcutCutAdd && !selected_.empty()) {
              if (cut_paths_callback_) {
                clear_copy_selection();

                std::vector<std::string> to_cut;
                for (auto& path : selected_) {
                  if (!is_in_cut_selection(path)) {
                    to_cut.push_back(path);
                  }
                }

                if (!to_cut.empty()) {
                  cut_paths_callback_(to_cut, true);
                  for (auto& path : to_cut) {
                    cut_selection_.push_back(path);
                  }
                }
              }
              selected_.clear();
            }

            if (shortcutCopy && !selected_.empty()) {
              if (copy_paths_callback_) {
                clear_copy_selection();
                clear_cut_selection();
                copy_paths_callback_(selected_, false);
                for (const auto& path : selected_)
                  copy_selection_.push_back(path);
              }
              selected_.clear();
            }

            if (shortcutCopyAdd && !selected_.empty()) {
              if (copy_paths_callback_) {
                clear_cut_selection();

                std::vector<std::string> to_copy;
                for (auto& path : selected_) {
                  if (!is_in_copy_selection(path)) {
                    to_copy.push_back(path);
                  }
                }

                if (!to_copy.empty()) {
                  copy_paths_callback_(to_copy, true);
                  for (auto& path : to_copy) {
                    copy_selection_.push_back(path);
                  }
                }
              }
              selected_.clear();
            }

            if (shortcutDelete && !selected_.empty()) {
              request_delete(selected_);
            }

            if (CherryGUI::BeginPopupContextItem(("context_" + path.string()).c_str())) {
              CherryGUI::GetFont()->Scale = 0.9;
              CherryGUI::PushFont(CherryGUI::GetFont());

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

              item_context_menu_opened_ = true;

              FileTypes fileType = detect_file(path.string());

              if (item_handles_.empty()) {
                if (item_ident) {
                  item_handles_ = vxe::get_all_item_handlers_for(item_ident->name);
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
                  itepath_s_.push_back({ path.string(), item_ident->line_color });
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (CherryGUI::MenuItem(
                      "Copy",
                      "Ctrl + C",
                      Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")),
                      NULL)) {
                if (copy_paths_callback_) {
                  clear_copy_selection();
                  clear_cut_selection();
                  copy_paths_callback_(selected_, false);
                  for (auto& path : selected_) {
                    copy_selection_.push_back(path);
                  }
                }

                selected_.clear();
                CherryGUI::CloseCurrentPopup();
              }

              if (copy_selection_.size() > 0) {
                bool nothing_new_to_copy = !has_new_paths_to_copy(selected_);
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_copy);
                if (CherryGUI::MenuItem(
                        label.c_str(),
                        "Ctrl + Alt + C",
                        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")),
                        NULL)) {
                  if (copy_paths_callback_) {
                    clear_cut_selection();
                    std::vector<std::string> to_copy;
                    for (auto& path : selected_) {
                      if (!is_in_copy_selection(path)) {
                        to_copy.push_back(path);
                      }
                    }

                    if (!to_copy.empty()) {
                      copy_paths_callback_(to_copy, true);
                      for (auto& path : to_copy) {
                        copy_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              if (CherryGUI::MenuItem(
                      "Cut",
                      "Ctrl + X",
                      Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")),
                      NULL)) {
                if (cut_paths_callback_) {
                  clear_cut_selection();
                  clear_copy_selection();
                  cut_paths_callback_(selected_, false);
                  for (auto& path : selected_) {
                    cut_selection_.push_back(path);
                  }
                }

                selected_.clear();
                CherryGUI::CloseCurrentPopup();
              }

              if (cut_selection_.size() > 0) {
                bool nothing_new_to_cut = !has_new_paths_to_cut(selected_);
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_cut);
                if (CherryGUI::MenuItem(
                        label.c_str(),
                        "Ctrl + Alt + X",
                        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")),
                        NULL)) {
                  if (cut_paths_callback_) {
                    clear_copy_selection();

                    std::vector<std::string> to_cut;
                    for (auto& path : selected_) {
                      if (!is_in_cut_selection(path)) {
                        to_cut.push_back(path);
                      }
                    }

                    if (!to_cut.empty()) {
                      cut_paths_callback_(to_cut, true);
                      for (auto& path : to_cut) {
                        cut_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
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
          } else if (filename != "...") {
            if (CherryGUI::BeginPopupContextItem(("context_" + path.string()).c_str())) {
              CherryKit::SeparatorText("Main");

              if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
                path_to_rename_ = path.string();
              }

              {
                std::string label = "Cut (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + X")) {
                  if (cut_paths_callback_) {
                    clear_cut_selection();
                    clear_copy_selection();
                    cut_paths_callback_(selected_, false);
                    for (auto& path : selected_) {
                      cut_selection_.push_back(path);
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
              }
              if (cut_selection_.size() > 0) {
                bool nothing_new_to_cut = !has_new_paths_to_cut(selected_);
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_cut);
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                  if (cut_paths_callback_) {
                    clear_copy_selection();

                    std::vector<std::string> to_cut;
                    for (auto& path : selected_) {
                      if (!is_in_cut_selection(path)) {
                        to_cut.push_back(path);
                      }
                    }

                    if (!to_cut.empty()) {
                      cut_paths_callback_(to_cut, true);
                      for (auto& path : to_cut) {
                        cut_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              {
                std::string label = "Copy (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
                  if (copy_paths_callback_) {
                    clear_copy_selection();
                    clear_cut_selection();
                    copy_paths_callback_(selected_, false);
                    for (auto& path : selected_) {
                      copy_selection_.push_back(path);
                    }
                  }

                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (copy_selection_.size() > 0) {
                bool nothing_new_to_copy = !has_new_paths_to_copy(selected_);
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_copy);
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                  if (copy_paths_callback_) {
                    clear_cut_selection();
                    std::vector<std::string> to_copy;
                    for (auto& path : selected_) {
                      if (!is_in_copy_selection(path)) {
                        to_copy.push_back(path);
                      }
                    }

                    if (!to_copy.empty()) {
                      copy_paths_callback_(to_copy, true);
                      for (auto& path : to_copy) {
                        copy_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              if (CherryGUI::MenuItem("Delete", "Suppr")) {
                request_delete({ path.string() });
                CherryGUI::CloseCurrentPopup();
              }

              if (selected_.size() > 1) {
                std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
                if (CherryGUI::MenuItem(label.c_str(), "Alt + Suppr")) {
                  request_delete(selected_);
                  CherryGUI::CloseCurrentPopup();
                }
              }
              CherryKit::SeparatorText("Customizations");

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

              draw_folder_customization_menu(path.string());

              CherryGUI::EndPopup();
            }
          }

          if (!item_context_menu_opened_) {
            item_handles_.clear();
          }

          const float icon_size = 18.0f;
          const ImVec4 muted(0.55f, 0.55f, 0.55f, 1.0f);
          const float text_y = cell_pos.y + (rowHeight - CherryGUI::GetTextLineHeight()) * 0.5f;
          const ImVec2 icon_pos(cell_pos.x + 4.0f, cell_pos.y + (rowHeight - icon_size) * 0.5f);
          const ImVec2 name_pos(icon_pos.x + icon_size + 8.0f, text_y);
          const float name_max_width = name_col_width - (name_pos.x - cell_pos.x) - 6.0f;

          auto align_text = [&]() { CherryGUI::SetCursorScreenPos(ImVec2(CherryGUI::GetCursorScreenPos().x, text_y)); };

          CherryGUI::SetCursorScreenPos(icon_pos);

          if (!isFolder) {
            FileTypes fileType = detect_file(path.string());

            if (item_ident) {
              tex = Cherry::GetTexture(item_ident->logo_path);
            } else {
              auto it = FILE_ICONS.find(fileType);
              if (it != FILE_ICONS.end()) {
                tex = Cherry::GetTexture(Cherry::GetPath(it->second));
              } else {
                tex = Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/files/icon_unknow_file.png"));
              }
            }

            if (tex) {
              CherryGUI::Image(tex, ImVec2(icon_size, icon_size));
            }
          } else {
            draw_folder_button(
                "draw_folder_icon",
                ImVec2(icon_size, icon_size),
                path.string(),
                current_editing_folder_.first == path.string() ? Cherry::ImU32ToHex(current_editing_folder_.second)
                                                               : get_content_browser_folder_color(path.string()));
          }

          ImU32 textColor = IM_COL32(255, 255, 255, 255);
          ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
          ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

          if (path_to_rename_ == path.string()) {
            CherryGUI::SetCursorScreenPos(ImVec2(name_pos.x, cell_pos.y + (rowHeight - CherryGUI::GetFrameHeight()) * 0.5f));
            draw_rename_input(path.string(), CherryGUI::GetContentRegionAvail().x);
          } else {
            if (filename == "...") {
              filenameString = "...";
            }
            draw_truncated_name(CherryGUI::GetWindowDrawList(), name_pos, filenameString, name_max_width, textColor);
          }

          CherryGUI::TableSetColumnIndex(1);
          align_text();
          if (item_ident) {
            CherryGUI::TextColored(muted, "%s", item_ident->description.c_str());
          } else {
            CherryGUI::TextColored(muted, "%s", isFolder ? "Folder" : "File");
          }

          std::string size_label = "---";
          if (filename != "...") {
            if (isFolder || item_ident) {
              size_label = get_folder_size_cached(path.string());
            } else {
              size_label = format_file_size(size);
            }
          }

          CherryGUI::TableSetColumnIndex(2);
          align_text();
          CherryGUI::TextUnformatted(size_label.c_str());

          CherryGUI::TableSetColumnIndex(3);
          align_text();
          if (cftime != 0) {
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&cftime));
            CherryGUI::TextColored(muted, "%s", buffer);
          } else {
            CherryGUI::TextColored(muted, "---");
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
          for (auto& directoryEntry : directories) {
            const auto& path = directoryEntry.path();
            std::string filename = path.filename().string();

            if (!show_hidden_ && !filename.empty() && filename[0] == '.') {
              continue;
            }

            if (!are_strings_similar(filename, project_search_, threshold_) &&
                !is_only_spaces_or_empty(project_search_.c_str()))
              continue;

            ImTextureID tex = nullptr;
            FileTypes type = detect_file(path.string());
            switch (type) {
              case FileTypes::File_PICTURE:
                tex = Application::GetCurrentRenderedWindow()->GetTexture(
                    Cherry::GetPath("resources/imgs/icons/files/icon_picture_file.png"));
                break;
              default:
                tex = Application::GetCurrentRenderedWindow()->GetTexture(
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
          for (auto& itemEntry : recognized_modules_items_) {
            const auto& path = itemEntry.second;
            std::filesystem::path fsPath(path);
            std::filesystem::directory_entry fsDirectoryEntry(path);
            std::string filenameString = fsPath.filename().string();

            if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
              continue;
            }

            if (!extentions_filters_.empty()) {
              bool match = false;
              for (const auto& ext : extentions_filters_) {
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

            ImTextureID tex = Application::GetCurrentRenderedWindow()->GetTexture(
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
          for (auto& fileEntry : files) {
            const auto& path = fileEntry.path();
            std::string filename = path.filename().string();

            if (!show_hidden_ && !filename.empty() && filename[0] == '.') {
              continue;
            }

            if (!extentions_filters_.empty()) {
              bool match = false;
              for (const auto& ext : extentions_filters_) {
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

            ImTextureID tex = Application::GetCurrentRenderedWindow()->GetTexture(
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

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(5);
    }

    if (show_mode_ == ContentShowMode::Objects) {
      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

      CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);

      CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

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
          begin_import_content();
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
          select_all_visible();
        }

        CherryGUI::GetFont()->Scale = oldfontsize;
        CherryGUI::PopFont();

        CherryGUI::EndPopup();
      }

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(2);

      if (show_folders_) {
        for (auto& directoryEntry : directories) {
          const auto& path = directoryEntry.path();
          std::string filenameString = path.filename().string();

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          bool selected = (std::find(selected_.begin(), selected_.end(), path) != selected_.end());

          if (are_strings_similar(filenameString, project_search_, threshold_) ||
              is_only_spaces_or_empty(project_search_.c_str())) {
            CherryGUI::PushID(filenameString.c_str());

            float padding = 10.0f;
            float logoSize = 36.0f;
            float borderRadius = 6.0f;

            float availableWidth = CherryGUI::GetContentRegionAvail().x;
            float cardHeight = logoSize + padding * 2;
            ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();
            ImVec2 cardSize(availableWidth, cardHeight);
            bool is_renaming = path_to_rename_ == path.string();

            CherryGUI::InvisibleButton(("folder_card_" + filenameString).c_str(), cardSize);

            if (is_renaming) {
              CherryGUI::SetItemAllowOverlap();
            }
            bool hovered = CherryGUI::IsItemHovered();

            if (hovered && !is_renaming) {
              CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);

              if (CherryGUI::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                change_directory(path);
              } else if (CherryGUI::IsMouseClicked(ImGuiMouseButton_Left)) {
                select_on_click(path.string());
              } else if (CherryGUI::IsMouseClicked(ImGuiMouseButton_Right) && !selected) {
                selected_.clear();
                selected_.push_back(path.string());
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
                    clear_cut_selection();
                    clear_copy_selection();
                    cut_paths_callback_(selected_, false);
                    for (auto& path : selected_) {
                      cut_selection_.push_back(path);
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (cut_selection_.size() > 0) {
                bool nothing_new_to_cut = !has_new_paths_to_cut(selected_);
                std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_cut);
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                  if (cut_paths_callback_) {
                    clear_copy_selection();

                    std::vector<std::string> to_cut;
                    for (auto& path : selected_) {
                      if (!is_in_cut_selection(path)) {
                        to_cut.push_back(path);
                      }
                    }

                    if (!to_cut.empty()) {
                      cut_paths_callback_(to_cut, true);
                      for (auto& path : to_cut) {
                        cut_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              {
                std::string label = "Copy (" + std::to_string(selected_.size()) + ") selected";

                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + C")) {
                  if (copy_paths_callback_) {
                    clear_copy_selection();
                    clear_cut_selection();
                    copy_paths_callback_(selected_, false);
                    for (auto& path : selected_) {
                      copy_selection_.push_back(path);
                    }
                  }

                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
              }

              if (copy_selection_.size() > 0) {
                bool nothing_new_to_copy = !has_new_paths_to_copy(selected_);
                std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";

                CherryGUI::BeginDisabled(nothing_new_to_copy);
                if (CherryGUI::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                  if (copy_paths_callback_) {
                    clear_cut_selection();
                    std::vector<std::string> to_copy;
                    for (auto& path : selected_) {
                      if (!is_in_copy_selection(path)) {
                        to_copy.push_back(path);
                      }
                    }

                    if (!to_copy.empty()) {
                      copy_paths_callback_(to_copy, true);
                      for (auto& path : to_copy) {
                        copy_selection_.push_back(path);
                      }
                    }
                  }
                  selected_.clear();
                  CherryGUI::CloseCurrentPopup();
                }
                CherryGUI::EndDisabled();
              }

              if (CherryGUI::MenuItem("Delete", "Suppr")) {
                request_delete({ path.string() });
                CherryGUI::CloseCurrentPopup();
              }

              if (selected_.size() > 1) {
                std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
                if (CherryGUI::MenuItem(label.c_str(), "Alt + Suppr")) {
                  request_delete(selected_);
                  CherryGUI::CloseCurrentPopup();
                }
              }

              CherryKit::SeparatorText("Customizations");

              CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

              draw_folder_customization_menu(path.string());

              CherryGUI::EndPopup();
            }

            ImU32 bgColor =
                selected ? IM_COL32(40, 90, 220, 80) : (hovered ? IM_COL32(52, 52, 52, 255) : IM_COL32(40, 40, 40, 255));
            ImU32 borderColor =
                selected ? IM_COL32(90, 130, 255, 255) : (hovered ? IM_COL32(80, 80, 80, 255) : IM_COL32(60, 60, 60, 255));
            std::string folder_color =
                current_editing_folder_.first == path.string()   ? Cherry::ImU32ToHex(current_editing_folder_.second)
                : current_editing_folder_.first == path.string() ? Cherry::ImU32ToHex(current_editing_folder_.second)
                                                                 : get_content_browser_folder_color(path.string());
            ImU32 accentColor = Cherry::HexToImU32(folder_color);

            ImDrawList* drawList = CherryGUI::GetWindowDrawList();
            ImVec2 cardMax(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y);

            drawList->AddRectFilled(cursorPos, cardMax, bgColor, borderRadius);
            drawList->AddRect(cursorPos, cardMax, borderColor, borderRadius);
            drawList->AddRectFilled(
                cursorPos, ImVec2(cursorPos.x + 4.0f, cardMax.y), accentColor, borderRadius, ImDrawFlags_RoundCornersLeft);

            ImVec2 logoPos(cursorPos.x + padding + 4.0f, cursorPos.y + (cardHeight - logoSize) * 0.5f);
            CherryGUI::SetCursorScreenPos(logoPos);
            draw_folder_button(
                "draw_folder_icon",
                ImVec2(logoSize, logoSize),
                path.string(),
                current_editing_folder_.first == path.string() ? Cherry::ImU32ToHex(current_editing_folder_.second)
                                                               : get_content_browser_folder_color(path.string()));

            float logoEndX = logoPos.x + logoSize;

            std::string sizeLabel = get_folder_size_cached(path.string());
            float sizeWidth = CherryGUI::CalcTextSize(sizeLabel.c_str()).x;
            float textH = CherryGUI::GetTextLineHeight();
            float textMaxWidth = cardMax.x - logoEndX - padding * 3.0f - sizeWidth;

            ImVec2 textPos(logoEndX + padding, cursorPos.y + (cardHeight - (textH * 2.0f + 2.0f)) * 0.5f);

            ImU32 textColor = IM_COL32(255, 255, 255, 255);
            ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
            ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

            if (is_renaming) {
              CherryGUI::SetCursorScreenPos(textPos);
              draw_rename_input(path.string(), textMaxWidth);
            } else {
              draw_truncated_name(drawList, textPos, filenameString, textMaxWidth, textColor);
            }

            drawList->AddText(ImVec2(textPos.x, textPos.y + textH + 2.0f), IM_COL32(140, 140, 140, 255), "Folder");
            drawList->AddText(
                ImVec2(cardMax.x - padding - sizeWidth, cursorPos.y + (cardHeight - textH) * 0.5f),
                IM_COL32(170, 170, 170, 255),
                sizeLabel.c_str());

            CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + 6.0f));

            CherryGUI::PopID();
          }
        }
      }

      if (show_items_) {
        for (auto& itemEntry : recognized_modules_items_) {
          const auto& path = itemEntry.second;
          std::filesystem::path fsPath(path);
          std::string filenameString = fsPath.filename().string();

          bool selected = false;

          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto& ext : extentions_filters_) {
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
            CherryGUI::PushID(filenameString.c_str());

            const auto& ident = itemEntry.first;

            if (draw_object_card(
                    path,
                    filenameString,
                    ident->description,
                    get_folder_size_cached(path),
                    ident->logo_path,
                    Cherry::HexToImU32(ident->line_color),
                    selected,
                    ident)) {
              select_on_click(path);
            }

            CherryGUI::PopID();
          }
        }
      }

      if (show_files_) {
        for (auto& fileEntry : files) {
          const auto& path = fileEntry.path();
          std::string filenameString = path.filename().string();

          bool selected = false;
          if (!show_hidden_ && !filenameString.empty() && filenameString[0] == '.') {
            continue;
          }

          if (!extentions_filters_.empty()) {
            bool match = false;
            for (const auto& ext : extentions_filters_) {
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

            CherryGUI::PushID(filenameString.c_str());

            FileTypes fileType = detect_file(path.string());

            auto it = FILE_TYPE_INFOS.find(fileType);
            const FileTypeInfo& info = (it != FILE_TYPE_INFOS.end()) ? it->second : DEFAULT_TYPE_INFO;

            if (draw_object_card(
                    path.string(),
                    filenameString,
                    info.label,
                    format_file_size(fileSize),
                    Cherry::GetPath(info.icon),
                    Cherry::HexToImU32(info.color),
                    selected)) {
              select_on_click(path.string());
            }

            CherryGUI::PopID();
          }
        }
      }

      if (CherryGUI::IsWindowHovered() && !CherryGUI::IsAnyItemHovered() &&
          CherryGUI::IsMouseReleased(ImGuiMouseButton_Left)) {
        selected_.clear();
      }

      if (CherryGUI::IsWindowHovered() && !CherryGUI::IsAnyItemHovered() &&
          CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
        selected_.clear();
        CherryGUI::OpenPopup("EmptySpacePopup");
      }
    }
  }

  std::string ContentBrowser::get_folder_size_cached(const std::string& path) {
    double now = CherryGUI::GetTime();
    auto it = folder_size_cache_.find(path);

    if (it != folder_size_cache_.end() && now - it->second.first < 3.0) {
      return it->second.second;
    }

    std::string size = format_file_size(get_directory_size(path));
    folder_size_cache_[path] = { now, size };

    return size;
  }

  void ContentBrowser::add_child(const ContentBrowserChild& child) {
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
    new_win->set_create_item_callback(
        [this](const std::function<void(const std::string&)>& creator) { this->create_item(creator); });
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

  void ContentBrowser::request_delete(const std::vector<std::string>& paths) {
    if (paths.empty())
      return;

    pending_delete_paths_ = paths;
    request_open_delete_modal_ = true;
  }

  void ContentBrowser::draw_delete_confirmation_modal() {
    if (request_open_delete_modal_) {
      CherryGUI::OpenPopup("Confirm Delete##content_browser_delete_confirm");
      request_open_delete_modal_ = false;
    }

    CherryGUI::SetNextWindowSize(ImVec2(420, 0), ImGuiCond_Appearing);

    if (CherryGUI::BeginPopupModal(
            "Confirm Delete##content_browser_delete_confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      CherryGUI::Text(
          "Are you sure you want to delete %zu item%s?",
          pending_delete_paths_.size(),
          pending_delete_paths_.size() > 1 ? "s" : "");

      CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "This action cannot be undone.");
      CherryGUI::Separator();

      CherryGUI::BeginChild(
          "DeletePreviewList", ImVec2(400, (std::min)(150.0f, pending_delete_paths_.size() * 20.0f + 10.0f)), true);
      for (auto& p : pending_delete_paths_) {
        std::filesystem::path fp(p);
        bool isDir = std::filesystem::is_directory(fp);
        CherryGUI::TextUnformatted((isDir ? "[dir] " : "[file] "));
        CherryGUI::SameLine();
        CherryGUI::TextUnformatted(fp.filename().string().c_str());
      }
      CherryGUI::EndChild();

      CherryGUI::Spacing();

      float buttonWidth = 120.0f;
      float totalWidth = buttonWidth * 2 + 10.0f;
      CherryGUI::SetCursorPosX((CherryGUI::GetWindowWidth() - totalWidth) * 0.5f);

      if (CherryGUI::Button("Cancel", ImVec2(buttonWidth, 0))) {
        pending_delete_paths_.clear();
        CherryGUI::CloseCurrentPopup();
      }

      CherryGUI::SameLine();

      CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.15f, 0.15f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.2f, 0.2f, 1.0f));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

      if (CherryGUI::Button("Delete", ImVec2(buttonWidth, 0))) {
        if (delete_path_callback_) {
          for (auto& p : pending_delete_paths_) {
            delete_path_callback_(p);
          }
        }
        selected_.clear();
        pending_delete_paths_.clear();
        CherryGUI::CloseCurrentPopup();
      }

      CherryGUI::PopStyleColor(3);

      CherryGUI::EndPopup();
    }
  }

  void ContentBrowser::draw_rename_input(const std::string& path, float width) {
    if (rename_initialized_path_ != path) {
      std::string name = std::filesystem::path(path).filename().string();
      std::strncpy(rename_buffer_, name.c_str(), sizeof(rename_buffer_) - 1);
      rename_buffer_[sizeof(rename_buffer_) - 1] = '\0';
      rename_initialized_path_ = path;
      CherryGUI::SetKeyboardFocusHere();
    }

    CherryGUI::PushID(path.c_str());
    CherryGUI::SetNextItemWidth(width);
    bool entered = CherryGUI::InputText(
        "##rename_input",
        rename_buffer_,
        sizeof(rename_buffer_),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
    bool hovered = CherryGUI::IsItemHovered();
    CherryGUI::PopID();

    bool cancelled = CherryGUI::IsKeyPressed(ImGuiKey_Escape);
    bool clicked_outside =
        !hovered && (CherryGUI::IsMouseClicked(ImGuiMouseButton_Left) || CherryGUI::IsMouseClicked(ImGuiMouseButton_Right));

    if (!cancelled && !entered && !clicked_outside) {
      return;
    }

    std::string new_name = rename_buffer_;
    std::string old_name = std::filesystem::path(path).filename().string();

    if (!cancelled && !new_name.empty() && new_name != old_name) {
      vxe::submit_rename(path, new_name);
      refresh_custom_folders();
      selected_.clear();
    }

    path_to_rename_.clear();
    rename_initialized_path_.clear();
  }

  bool ContentBrowser::draw_object_card(
      const std::string& path,
      const std::string& name,
      const std::string& subtitle,
      const std::string& size_label,
      const std::string& logo_path,
      ImU32 accent,
      bool selected,
      const std::shared_ptr<ItemIdentifierInterface>& item_ident) {
    const float padding = 10.0f;
    const float logo_size = 36.0f;
    const float border_radius = 6.0f;
    const bool dimmed = !name.empty() && name[0] == '.';
    const bool is_renaming = path_to_rename_ == path;

    float width = CherryGUI::GetContentRegionAvail().x;
    float height = logo_size + padding * 2;
    ImVec2 pos = CherryGUI::GetCursorScreenPos();
    ImVec2 max(pos.x + width, pos.y + height);

    CherryGUI::InvisibleButton("##object_card", ImVec2(width, height));
    if (is_renaming) {
      CherryGUI::SetItemAllowOverlap();
    }
    bool hovered = CherryGUI::IsItemHovered();
    bool clicked = hovered && !is_renaming && CherryGUI::IsMouseClicked(ImGuiMouseButton_Left);

    if (hovered && !is_renaming) {
      CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);

      if (CherryGUI::IsMouseClicked(ImGuiMouseButton_Right) && !selected) {
        selected_.clear();
        selected_.push_back(path);
        selected = true;
      }
    }

    if (!is_renaming && CherryGUI::BeginPopupContextItem("##object_card_context")) {
      draw_item_context_menu(path, item_ident);
      CherryGUI::EndPopup();
    }

    ImU32 bg_color =
        selected ? IM_COL32(40, 90, 220, 80) : (hovered ? IM_COL32(52, 52, 52, 255) : IM_COL32(40, 40, 40, 255));
    ImU32 border_color =
        selected ? IM_COL32(90, 130, 255, 255) : (hovered ? IM_COL32(80, 80, 80, 255) : IM_COL32(60, 60, 60, 255));

    ImDrawList* drawList = CherryGUI::GetWindowDrawList();

    drawList->AddRectFilled(pos, max, bg_color, border_radius);
    drawList->AddRect(pos, max, border_color, border_radius);
    drawList->AddRectFilled(pos, ImVec2(pos.x + 4.0f, max.y), accent, border_radius, ImDrawFlags_RoundCornersLeft);

    ImVec2 logo_pos(pos.x + padding + 4.0f, pos.y + (height - logo_size) * 0.5f);
    ImTextureID tex = Cherry::GetTexture(logo_path);
    if (tex) {
      drawList->AddImage(
          tex,
          logo_pos,
          ImVec2(logo_pos.x + logo_size, logo_pos.y + logo_size),
          ImVec2(0, 0),
          ImVec2(1, 1),
          dimmed ? IM_COL32(255, 255, 255, 120) : IM_COL32(255, 255, 255, 255));
    }

    float text_h = CherryGUI::GetTextLineHeight();
    float size_width = CherryGUI::CalcTextSize(size_label.c_str()).x;
    float text_x = logo_pos.x + logo_size + padding;
    float text_max_width = max.x - text_x - padding * 2.0f - size_width;
    ImVec2 text_pos(text_x, pos.y + (height - (text_h * 2.0f + 2.0f)) * 0.5f);

    ImU32 text_color = dimmed ? IM_COL32(255, 255, 255, 140) : IM_COL32(255, 255, 255, 255);

    if (is_renaming) {
      CherryGUI::SetCursorScreenPos(ImVec2(text_x, pos.y + (height - CherryGUI::GetFrameHeight()) * 0.5f));
      draw_rename_input(path, text_max_width);
    } else {
      draw_truncated_name(drawList, text_pos, name, text_max_width, text_color);
      drawList->AddText(
          ImVec2(text_pos.x, text_pos.y + text_h + 2.0f),
          IM_COL32(140, 140, 140, 255),
          truncate_text(subtitle, text_max_width).c_str());
    }

    drawList->AddText(
        ImVec2(max.x - padding - size_width, pos.y + (height - text_h) * 0.5f),
        IM_COL32(170, 170, 170, 255),
        size_label.c_str());

    CherryGUI::SetCursorScreenPos(ImVec2(pos.x, max.y + 6.0f));

    return clicked;
  }

  void ContentBrowser::draw_item_context_menu(
      const std::string& path,
      const std::shared_ptr<ItemIdentifierInterface>& item_ident) {
    float oldfontsize = CherryGUI::GetFont()->Scale;

    CherryGUI::GetFont()->Scale = 0.9;
    CherryGUI::PushFont(CherryGUI::GetFont());

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);

    auto handlers = item_ident ? vxe::get_all_item_handlers_for(item_ident->name)
                               : vxe::get_all_item_handlers_for(get_file_type_str(detect_file(path)));

    if (!handlers.empty()) {
      CherryKit::SeparatorText("Actions");
    }

    for (auto ih : handlers) {
      if (CherryGUI::MenuItem(ih->title.c_str(), ih->description.c_str(), Cherry::GetTexture(ih->logo), false)) {
        ih->handler(path);
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
        itepath_s_.push_back({ path, item_ident->line_color });
        CherryGUI::CloseCurrentPopup();
      }
    }

    if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
      path_to_rename_ = path;
    }

    if (CherryGUI::MenuItem(
            "Copy", "Ctrl + C", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")), NULL)) {
      if (copy_paths_callback_) {
        clear_copy_selection();
        clear_cut_selection();
        copy_paths_callback_(selected_, false);
        for (auto& selected : selected_) {
          copy_selection_.push_back(selected);
        }
      }

      selected_.clear();
      CherryGUI::CloseCurrentPopup();
    }

    if (copy_selection_.size() > 0) {
      bool nothing_new_to_copy = !has_new_paths_to_copy(selected_);
      std::string label = "Copy in addition (" + std::to_string(copy_selection_.size()) + " copies)";

      CherryGUI::BeginDisabled(nothing_new_to_copy);
      if (CherryGUI::MenuItem(
              label.c_str(),
              "Ctrl + Alt + C",
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")),
              NULL)) {
        if (copy_paths_callback_) {
          clear_cut_selection();
          std::vector<std::string> to_copy;
          for (auto& selected : selected_) {
            if (!is_in_copy_selection(selected)) {
              to_copy.push_back(selected);
            }
          }

          if (!to_copy.empty()) {
            copy_paths_callback_(to_copy, true);
            for (auto& selected : to_copy) {
              copy_selection_.push_back(selected);
            }
          }
        }
        selected_.clear();
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::EndDisabled();
    }

    if (CherryGUI::MenuItem(
            "Cut", "Ctrl + X", Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")), NULL)) {
      if (cut_paths_callback_) {
        clear_cut_selection();
        clear_copy_selection();
        cut_paths_callback_(selected_, false);
        for (auto& selected : selected_) {
          cut_selection_.push_back(selected);
        }
      }

      selected_.clear();
      CherryGUI::CloseCurrentPopup();
    }

    if (cut_selection_.size() > 0) {
      bool nothing_new_to_cut = !has_new_paths_to_cut(selected_);
      std::string label = "Cut in addition (" + std::to_string(cut_selection_.size()) + " copies)";

      CherryGUI::BeginDisabled(nothing_new_to_cut);
      if (CherryGUI::MenuItem(
              label.c_str(),
              "Ctrl + Alt + X",
              Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_cut.png")),
              NULL)) {
        if (cut_paths_callback_) {
          clear_copy_selection();

          std::vector<std::string> to_cut;
          for (auto& selected : selected_) {
            if (!is_in_cut_selection(selected)) {
              to_cut.push_back(selected);
            }
          }

          if (!to_cut.empty()) {
            cut_paths_callback_(to_cut, true);
            for (auto& selected : to_cut) {
              cut_selection_.push_back(selected);
            }
          }
        }
        selected_.clear();
        CherryGUI::CloseCurrentPopup();
      }
      CherryGUI::EndDisabled();
    }

    if (CherryGUI::MenuItem("Delete", "Suppr")) {
      request_delete({ path });
      CherryGUI::CloseCurrentPopup();
    }

    if (selected_.size() > 1) {
      std::string label = "Delete (" + std::to_string(selected_.size()) + " selected)";
      if (CherryGUI::MenuItem(label.c_str(), "Alt + Suppr")) {
        request_delete(selected_);
        CherryGUI::CloseCurrentPopup();
      }
    }

    CherryKit::SeparatorText("Customizations");

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 2.0f);

    draw_folder_customization_menu(path);

    CherryGUI::EndPopup();
  }
  void ContentBrowser::draw_folder_customization_menu(const std::string& path) {
    static bool EditingColor = false;

    current_editing_folder_is_favorite_ = is_path_favorite(path);

    if (CherryGUI::BeginMenu("Change color")) {
      if (!EditingColor) {
        current_editing_folder_ = { path, HexToImU32(get_content_browser_folder_color(path)) };
      }

      EditingColor = true;

      if (draw_color_picker_3U32("MyColor", &current_editing_folder_.second)) {
        CherryGUI::CloseCurrentPopup();
      }

      CherryGUI::EndMenu();
    } else {
      EditingColor = false;
      current_editing_folder_ = { "", ImU32() };
    }

    if (CherryGUI::MenuItem("Mark as favorite", "", current_editing_folder_is_favorite_)) {
      current_editing_folder_ = { path, HexToImU32(get_content_browser_folder_color(path)) };

      current_editing_folder_is_favorite_ = !current_editing_folder_is_favorite_;
      set_colored_folder(current_editing_folder_.first, ImU32ToHex(current_editing_folder_.second));
      vxe::publish_content_browser_custom_folder(
          current_editing_folder_.first, ImU32ToHex(current_editing_folder_.second), current_editing_folder_is_favorite_);

      if (current_editing_folder_is_favorite_) {
        favorite_folders_.push_back(current_editing_folder_.first);
      } else {
        auto it = std::find(favorite_folders_.begin(), favorite_folders_.end(), current_editing_folder_.first);
        if (it != favorite_folders_.end()) {
          favorite_folders_.erase(it);
        }
      }
    }
  }
  std::string ContentBrowser::truncate_text(const std::string& text, float max_width) {
    if (max_width <= 0.0f) {
      return "";
    }

    if (CherryGUI::CalcTextSize(text.c_str()).x <= max_width) {
      return text;
    }

    const std::string suffix = "[...]";
    float suffix_width = CherryGUI::CalcTextSize(suffix.c_str()).x;

    if (suffix_width >= max_width) {
      return suffix;
    }

    size_t len = text.size();
    while (len > 0 && CherryGUI::CalcTextSize(text.c_str(), text.c_str() + len).x + suffix_width > max_width) {
      --len;
      while (len > 0 && (static_cast<unsigned char>(text[len]) & 0xC0) == 0x80) {
        --len;
      }
    }

    return text.substr(0, len) + suffix;
  }

  void ContentBrowser::draw_truncated_name(
      ImDrawList* drawList,
      ImVec2 pos,
      const std::string& name,
      float max_width,
      ImU32 color) {
    std::string shown = truncate_text(name, max_width);

    if (shown.empty()) {
      return;
    }

    const std::string suffix = "[...]";
    const bool truncated = shown != name;
    std::string head = truncated ? shown.substr(0, shown.size() - suffix.size()) : shown;

    if (!head.empty()) {
      draw_highlighted_text_(
          drawList, pos, head.c_str(), project_search_.c_str(), IM_COL32(255, 255, 0, 255), color, IM_COL32(0, 0, 0, 255));
    }

    if (!truncated) {
      return;
    }

    float head_width = head.empty() ? 0.0f : CherryGUI::CalcTextSize(head.c_str()).x;
    drawList->AddText(ImVec2(pos.x + head_width, pos.y), IM_COL32(120, 120, 120, 255), suffix.c_str());

    ImVec2 max(pos.x + CherryGUI::CalcTextSize(shown.c_str()).x, pos.y + CherryGUI::GetTextLineHeight());
    if (CherryGUI::IsMouseHoveringRect(pos, max) && CherryGUI::IsWindowHovered()) {
      CherryGUI::SetTooltip("%s", name.c_str());
    }
  }
}  // namespace vxe