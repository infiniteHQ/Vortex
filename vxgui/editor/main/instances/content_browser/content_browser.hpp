//
//  content_browser.hpp
//  Header and declarations for the content browser of the editor, with UI
//  window and front-end logic.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the MIT license.
//	For a copy,see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "./content_browser_utils.hpp"

#ifndef VORTEX_EDITOR_CONTENT_BROWSER_APPWINDOW_H
#define VORTEX_EDITOR_CONTENT_BROWSER_APPWINDOW_H

using namespace Cherry;

namespace VortexEditor {

class ContentBrowserAppWindow
    : public std::enable_shared_from_this<ContentBrowserAppWindow> {
public:
  ContentBrowserAppWindow(const std::string &name,
                          const std::string &start_path);

  // Create window
  static std::shared_ptr<ContentBrowserAppWindow>
  create(const std::string &name, const std::string &base_path);

  // Rendering
  std::shared_ptr<Cherry::AppWindow> &get_app_window();
  void setup_render_callback();
  void render();
  void render_side_bar();
  void render_filters_bar();
  void render_content_bar();
  void render_details_bar();
  void render_right_menubar();
  void render_menubar();
  void draw_path_bar(const std::string &path);
  void draw_folder_icon(ImVec2 pos, ImVec2 size, ImU32 color);

  // Widgets
  bool item_card(const ItemCardParams &p);
  bool horizontal_item_card(const ItemCardParams &p);

  // Navigation
  void change_directory(const std::filesystem::path &new_directory);
  void go_back();
  void go_forward();
  void path_changed();
  void select(const std::string &path) {
    for (auto already : selected_) {
      if (path == already) {
        return;
      }
    }
    selected_.push_back(path);
  }

  void RefreshCustomFolders();
  void RefreshPools();

  void add_child(const ContentBrowserChild &child);
  void CreateFile();
  void CreateFolder();
  void SpawnImportWindow();

  void ChangeShowMode(ContentShowMode mode) { this->m_ShowMode = mode; }

  void FolderButton(const char *id, ImVec2 size, const std::string &path,
                    const std::string &color);
  void DrawHierarchy(std::filesystem::path path, bool isDir,
                     const std::string &label);

  std::string GetFileTypeStr(FileTypes type);

  void SetDefaultFolderColor(const std::string &hex);

  void FolderIcon(ImVec2 size, ImU32 color);

  std::vector<std::shared_ptr<VortexEditor::ContentBrowserAddWindow>>
      m_AddWindows;

  int m_AddWindowCounter = 0;

  void SpawnAddWindow() {
    m_AddWindowCounter++;
    Cherry::ApplicationSpecification spec;

    std::string name = "?loc:loc.window_names.add_content" +
                       std::to_string(m_AddWindowCounter);
    auto new_win = VortexEditor::ContentBrowserAddWindow::create(
        name, current_directory_.string());
    new_win->get_app_window()->SetVisibility(true);
    new_win->SetCreateFileCallback([this]() { this->CreateFile(); });
    new_win->SetCreateFolderCallback([this]() { this->CreateFolder(); });
    new_win->SetImportContentCallback([this]() { this->SpawnImportWindow(); });

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
    spec.FramebarCallback = []() {};
    spec.UsingCloseCallback = true;
    spec.CloseCallback = [this, new_win]() {
      Cherry::DeleteAppWindow(new_win->get_app_window());
    };

    spec.MenubarCallback = []() {};
    spec.WindowSaves = false;
    new_win->get_app_window()->AttachOnNewWindow(spec);
    Cherry::AddAppWindow(new_win->get_app_window());
  }

  std::string GetContentBrowserFolderColor(const std::string &path) {
    size_t lastSlashIndex = path.find_last_of('/');
    std::string folderName = path.substr(lastSlashIndex + 1);

    for (auto &colored_folder : folder_colors_) {
      if (colored_folder.first == path) {
        return colored_folder.second;
      }
    }

    if (!folderName.empty() && folderName[0] == '.') {
      return "#544F46FF";
    }

    return "#c2a24c";
  }

  void AddReconizedItem(const std::shared_ptr<ContenBrowserItem> &item) {};
  bool IsPathFavorite(const std::string &path) {
    for (auto fav_folder : m_FavoriteFolders) {
      if (fav_folder == path) {
        return true;
      }
    }
    return false;
  };

  void SetColoredFolder(const std::string &path, const std::string &hex_color) {
  };

  std::vector<ContentBrowserChild> m_Childs;

  std::function<void(const std::string &)> m_DeletePathCallback;
  std::function<void(const std::vector<std::string> &, bool)>
      m_CopyPathsCallback;
  std::function<void(const std::vector<std::string> &, bool)>
      m_CutPathsCallback;
  std::function<void(const std::string &)> m_PastePathsCallback;

  bool HasExtension(const std::string &ext) {
    return std::find(ExtentionsFilters.begin(), ExtentionsFilters.end(), ext) !=
           ExtentionsFilters.end();
  }

  void ToggleExtension(const std::string &ext, bool *state) {
    if (*state) {
      if (!HasExtension(ext)) {
        ExtentionsFilters.push_back(ext);
      }
    } else {
      ExtentionsFilters.erase(
          std::remove(ExtentionsFilters.begin(), ExtentionsFilters.end(), ext),
          ExtentionsFilters.end());
    }
  }

private:
  bool opened;

  bool thumbnail_mode_ = true;
  bool list_mode_ = false;

  bool show_types_ = false;
  bool show_sizes_ = false;
  bool show_filter_pannel_ = false;
  bool show_thumbnail_visualizer_ = false;

  bool will_show_filter_pannel_ = false;
  bool previous_filter_pannel_state_ = false;
  bool m_PreviousThumbnailVisualizerState = false;

  bool m_ShowFolderPannel = true;
  bool m_ShowSelectionQuantifier = false;
  bool m_ChildSizesInitialized = false;

  ContentShowMode m_ShowMode = ContentShowMode::Thumbmails;

  // directories
  std::filesystem::path base_directory_;
  std::filesystem::path previous_directory_;
  std::filesystem::path current_directory_;

  // history
  std::stack<std::filesystem::path> back_history_;
  std::stack<std::filesystem::path> forward_history_;

  // selection
  std::vector<std::string> selected_;
  std::vector<std::string> copy_selection_;
  std::vector<std::string> cut_selection_;

  // Path/Color
  std::vector<std::pair<std::string, std::string>> folder_colors_;
  std::string default_folder_color_;
  std::string default_hidden_folder_color_;

  // pools and special items
  std::vector<std::string> m_FavoriteFolders;
  std::vector<std::pair<std::string, std::string>> m_Pools;
  std::vector<std::shared_ptr<ContenBrowserItem>> m_ItemToReconize;

  std::vector<std::filesystem::path> m_Favorites;

  std::shared_ptr<Image> m_ProjectIcon;
  std::shared_ptr<Image> m_FileIcon;
  std::shared_ptr<Image> m_DirectoryIcon;

  // Filters
  bool ShowHidden = false;
  bool ShowFolders = true;
  bool ShowFiles = true;
  bool ShowItems = true;
  std::vector<std::string> ExtentionsFilters;

  bool m_SearchBar = false;

  std::shared_ptr<AppWindow> m_AppWindow;
};
} // namespace VortexEditor

#endif // VORTEX_EDITOR_CONTENT_BROWSER_APPWINDOW_H