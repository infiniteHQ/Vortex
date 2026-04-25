//
//  content_browser.cpp
//  Main sources of the content browser ui and front-end, constructors and
//  main features.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./content_browser.hpp"

namespace vxe {

  ContentBrowser::ContentBrowser(const std::string &name, const std::string &start_path) {
    app_window_ = std::make_shared<AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));
    std::shared_ptr<AppWindow> win = app_window_;

    refresh_custom_folders();
    refresh_pools();

    app_window_->SetLeftMenubarCallback([this]() { render_menubar(); });
    app_window_->m_Closable = true;
    app_window_->SetCloseCallback([this]() { Cherry::DeleteAppWindow(app_window_); });

    app_window_->SetRightMenubarCallback([this]() { render_right_menubar(); });
    app_window_->SetLeftBottombarCallback([this]() {
      if (selected_.size() > 0) {
        std::string terminaison = selected_.size() >= 0 ? "s" : "";
        std::string label = std::to_string(selected_.size()) + " element" + terminaison + " selected.";
        CherryGUI::Text(label.c_str());
      }

      if (copy_selection_.size() > 0) {
        std::string terminaison = copy_selection_.size() > 0 ? "s" : "";
        std::string label = std::to_string(copy_selection_.size()) + " element" + terminaison + " copied.";
        CherryGUI::Text(label.c_str());
      }
    });

    base_directory_ = start_path;
    current_directory_ = base_directory_;

    ContentBrowserChild sidebar("render_side_bar", [this]() { render_side_bar(); });
    sidebar.Enable();
    sidebar.m_DefaultSize = filterbar_width_;
    sidebar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");

    add_child(sidebar);

    ContentBrowserChild filterbar("render_filters_bar", [this]() { render_filters_bar(); });
    filterbar.Disable();
    filterbar.m_DefaultSize = 250.0f;
    filterbar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");
    add_child(filterbar);

    ContentBrowserChild contentbar("render_content_bar", [this]() { render_content_bar(); });
    contentbar.Enable();
    contentbar.m_DefaultSize = 0.0;
    add_child(ContentBrowserChild(contentbar));

    ContentBrowserChild detailsbar("render_details_bar", [this]() { render_details_bar(); });
    detailsbar.Disable();
    detailsbar.m_DefaultSize = 250.0f;
    add_child(ContentBrowserChild(detailsbar));
  }

  bool ContentBrowser::is_path_favorite(const std::string &path) {
    for (auto fav_folder : favorite_folders_) {
      if (fav_folder == path) {
        return true;
      }
    }
    return false;
  };

  std::string ContentBrowser::get_file_type_str(FileTypes type) {
    switch (type) {
      // Very low level
      case FileTypes::File_ASM: return "file_asm";
      case FileTypes::File_BIN: return "file_bin";

      // Programming languages
      case FileTypes::File_C: return "file_c";
      case FileTypes::File_H: return "file_h";
      case FileTypes::File_CPP: return "file_cpp";
      case FileTypes::File_HPP: return "file_hpp";
      case FileTypes::File_INL: return "file_inl";
      case FileTypes::File_RUST: return "file_rust";
      case FileTypes::File_ZIG: return "file_zig";
      case FileTypes::File_GO: return "file_go";
      case FileTypes::File_JAVA: return "file_java";
      case FileTypes::File_JAVASCRIPT: return "file_javascript";
      case FileTypes::File_COBOL: return "file_cobol";
      case FileTypes::File_PASCAL: return "file_pascal";
      case FileTypes::File_CARBON: return "file_carbon";
      case FileTypes::File_PYTHON: return "file_python";
      case FileTypes::File_RUBY: return "file_ruby";
      case FileTypes::File_PHP: return "file_php";
      case FileTypes::File_SWIFT: return "file_swift";
      case FileTypes::File_CSHARP: return "file_csharp";
      case FileTypes::File_VB: return "file_vb";
      case FileTypes::File_DART: return "file_dart";
      case FileTypes::File_KOTLIN: return "file_kotlin";
      case FileTypes::File_SCALA: return "file_scala";
      case FileTypes::File_PERL: return "file_perl";
      case FileTypes::File_SHELL: return "file_shell";
      case FileTypes::File_BATCH: return "file_batch";
      case FileTypes::File_LUA: return "file_lua";
      case FileTypes::File_R: return "file_r";
      case FileTypes::File_MATLAB: return "file_matlab";
      case FileTypes::File_JULIA: return "file_julia";
      case FileTypes::File_SQL: return "file_sql";
      case FileTypes::File_GROOVY: return "file_groovy";
      case FileTypes::File_FORTRAN: return "file_fortran";
      case FileTypes::File_ERLANG: return "file_erlang";
      case FileTypes::File_ELIXIR: return "file_elixir";
      case FileTypes::File_OCAML: return "file_ocaml";

      // Web and Markup
      case FileTypes::File_HTML: return "file_html";
      case FileTypes::File_CSS: return "file_css";
      case FileTypes::File_XML: return "file_xml";

      // Config
      case FileTypes::File_CFG: return "file_cfg";
      case FileTypes::File_JSON: return "file_json";
      case FileTypes::File_YAML: return "file_yaml";
      case FileTypes::File_INI: return "file_ini";

      // Git
      case FileTypes::File_GIT: return "file_git";

      // Documents
      case FileTypes::File_TXT: return "file_txt";
      case FileTypes::File_MD: return "file_md";
      case FileTypes::File_DOC: return "file_doc";
      case FileTypes::File_PDF: return "file_pdf";

      // Pictures
      case FileTypes::File_PICTURE: return "file_picture";

      // Videos
      case FileTypes::File_VIDEO: return "file_video";

      // Audio
      case FileTypes::File_AUDIO: return "file_audio";

      // Archives
      case FileTypes::File_ARCHIVE: return "file_archive";

      // Miscellaneous
      case FileTypes::File_LOG: return "file_log";
      case FileTypes::File_BACKUP: return "file_backup";
      case FileTypes::File_TEMP: return "file_temp";
      case FileTypes::File_DATA: return "file_data";

      // Other
      case FileTypes::File_UNKNOWN: return "file_unknown";
    }

    return "file_unknown";  // fallback
  }

  void ContentBrowser::draw_folder_button(
      const char *id,
      ImVec2 size,
      const std::string &path,
      const std::string &color = "#c2a24c") {
    auto cmp = CherryKit::WidgetFolder(color, size.x, size.y - 2);
    if (cmp.GetData("isClicked") == "true") {
      if (CherryGUI::IsKeyDown(ImGuiKey_LeftCtrl) || CherryGUI::IsKeyDown(ImGuiKey_RightCtrl)) {
        selected_.push_back(path);
      } else {
        selected_.clear();
        selected_.push_back(path);
      }
    }
    if (cmp.GetData("isDoubleClicked") == "true") {
      change_directory(path);
    }
  }

  void ContentBrowser::folder_icon(ImVec2 size, ImU32 color) {
    CherryKit::WidgetFolder("#c2a24c", size.x, size.y - 2);
  }

  void ContentBrowser::draw_hierarchy(std::filesystem::path path, bool isDir, const std::string &label) {
    if (!isDir)
      return;

    std::string tree_label = (label.empty() ? path.filename().string() : label);
    std::string unique_id = path.string() + "##treenode";

    ImGuiTreeNodeFlags treeNodeFlags =
        ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

    ImVec2 nodePos = CherryGUI::GetCursorScreenPos();

    bool opened = CherryGUI::TreeNodeEx(unique_id.c_str(), treeNodeFlags, "%s", tree_label.c_str());

    ImVec2 itemSize = CherryGUI::GetItemRectSize();
    ImVec2 iconSize = ImVec2(12, 12);
    ImVec2 iconPos = ImVec2(nodePos.x + 2.0f, nodePos.y + (itemSize.y - iconSize.y) * 0.5f);

    draw_folder(get_content_browser_folder_color(path.string()), iconSize.x, iconSize.y, iconPos);

    if (CherryGUI::IsItemClicked()) {
      change_directory(path);
    }

    if (opened) {
      try {
        std::vector<std::filesystem::directory_entry> entries;
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
          if (entry.is_directory()) {
            entries.push_back(entry);
          }
        }

        std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
          return a.path().filename() < b.path().filename();
        });

        for (const auto &entry : entries) {
          try {
            draw_hierarchy(entry.path(), entry.is_directory());
          } catch (const std::exception &e) {
            std::cerr << "Error displaying " << entry.path() << ": " << e.what() << std::endl;
          }
        }
      } catch (const std::exception &e) {
        std::cerr << "Error reading directory " << path << ": " << e.what() << std::endl;
      }

      CherryGUI::TreePop();
    }
  }

  void ContentBrowser::render_details_bar() {
  }

  std::shared_ptr<Cherry::AppWindow> &ContentBrowser::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<ContentBrowser> ContentBrowser::create(const std::string &name, const std::string &base_path) {
    auto instance = std::shared_ptr<ContentBrowser>(new ContentBrowser(name, base_path));
    instance->setup_render_callback();
    return instance;
  }

  void ContentBrowser::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

}  // namespace vxe