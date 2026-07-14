//
//  add_window.cpp
//  Sources for the "add window" of the content browser
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./add_window.hpp"

namespace vxe {

  namespace {

    struct TileEntry {
      std::string id;
      ImTextureID icon;
      std::string title;
      bool enabled = true;
      std::function<void()> on_click;
    };

    bool MatchesSearch(const std::string &title, const std::string &search) {
      if (search.empty()) {
        return true;
      }
      std::string lowerTitle = title;
      std::string lowerSearch = search;
      std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
      std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
      return lowerTitle.find(lowerSearch) != std::string::npos;
    }

    bool DrawTile(const char *id, ImTextureID icon, const std::string &title, float w, float h, bool enabled) {
      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      bool clicked = false;
      bool hovered = false;

      if (enabled) {
        clicked = CherryGUI::InvisibleButton(id, ImVec2(w, h));
        hovered = CherryGUI::IsItemHovered();
        if (hovered) {
          CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
      } else {
        CherryGUI::Dummy(ImVec2(w, h));
      }

      ImU32 bg = enabled ? (hovered ? IM_COL32(58, 58, 58, 255) : IM_COL32(40, 40, 40, 255)) : IM_COL32(30, 30, 30, 255);
      ImU32 border =
          enabled ? (hovered ? IM_COL32(90, 130, 220, 255) : IM_COL32(65, 65, 65, 255)) : IM_COL32(50, 50, 50, 255);
      ImU32 textColor = enabled ? IM_COL32(215, 215, 215, 255) : IM_COL32(110, 110, 110, 255);

      dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), bg, 5.0f);
      dl->AddRect(pos, ImVec2(pos.x + w, pos.y + h), border, 5.0f, 0, 1.2f);

      float iconSize = 28.0f;
      ImVec2 iconPos(pos.x + (w - iconSize) * 0.5f, pos.y + 12.0f);
      if (icon) {
        ImVec4 tint = enabled ? ImVec4(1, 1, 1, 1) : ImVec4(1, 1, 1, 0.35f);
        dl->AddImage(
            icon,
            iconPos,
            ImVec2(iconPos.x + iconSize, iconPos.y + iconSize),
            ImVec2(0, 0),
            ImVec2(1, 1),
            CherryGUI::ColorConvertFloat4ToU32(tint));
      }

      ImVec2 textSize = CherryGUI::CalcTextSize(title.c_str());
      float textX = pos.x + (w - textSize.x) * 0.5f;
      if (textSize.x > w - 8.0f) {
        textX = pos.x + 4.0f;
      }
      ImVec2 textPos(textX, iconPos.y + iconSize + 8.0f);
      dl->AddText(textPos, textColor, title.c_str());

      return clicked;
    }

    void
    DrawTileGrid(const std::vector<TileEntry> &entries, float available_width, float min_tile_w, float tile_h, float gap) {
      if (entries.empty()) {
        return;
      }

      int columns = (int)((available_width + gap) / (min_tile_w + gap));
      if (columns < 1) {
        columns = 1;
      }
      if (columns > (int)entries.size()) {
        columns = (int)entries.size();
      }

      float tile_w = (available_width - (columns - 1) * gap) / columns;

      for (size_t i = 0; i < entries.size(); ++i) {
        const auto &e = entries[i];

        if (DrawTile(e.id.c_str(), e.icon, e.title, tile_w, tile_h, e.enabled)) {
          if (e.on_click) {
            e.on_click();
          }
        }

        bool lastInRow = ((i + 1) % columns == 0);
        bool lastEntry = (i + 1 == entries.size());

        if (!lastInRow && !lastEntry) {
          CherryGUI::SameLine(0.0f, gap);
        } else if (!lastEntry) {
          CherryGUI::Dummy(ImVec2(0.0f, gap));
        }
      }

      CherryGUI::Dummy(ImVec2(0.0f, gap));
    }

    bool DrawRow(const char *id, ImTextureID icon, const std::string &title, const std::string &description, float width) {
      float height = 56.0f;
      float padding = 10.0f;
      float iconSize = 34.0f;

      ImVec2 pos = CherryGUI::GetCursorScreenPos();
      ImDrawList *dl = CherryGUI::GetWindowDrawList();

      bool clicked = CherryGUI::InvisibleButton(id, ImVec2(width, height));
      bool hovered = CherryGUI::IsItemHovered();

      if (hovered) {
        CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
      }

      ImU32 bg = hovered ? IM_COL32(52, 52, 52, 255) : IM_COL32(38, 38, 38, 255);
      dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bg, 5.0f);

      ImVec2 iconPos(pos.x + padding, pos.y + (height - iconSize) * 0.5f);
      if (icon) {
        dl->AddImage(icon, iconPos, ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
      }

      float textX = iconPos.x + iconSize + padding;
      dl->AddText(ImVec2(textX, pos.y + 9.0f), IM_COL32(230, 230, 230, 255), title.c_str());
      dl->AddText(ImVec2(textX, pos.y + 30.0f), IM_COL32(150, 150, 150, 255), description.c_str());

      return clicked;
    }

  }  // namespace

  AddWindow::AddWindow(const std::string &name, const std::string &path) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

    app_window_->SetRightBottombarCallback([this]() {
      CherryStyle::RemoveMarginY(7.5f);
      CherryStyle::RemoveMarginX(130.0f);
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
              .GetDataAs<bool>("isClicked")) {
        Cherry::DeleteAppWindow(app_window_);
      }

      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");

      if (CherryKit::ButtonImageText(
              Cherry::GetLocale("loc.window.content.content_browser.add"),
              Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
              .GetDataAs<bool>("isClicked")) {
        // TODO
      }
    });

    creation_path_ = path;

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &AddWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<AddWindow> AddWindow::create(const std::string &name, const std::string &path) {
    auto instance = std::shared_ptr<AddWindow>(new AddWindow(name, path));
    instance->setup_render_callback();
    return instance;
  }

  void AddWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void AddWindow::render() {
    float window_width = CherryGUI::GetWindowSize().x;
    float content_width = window_width - 17.0f;
    float main_image_height = window_width / 4.720f;

    CherryGUI::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_add.png")), ImVec2(window_width, main_image_height));

    CherryStyle::AddMarginX(8.0f);
    CherryGUI::Spacing();

    CherryStyle::AddMarginX(8.0f);
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText("Filters", Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"))
            .GetDataAs<bool>("isClicked")) {
      // TODO
    }

    CherryGUI::SameLine();

    CherryNextComponent.SetProperty("size_x", std::to_string(content_width - 90.0f));
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("description", "Search content...");
    CherryNextComponent.SetProperty(
        "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
    CherryNextComponent.SetProperty("description_logo_place", "r");
    CherryKit::InputString("", &add_window_search_);

    std::string search = add_window_search_;

    float grid_width = content_width - 8.0f;

    std::vector<TileEntry> quick_tiles = {
      { "addwin_tile_folder",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_folder.png")),
        "Folder",
        true,
        [this]() {
          if (create_folder_callback_) {
            create_folder_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
      { "addwin_tile_file",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/misc/icon_add_file.png")),
        "File",
        true,
        [this]() {
          if (create_file_callback_) {
            create_file_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
      { "addwin_tile_import",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/favicon.png")),
        "Import",
        false,
        [this]() {
          if (import_content_callback_) {
            import_content_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
    };

    std::vector<TileEntry> filtered_quick_tiles;
    for (auto &t : quick_tiles) {
      if (MatchesSearch(t.title, search)) {
        filtered_quick_tiles.push_back(t);
      }
    }

    if (!filtered_quick_tiles.empty()) {
      CherryStyle::AddMarginX(8.0f);
      CherryGUI::Spacing();
      CherryKit::SeparatorText("Quick create");
      CherryStyle::AddMarginX(8.0f);
      DrawTileGrid(filtered_quick_tiles, grid_width, 92.0f, 76.0f, 10.0f);
    }

    std::vector<TileEntry> simple_files = {
      { "addwin_tile_json",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/files/icon_json_file.png")),
        "JSON",
        true,
        [this]() {
          if (create_file_callback_) {
            create_file_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
      { "addwin_tile_cfg",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/files/icon_cfg_file.png")),
        "Config",
        true,
        [this]() {
          if (create_file_callback_) {
            create_file_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
      { "addwin_tile_ini",
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/icons/files/icon_ini_file.png")),
        "INI",
        true,
        [this]() {
          if (create_file_callback_) {
            create_file_callback_();
          }
          Cherry::DeleteAppWindow(app_window_);
        } },
    };

    std::vector<TileEntry> filtered_simple_files;
    for (auto &t : simple_files) {
      if (MatchesSearch(t.title, search)) {
        filtered_simple_files.push_back(t);
      }
    }

    if (!filtered_simple_files.empty()) {
      CherryStyle::AddMarginX(8.0f);
      CherryGUI::Spacing();
      CherryKit::SeparatorText("Create simple files");
      CherryStyle::AddMarginX(8.0f);
      DrawTileGrid(filtered_simple_files, grid_width, 92.0f, 76.0f, 10.0f);
    }

    for (auto m : vxe::get_current_context()->IO.em) {
      if (!m) {
        continue;
      }

      auto item_creators = m->get_content_browser_item_creators();
      if (item_creators.empty()) {
        continue;
      }

      std::vector<std::shared_ptr<decltype(item_creators)::value_type::element_type>> filtered_creators;
      for (auto &ic : item_creators) {
        if (MatchesSearch(ic->name, search)) {
          filtered_creators.push_back(ic);
        }
      }

      if (filtered_creators.empty()) {
        continue;
      }

      CherryStyle::AddMarginX(8.0f);
      CherryNextComponent.SetProperty("size_x", content_width);
      CherryNextComponent.SetProperty("color_border", "#343434");
      CherryNextComponent.SetProperty("color_bg", "#232323");
      CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
      CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
      CherryKit::HeaderImageText(
          m->proper_name(), Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"), [&]() {
            float row_width = content_width - 16.0f;

            for (auto &ic : filtered_creators) {
              CherryStyle::AddMarginX(8.0f);

              std::string row_id = "addwin_row_" + ic->name;
              ImTextureID icon = Cherry::GetTexture(ic->logo_path);

              if (DrawRow(row_id.c_str(), icon, ic->name, ic->description, row_width)) {
                if (ic->create_function && !creation_path_.empty()) {
                  ic->create_function(creation_path_);
                }
                Cherry::DeleteAppWindow(app_window_);
              }

              CherryGUI::Dummy(ImVec2(0.0f, 4.0f));
            }
          });
    }
  }

  void AddWindow::set_create_file_callback(const std::function<void()> &callback) {
    create_file_callback_ = callback;
  }

  void AddWindow::set_create_folder_callback(const std::function<void()> &callback) {
    create_folder_callback_ = callback;
  }

  void AddWindow::set_import_content_callback(const std::function<void()> &callback) {
    import_content_callback_ = callback;
  }

}  // namespace vxe