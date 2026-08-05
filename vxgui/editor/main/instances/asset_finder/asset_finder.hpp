//
//  asset_finder.hpp
//  Header and declarations for the asset finder window
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"

#ifndef VORTEX_EDITOR_ASSET_FINDER_HPP
#define VORTEX_EDITOR_ASSET_FINDER_HPP

namespace vxe {
  enum class AssetFinderShowMode { Thumbnails, List };

  struct AssetFinderFileTypeInfo {
    std::string icon;
    std::string label;
    ImU32 color;
  };

  class AssetFinderItem {
   public:
    bool (*f_Detect)(const std::string &path);

    std::string m_Name;

    ImTextureID m_Logo;
    ImVec4 m_LineColor;
    std::string m_Description;

    AssetFinderItem(
        bool (*detect_function)(const std::string &path),
        const std::string &name,
        const std::string &description,
        const ImVec4 &line_color)
        : m_Name(name),
          m_Description(description),
          f_Detect(detect_function),
          m_LineColor(line_color) { };
  };

  struct AssetFinderChild {
    std::function<void()> m_Child;
    std::string m_Name;
    bool m_Disabled = true;
    float m_DefaultSize = 0.0f;
    float m_MinSize;
    float m_MaxSize;
    float m_Size = 200.0f;
    float m_Ratio = 0.0f;
    bool m_Resizable = true;
    bool m_ResizeDisabled = false;
    bool m_Initialized = false;
    bool m_InitializedSec = false;
    bool m_InitializedTh = false;
    ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    void Enable() {
      m_Disabled = true;
    }

    void Disable() {
      m_Disabled = false;
    }

    AssetFinderChild(
        const std::string &name,
        const std::function<void()> &child,
        const float &default_size = 0.0f,
        const bool &resize_disabled = false,
        const float &min_size = 0.0f,
        const float &max_size = 0.0f)
        : m_Name(name),
          m_Child(child),
          m_ResizeDisabled(resize_disabled),
          m_DefaultSize(default_size),
          m_MinSize(min_size),
          m_MaxSize(max_size) {
    }
  };

  struct AssetFinderItemContent {
    std::string name;
    std::string path;
    std::string description;
    std::string size;
    std::string logo;
    std::shared_ptr<AssetFinderItem> item_ident;
  };

  struct AssetFinderItemStyle {
    ImU32 bg_color = IM_COL32(56, 56, 56, 150);
    ImU32 border_color = IM_COL32(50, 50, 50, 255);
    ImU32 line_color = IM_COL32(255, 255, 0, 255);
    float max_text_width = 100.0f;
    float border_radius = 5.0f;
  };

  struct AssetFinderItemParams {
    AssetFinderItemContent content;
    AssetFinderItemStyle style;
    bool selected = false;
  };

  class AssetFinder : public std::enable_shared_from_this<AssetFinder> {
   public:
    AssetFinder(const std::string &name, const std::string &start_path);

    enum class FileTypes {
      File_ASM,
      File_BIN,

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

      File_CFG,
      File_JSON,
      File_PICTURE,
      File_TXT,
      File_MD,
      File_YAML,
      File_INI,
      File_GIT,

      File_VORTEX_CONFIG,

      File_UNKNOW,
    };

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<AssetFinder> Create(const std::string &name, const std::string &base_path);
    void SetupRenderCallback();
    void Render();

    void AddChild(const AssetFinderChild &child);
    void ChangeDirectory(const std::filesystem::path &newDirectory);
    void GoBack();
    void GoForward();
    void DrawPathBar(const std::string &path);

    void RenderSideBar();
    void RenderContentBar();
    void RenderContentBarThumbnails();
    void RenderContentBarList();

    void SelectSingle(const std::string &path);
    void ClearSelection();
    std::string GetSingleSelected() const;

    void Select(const std::string &path) {
      SelectSingle(path);
    }

    bool IsHidden(const std::filesystem::path &path) const;

    bool DrawItemCard(const AssetFinderItemParams &p);

    void DrawFolderIcon(ImVec2 pos, ImVec2 size, ImU32 color);
    void FolderButton(const char *id, ImVec2 size, ImU32 color, const std::string &path);
    void DrawHierarchy(std::filesystem::path path, bool isDir, const std::string &label = "");

    void SetDefaultFolderColor(const std::string &hex);

    std::string GetFileBrowserFolderColor(const std::string &path) {
      for (auto &colored_folder : m_FolderColors) {
        if (colored_folder.first == path) {
          return colored_folder.second;
        }
      }

      return "#997D44FF";
    }

    bool IsPathFavorite(const std::string &path) {
      return false;
    };
    void SetColoredFolder(const std::string &path, const std::string &hex_color) { };

    std::vector<AssetFinderChild> m_Childs;
    std::string m_CurrentDirectory;

    std::function<bool(const std::string &)> m_IsValidPathCallback;
    std::vector<std::string> m_SelectedPath;
    int m_TargetPoolIndex;
    std::vector<std::string> m_TargetPossibilities;
    std::vector<std::shared_ptr<AssetFinderItem>> m_ItemToReconize;

    std::vector<std::string> m_Selected;
    bool m_GetFileBrowserPath = false;
    std::string m_ElementName;

    AssetFinderShowMode m_ShowMode = AssetFinderShowMode::Thumbnails;
    bool m_ShowFiles = true;
    bool m_ShowFolders = true;
    bool m_ShowItems = true;
    bool m_ShowHidden = true;

    std::function<void(const std::string &)> m_DeletePathCallback;
    std::function<void(const std::vector<std::string> &)> m_CopyPathsCallback;
    std::function<void(const std::vector<std::string> &)> m_PastePathsCallback;

    std::function<void(const std::string &)> m_ValidateCallback;

    std::function<void()> m_CancelCallback;

    void SetDeletePathCallback(const std::function<void(const std::string &)> &cb) {
      m_DeletePathCallback = cb;
    }
    void SetCopyPathsCallback(const std::function<void(const std::vector<std::string> &)> &cb) {
      m_CopyPathsCallback = cb;
    }
    void SetPastePathsCallback(const std::function<void(const std::vector<std::string> &)> &cb) {
      m_PastePathsCallback = cb;
    }
    void SetValidateCallback(const std::function<void(const std::string &)> &cb) {
      m_ValidateCallback = cb;
    }
    void SetCancelCallback(const std::function<void()> &cb) {
      m_CancelCallback = cb;
    }

    void DrawHighlightedText(
        ImDrawList *drawList,
        ImVec2 textPos,
        const char *text,
        const char *search,
        ImU32 highlightColor,
        ImU32 textColor,
        ImU32 highlightTextColor);
    bool isOnlySpacesOrEmpty(const char *str);

    std::string toLowerCase(const std::string &str);

    int levenshteinDistance(const std::string &s1, const std::string &s2);

    bool hasCommonLetters(const std::string &s1, const std::string &s2);

    bool areStringsSimilar(const std::string &s1, const std::string &s2, double threshold);

#ifndef _WIN32
    std::string ExecCommand(const char *cmd);
#endif

    std::string GetUserDirectory(const std::string &xdgName);

    std::uintmax_t getDirectorySize(const std::filesystem::path &directoryPath);

    std::string formatFileSize(size_t size);
    ImU32 DarkenColor(ImU32 color, float amount);
    std::string get_extension(const std::string &path);
    FileTypes detect_file(const std::string &path);

    const AssetFinderFileTypeInfo &GetFileTypeInfo(FileTypes type);

   private:
    bool opened;

    const std::string def = Cherry::GetPath("resources/imgs/icons/files/icon_default_file.png");
    const std::string pic = Cherry::GetPath("resources/imgs/icons/files/icon_picture_file.png");
    const std::string unk = Cherry::GetPath("resources/imgs/icons/files/icon_unknow_file.png");
    bool m_ShowFolderPannel = true;
    bool m_ShowFilterPannel = false;
    bool m_ShowThumbnailVisualizer = false;
    bool m_ShowSelectionQuantifier = false;
    float padding = 30.0f;
    float thumbnailSize = 94.0f;
    std::string pathToRename = "";
    char pathRename[256] = {};
    bool pool_add_mode = false;
    char pool_add_path[512] = {};
    char ProjectSearch[256] = {};
    float threshold = 0.4f;

    std::filesystem::path m_BaseDirectory;

    bool m_ChildsInitialized = false;

    std::stack<std::filesystem::path> m_BackHistory;
    std::stack<std::filesystem::path> m_ForwardHistory;

    std::vector<std::string> m_CopySelection;

    std::string m_DefaultFolderColor;

    std::vector<std::pair<std::string, std::string>> m_FolderColors;
    std::vector<std::string> m_FavoriteFolders;
    std::vector<std::string> m_Pools;

    std::vector<std::filesystem::path> m_Favorites;

    std::shared_ptr<Cherry::AppWindow> m_AppWindow;
  };
}  // namespace vxe
#endif  // VORTEX_EDITOR_ASSET_FINDER_HPP