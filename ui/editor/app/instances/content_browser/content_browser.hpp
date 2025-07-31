#pragma once
#include "../../../../../lib/cherry/cherry.hpp"
#include "../../../../../main/include/vortex.h"
#include "../../../../../main/include/vortex_internals.h"

#ifndef VORTEX_EDITOR_CONTENT_BROWSER_APPWINDOW_H
#define VORTEX_EDITOR_CONTENT_BROWSER_APPWINDOW_H

using namespace Cherry;

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stack>
#include <unordered_set>

namespace VortexEditor {
class ContenBrowserItem {
public:
  bool (*f_Detect)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 m_LineColor;
  std::string m_Description;

  ContenBrowserItem(bool (*detect_function)(const std::string &path),
                    const std::string &name, const std::string &description,
                    const ImVec4 &line_color)
      : m_Name(name), m_Description(description), f_Detect(detect_function),
        m_LineColor(line_color) {};
};
enum class ContentShowMode { Thumbmails, List, Columns };

enum class FileTypes {
  // Very low level
  File_ASM,
  File_BIN,

  // Programming languages
  File_C,
  File_H,
  File_CPP,
  File_HPP,
  File_INL,
  File_RUST,
  File_ZIG,
  File_GO,
  File_JAVA,
  File_JAVASCRIPT,
  File_COBOL,
  File_PASCAL,
  File_CARBON,
  File_PYTHON,
  File_RUBY,
  File_PHP,
  File_SWIFT,
  File_CSHARP,
  File_VB,
  File_DART,
  File_KOTLIN,
  File_SCALA,
  File_PERL,
  File_SHELL,
  File_BATCH,
  File_LUA,
  File_R,
  File_MATLAB,
  File_JULIA,
  File_SQL,
  File_GROOVY,
  File_FORTRAN,
  File_ERLANG,
  File_ELIXIR,
  File_OCAML,

  // Web and Markup
  File_HTML,
  File_CSS,
  File_XML,

  // Config
  File_CFG,
  File_JSON,
  File_YAML,
  File_INI,

  // Git
  File_GIT,

  // Documents
  File_TXT,
  File_MD,
  File_DOC,
  File_PDF,

  // Pictures
  File_PICTURE,

  // Videos
  File_VIDEO,

  // Audio
  File_AUDIO,

  // Archives
  File_ARCHIVE,

  // Miscellaneous
  File_LOG,
  File_BACKUP,
  File_TEMP,
  File_DATA,

  // Other
  File_UNKNOWN,
};

struct FileHandler {
  FileTypes for_type;
  std::string file_handler_name;
  std::string handler_module_name;
  std::string handler_plugin_name;
};

struct ContentBrowserChild {
  std::function<void()> m_Child;
  std::string m_Name;
  bool m_Disabled = true;
  float m_DefaultSize = 0.0f;
  float m_Size = 0.0f;
  ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

  void Enable() { m_Disabled = false; }

  void Disable() { m_Disabled = true; }

  ContentBrowserChild(const std::string &name,
                      const std::function<void()> &child,
                      float defaultSize = 0.0f)
      : m_Name(name), m_Child(child), m_DefaultSize(defaultSize),
        m_Size(defaultSize) {}
  ContentBrowserChild() {};
};

class ContentBrowserAppWindow
    : public std::enable_shared_from_this<ContentBrowserAppWindow> {
public:
  ContentBrowserAppWindow(const std::string &name,
                          const std::string &start_path);

  void RefreshCustomFolders();
  void RefreshPools();

  std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
  static std::shared_ptr<ContentBrowserAppWindow>
  Create(const std::string &name, const std::string &base_path);
  void SetupRenderCallback();
  void Render();

  bool ItemCard(const std::string &name, const std::string &path,
                const std::string &description, const std::string &size,
                bool selected, const std::string &logo, ImU32 bgColor,
                ImU32 borderColor, ImU32 lineColor, float maxTextWidth,
                float borderRadius);
  void AddChild(const ContentBrowserChild &child);
  void ChangeDirectory(const std::filesystem::path &newDirectory);
  void GoBack();
  void GoForward();
  void DrawPathBar(const std::string &path);

  void RenderSideBar();
  void RenderFiltersBar();
  void RenderContentBar();
  void RenderDetailsBar();
  void RenderRightMenubar();

  void RenderMenubar();

  void Select(const std::string &path) {
    for (auto already : m_Selected) {
      if (path == already) {
        return;
      }
    }
    m_Selected.push_back(path);
  }

  void ChangeShowMode(ContentShowMode mode) { this->m_ShowMode = mode; }

  // To move in components
  void DrawFolderIcon(ImVec2 pos, ImVec2 size, ImU32 color);
  void MyFolderButton(const char *id, ImVec2 size, ImU32 color,
                      const std::string &path);
  void FolderButton(const char *id, ImVec2 size, ImU32 color,
                    const std::string &path);
  void DrawHierarchy(std::filesystem::path path, bool isDir,
                     const std::string &label);

  void SetDefaultFolderColor(const std::string &hex);

  void FolderIcon(ImVec2 size, ImU32 color);

  std::string GetContentBrowserFolderColor(const std::string &path) {
    size_t lastSlashIndex = path.find_last_of('/');
    std::string folderName = path.substr(lastSlashIndex + 1);

    for (auto &colored_folder : m_FolderColors) {
      if (colored_folder.first == path) {
        return colored_folder.second;
      }
    }

    if (!folderName.empty() && folderName[0] == '.') {
      return "#544F46FF";
    }

    return "#997D44FF";
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

private:
  bool opened;

  bool m_ThumbnailMode = true;
  bool m_ListMode = false;

  bool m_ShowTypes = false;
  bool m_ShowSizes = false;

  bool m_WillShowFilterPannel = false;
  bool m_ShowFilterPannel = false;
  bool m_ShowThumbnailVisualizer = false;
  bool m_PreviousFilterPannelState = false;
  bool m_PreviousThumbnailVisualizerState = false;

  bool m_ShowFolderPannel = true;
  bool m_ShowSelectionQuantifier = false;
  bool m_ChildSizesInitialized = false;

  ContentShowMode m_ShowMode = ContentShowMode::Thumbmails;

  std::filesystem::path m_BaseDirectory;

  std::filesystem::path m_CurrentDirectory;
  std::stack<std::filesystem::path> m_BackHistory;
  std::stack<std::filesystem::path> m_ForwardHistory;

  std::vector<std::string> m_Selected;
  std::vector<std::string> m_CopySelection;
  std::vector<std::string> m_CutSelection;

  std::string m_DefaultFolderColor;
  std::string m_DefaultHiddenFolderColor;

  // Path/Color
  std::vector<std::pair<std::string, std::string>> m_FolderColors;
  std::vector<std::string> m_FavoriteFolders;
  std::vector<std::pair<std::string, std::string>> m_Pools;
  std::vector<std::shared_ptr<ContenBrowserItem>> m_ItemToReconize;

  std::vector<std::filesystem::path> m_Favorites;

  std::shared_ptr<Image> m_ProjectIcon;
  std::shared_ptr<Image> m_FileIcon;
  std::shared_ptr<Image> m_DirectoryIcon;

  std::shared_ptr<AppWindow> m_AppWindow;

  bool m_SearchBar = false;
};
} // namespace VortexEditor

#endif // VORTEX_EDITOR_CONTENT_BROWSER_APPWINDOW_H