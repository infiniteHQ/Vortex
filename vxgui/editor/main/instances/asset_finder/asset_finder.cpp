#include "./asset_finder.hpp"

#ifdef _WIN32
#include <knownfolders.h>
#include <shlobj.h>
#include <windows.h>
#elif __APPLE__
#include <pwd.h>
#include <unistd.h>
#else  // Linux
#include <pwd.h>
#include <unistd.h>
#endif

// To move in class members
static float padding = 30.0f;
static float thumbnailSize = 94.0f;
static std::string pathToRename = "";
static char pathRename[256];
static bool pool_add_mode = false;
static char pool_add_path[512];
static char ProjectSearch[256];
static float threshold = 0.4f;

static void DrawHighlightedText(
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

static bool isOnlySpacesOrEmpty(const char *str) {
  if (str == nullptr || std::strlen(str) == 0) {
    return true;
  }

  for (size_t i = 0; i < std::strlen(str); ++i) {
    if (str[i] != ' ') {
      return false;
    }
  }
  return true;
}

static std::string toLowerCase(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

static int levenshteinDistance(const std::string &s1, const std::string &s2) {
  const size_t m = s1.size();
  const size_t n = s2.size();
  std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

  for (size_t i = 0; i <= m; ++i) {
    for (size_t j = 0; j <= n; ++j) {
      if (i == 0) {
        dp[i][j] = j;
      } else if (j == 0) {
        dp[i][j] = i;
      } else {
        int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
        dp[i][j] = std::min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
      }
    }
  }
  return dp[m][n];
}

static bool hasCommonLetters(const std::string &s1, const std::string &s2) {
  std::unordered_set<char> set1(s1.begin(), s1.end());
  for (char c : s2) {
    if (set1.find(c) != set1.end()) {
      return true;
    }
  }
  return false;
}

static bool areStringsSimilar(const std::string &s1, const std::string &s2, double threshold) {
  std::string lower_s1 = toLowerCase(s1);
  std::string lower_s2 = toLowerCase(s2);

  int dist = levenshteinDistance(lower_s1, lower_s2);
  int maxLength = std::max(lower_s1.size(), lower_s2.size());
  double similarity = maxLength == 0 ? 1.0 : (1.0 - (static_cast<double>(dist) / maxLength));

  if (std::strlen(ProjectSearch) < 5) {
    return similarity >= threshold || hasCommonLetters(lower_s1, lower_s2);
  }

  return similarity >= threshold;
}

#ifndef _WIN32
std::string ExecCommand(const char *cmd) {
  std::array<char, 256> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe)
    return "";
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  if (!result.empty() && result.back() == '\n')
    result.pop_back();
  return result;
}
#endif

std::string GetUserDirectory(const std::string &xdgName) {
#ifdef _WIN32
  static const std::map<std::string, const KNOWNFOLDERID> folderMap = {
    { "DESKTOP", FOLDERID_Desktop }, { "DOCUMENTS", FOLDERID_Documents }, { "DOWNLOAD", FOLDERID_Downloads },
    { "MUSIC", FOLDERID_Music },     { "PICTURES", FOLDERID_Pictures },   { "VIDEOS", FOLDERID_Videos },
    { "HOME", FOLDERID_Profile }
  };

  auto it = folderMap.find(xdgName);
  if (it == folderMap.end())
    return "";

  PWSTR path = nullptr;
  if (SUCCEEDED(SHGetKnownFolderPath(it->second, 0, NULL, &path))) {
    std::wstring ws(path);
    CoTaskMemFree(path);
    return std::string(ws.begin(), ws.end());
  }
  return "";

#else  // Unix-like
  if (xdgName == "HOME") {
    const char *homeDir = getenv("HOME");
    if (!homeDir) {
      struct passwd *pw = getpwuid(getuid());
      homeDir = pw->pw_dir;
    }
    return std::string(homeDir);
  }

  std::string cmd = "xdg-user-dir " + xdgName;
  std::string output = ExecCommand(cmd.c_str());
  if (!output.empty())
    return output;

  const char *homeDir = getenv("HOME");
  if (!homeDir) {
    struct passwd *pw = getpwuid(getuid());
    homeDir = pw->pw_dir;
  }
  return std::string(homeDir) + "/" + xdgName;
#endif
}

static std::uintmax_t getDirectorySize(const std::filesystem::path &directoryPath) {
  std::uintmax_t size = 0;
  std::error_code ec;

  for (const auto &entry : std::filesystem::recursive_directory_iterator(
           directoryPath, std::filesystem::directory_options::skip_permission_denied, ec)) {
    if (std::filesystem::is_regular_file(entry.path())) {
      size += std::filesystem::file_size(entry.path());
    }
  }

  return size;
}

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

static std::string formatFileSize(size_t size) {
  const char *units[] = { "o", "ko", "Mo", "Go", "To" };
  int unitIndex = 0;
  double displaySize = static_cast<double>(size);

  while (displaySize >= 1024 && unitIndex < 4) {
    displaySize /= 1024;
    ++unitIndex;
  }

  char formattedSize[20];
  snprintf(formattedSize, sizeof(formattedSize), "%.2f %s", displaySize, units[unitIndex]);
  return std::string(formattedSize);
}

static ImU32 DarkenColor(ImU32 color, float amount) {
  int r = (color >> IM_COL32_R_SHIFT) & 0xFF;
  int g = (color >> IM_COL32_G_SHIFT) & 0xFF;
  int b = (color >> IM_COL32_B_SHIFT) & 0xFF;
  int a = (color >> IM_COL32_A_SHIFT) & 0xFF;

  r = static_cast<int>(r * (1.0f - amount));
  g = static_cast<int>(g * (1.0f - amount));
  b = static_cast<int>(b * (1.0f - amount));

  return IM_COL32(r, g, b, a);
}

std::string get_extension(const std::string &path) {
  size_t dot_pos = path.find_last_of('.');
  if (dot_pos == std::string::npos)
    return "";
  return path.substr(dot_pos + 1);
}

FileTypes detect_file(const std::string &path) {
  static const std::unordered_map<std::string, FileTypes> extension_map = {
    { "asm", FileTypes::File_ASM },      { "bin", FileTypes::File_BIN },       { "c", FileTypes::File_C },
    { "h", FileTypes::File_H },          { "cpp", FileTypes::File_CPP },       { "hpp", FileTypes::File_HPP },
    { "inl", FileTypes::File_INL },      { "rs", FileTypes::File_RUST },       { "zig", FileTypes::File_ZIG },
    { "go", FileTypes::File_GO },        { "cfg", FileTypes::File_CFG },       { "json", FileTypes::File_JSON },
    { "txt", FileTypes::File_TXT },      { "md", FileTypes::File_MD },         { "yaml", FileTypes::File_YAML },
    { "ini", FileTypes::File_INI },      { "gitignore", FileTypes::File_GIT }, { "gitmodules", FileTypes::File_GIT },
    { "git", FileTypes::File_GIT },      { "png", FileTypes::File_PICTURE },   { "jpg", FileTypes::File_PICTURE },
    { "jpeg", FileTypes::File_PICTURE },
  };

  std::string extension = get_extension(path);
  auto it = extension_map.find(extension);
  if (it != extension_map.end()) {
    return it->second;
  } else {
    return FileTypes::File_UNKNOW;
  }
}

struct AssetFinderFileTypeInfo {
  std::string icon;
  std::string label;
  ImU32 color;
};

static const AssetFinderFileTypeInfo &GetFileTypeInfo(FileTypes type) {
  static const std::string def = Cherry::GetPath("resources/imgs/icons/files/icon_default_file.png");
  static const std::string pic = Cherry::GetPath("resources/imgs/icons/files/icon_picture_file.png");
  static const std::string unk = Cherry::GetPath("resources/imgs/icons/files/icon_unknow_file.png");

  static const std::unordered_map<FileTypes, AssetFinderFileTypeInfo> infos = {
    { FileTypes::File_PICTURE, { pic, "Image", IM_COL32(90, 200, 140, 255) } },
    { FileTypes::File_CPP, { def, "C++ Source", IM_COL32(0, 150, 220, 255) } },
    { FileTypes::File_HPP, { def, "C++ Header", IM_COL32(0, 150, 220, 255) } },
    { FileTypes::File_C, { def, "C Source", IM_COL32(90, 150, 220, 255) } },
    { FileTypes::File_H, { def, "C Header", IM_COL32(90, 150, 220, 255) } },
    { FileTypes::File_INL, { def, "Inline Source", IM_COL32(90, 150, 220, 255) } },
    { FileTypes::File_RUST, { def, "Rust Source", IM_COL32(222, 165, 132, 255) } },
    { FileTypes::File_ZIG, { def, "Zig Source", IM_COL32(247, 164, 29, 255) } },
    { FileTypes::File_GO, { def, "Go Source", IM_COL32(0, 173, 216, 255) } },
    { FileTypes::File_JSON, { def, "JSON", IM_COL32(219, 174, 89, 255) } },
    { FileTypes::File_TXT, { def, "Text", IM_COL32(180, 180, 180, 255) } },
    { FileTypes::File_MD, { def, "Markdown", IM_COL32(180, 180, 180, 255) } },
    { FileTypes::File_YAML, { def, "YAML", IM_COL32(160, 120, 220, 255) } },
    { FileTypes::File_INI, { def, "Config", IM_COL32(160, 160, 160, 255) } },
    { FileTypes::File_CFG, { def, "Config", IM_COL32(160, 160, 160, 255) } },
    { FileTypes::File_GIT, { def, "Git", IM_COL32(240, 90, 60, 255) } },
    { FileTypes::File_ASM, { def, "Assembly", IM_COL32(200, 100, 100, 255) } },
    { FileTypes::File_BIN, { def, "Binary", IM_COL32(120, 120, 120, 255) } },
  };

  static const AssetFinderFileTypeInfo default_info = { unk, "File", IM_COL32(140, 140, 140, 255) };

  auto it = infos.find(type);
  return it != infos.end() ? it->second : default_info;
}

namespace vxe {
  AssetFinder::AssetFinder(const std::string &name, const std::string &start_path) {
    m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
    m_AppWindow->SetIcon(Cherry::Application::CookPath("resources/imgs/icons/misc/icon_folder.png"));
    std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;

    m_AppWindow->SetInternalPaddingX(0.0f);
    m_AppWindow->SetInternalPaddingY(0.0f);
    m_AppWindow->SetLeftMenubarCallback([this]() {
      CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 12));

      static bool wasButtonX1Pressed = false;
      static bool wasButtonX2Pressed = false;
      if (m_BackHistory.empty()) {
        Cherry::SetNextComponentProperty("color_border", "#00000000");
        Cherry::SetNextComponentProperty("size_x", "15.0f");
        Cherry::SetNextComponentProperty("size_y", "15.0f");
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_arrow_l_disabled.png"))
                .GetData("isClicked") == "true") {
          //
        }
      } else {
        Cherry::SetNextComponentProperty("color_border", "#00000000");
        Cherry::SetNextComponentProperty("size_x", "15.0f");
        Cherry::SetNextComponentProperty("size_y", "15.0f");
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_arrow_l_enabled.png"))
                .GetData("isClicked") == "true") {
          GoBack();
        }

        Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
        bool isButtonX1Pressed = mouseState & SDL_BUTTON(SDL_BUTTON_X1);
        if (isButtonX1Pressed && !wasButtonX1Pressed) {
          GoBack();
        }
        wasButtonX1Pressed = isButtonX1Pressed;
      }

      if (m_ForwardHistory.empty()) {
        Cherry::SetNextComponentProperty("color_border", "#00000000");
        Cherry::SetNextComponentProperty("size_x", "15.0f");
        Cherry::SetNextComponentProperty("size_y", "15.0f");
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_arrow_r_disabled.png"))
                .GetData("isClicked") == "true") {
          //
        }
      } else {
        Cherry::SetNextComponentProperty("color_border", "#00000000");
        Cherry::SetNextComponentProperty("size_x", "15.0f");
        Cherry::SetNextComponentProperty("size_y", "15.0f");
        if (CherryKit::ButtonImageText("", Cherry::GetPath("resources/imgs/icons/misc/icon_arrow_r_enabled.png"))
                .GetData("isClicked") == "true") {
          GoForward();
        }

        Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
        bool isButtonX2Pressed = mouseState & SDL_BUTTON(SDL_BUTTON_X2);
        if (isButtonX2Pressed && !wasButtonX2Pressed) {
          GoForward();
        }
        wasButtonX2Pressed = isButtonX2Pressed;
      }
      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor();

      CherryNextComponent.SetProperty("size_x", 400.0f);
      CherryKit::InputString("", &m_CurrentDirectory);
    });

    m_AppWindow->SetRightMenubarCallback([this]() {
      int mode_index = (m_ShowMode == AssetFinderShowMode::Thumbnails) ? 0 : 1;
      CherryNextComponent.SetProperty("size_x", 150.0f);
      switch (CherryKit::ComboImageText(
                  "",
                  { { "Thumbnails", Cherry::GetPath("resources/imgs/icons/misc/icon_thumbnails.png") },
                    { "List", Cherry::GetPath("resources/imgs/icons/misc/icon_lines.png") } },
                  mode_index)
                  .GetPropertyAs<int>("selected")) {
        case 0: m_ShowMode = AssetFinderShowMode::Thumbnails; break;
        case 1: m_ShowMode = AssetFinderShowMode::List; break;
        default: break;
      }

      CherryGUI::SameLine();
      CherryKit::CheckboxText("Show hidden", &m_ShowHidden);

      CherryGUI::SameLine();
      CherryGUI::PushItemWidth(200.0f);
      CherryGUI::InputTextWithHint("##assetfinder_search", "Search...", ProjectSearch, sizeof(ProjectSearch));
      CherryGUI::PopItemWidth();
    });

    m_AppWindow->SetRightBottombarCallback([this]() {
      CherryStyle::RemoveMarginY(10.0f);

      std::string label;

      if (m_Selected.empty()) {
        label = "Import " + m_ElementName;
      } else {
        std::filesystem::path selectedPath(m_Selected.front());
        label = "Import \"" + selectedPath.filename().string() + "\"";
      }
      std::string text = std::string("Cancel") + label;
      ImVec2 buttonSize = CherryGUI::CalcTextSize(text.c_str());

      CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - buttonSize.x - 75);

      if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
              .GetData("isClicked") == "true") {
        ClearSelection();
        m_GetFileBrowserPath = false;
        if (m_CancelCallback) {
          m_CancelCallback();
        } else {
          GetAppWindow()->SetVisibility(false);
        }
      }

      if (m_Selected.empty()) {
        CherryGUI::BeginDisabled();
      }

      if (!m_Selected.empty()) {
        Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
        Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
        Cherry::SetNextComponentProperty("color_text", "#121212FF");
      }

      CherryStyle::AddMarginY(1.0f);
      if (CherryKit::ButtonImageText(label, Cherry::GetPath("resources/imgs/add.png")).GetData("isClicked") == "true") {
        if (!m_Selected.empty()) {
          // Trigger the copy right away, don't wait for a poll elsewhere.
          if (m_ValidateCallback) {
            m_ValidateCallback(m_Selected.front());
          }
          // Kept for backward compatibility with code still polling this flag.
          m_GetFileBrowserPath = true;
        }
      }

      if (m_Selected.empty()) {
        CherryGUI::EndDisabled();
      }
    });

    m_BaseDirectory = start_path;
    m_CurrentDirectory = m_BaseDirectory.string();

    AssetFinderChild sidebar("RenderSideBar", [this]() { RenderSideBar(); }, 220.0f);
    sidebar.Enable();
    sidebar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");
    AddChild(sidebar);

    AssetFinderChild contentbar("RenderContentBar", [this]() { RenderContentBar(); });
    contentbar.Enable();
    AddChild(contentbar);
  }

  void AssetFinder::DrawPathBar(const std::string &path) {
    CherryGUI::BeginChild("PathBar", ImVec2(0, 30), false);
#ifdef _WIN32
    const char separator = '\\';
#else
    const char separator = '/';
#endif

    std::vector<std::string> elements;
    std::stringstream ss(path);
    std::string segment;

    while (std::getline(ss, segment, separator)) {
      elements.push_back(segment);
    }

    for (size_t i = 0; i < elements.size(); ++i) {
      CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", elements[i].c_str());

      if (i < elements.size() - 1) {
        CherryGUI::SameLine(0, 5.0f);
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        CherryGUI::TextUnformatted("/");
        CherryGUI::PopStyleColor();
        CherryGUI::SameLine(0, 5.0f);
      }
    }

    CherryGUI::EndChild();
  }

  void AssetFinder::AddChild(const AssetFinderChild &child) {
    m_Childs.push_back(child);
  }

  void AssetFinder::GoBack() {
    if (!m_BackHistory.empty()) {
      m_ForwardHistory.push(m_CurrentDirectory);
      m_CurrentDirectory = m_BackHistory.top().string();
      m_BackHistory.pop();
    }
  }

  void AssetFinder::GoForward() {
    if (!m_ForwardHistory.empty()) {
      m_BackHistory.push(m_CurrentDirectory);
      m_CurrentDirectory = m_ForwardHistory.top().string();
      m_ForwardHistory.pop();
    }
  }

  void AssetFinder::ChangeDirectory(const std::filesystem::path &newDirectory) {
    if (newDirectory != m_CurrentDirectory) {
      if (!m_CurrentDirectory.empty()) {
        m_BackHistory.push(m_CurrentDirectory);

        while (!m_ForwardHistory.empty()) {
          m_ForwardHistory.pop();
        }
      }
      m_CurrentDirectory = newDirectory.string();
      ClearSelection();
    }
  }

  void AssetFinder::SelectSingle(const std::string &path) {
    m_Selected.clear();
    m_Selected.push_back(path);
  }

  void AssetFinder::ClearSelection() {
    m_Selected.clear();
  }

  std::string AssetFinder::GetSingleSelected() const {
    return m_Selected.empty() ? std::string() : m_Selected.front();
  }

  bool AssetFinder::IsHidden(const std::filesystem::path &path) const {
    std::string filename = path.filename().string();
    return !filename.empty() && filename[0] == '.';
  }

  void AssetFinder::DrawFolderIcon(ImVec2 pos, ImVec2 size, ImU32 color) {
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

  void AssetFinder::FolderButton(const char *id, ImVec2 size, ImU32 color, const std::string &path) {
    if (CherryKit::WidgetFolder(Cherry::ImU32ToHex(color), size.x, size.y - 2).GetData("isDoubleClicked") == "true") {
      ChangeDirectory(path);
    }
  }

  void AssetFinder::DrawHierarchy(std::filesystem::path path, bool isDir, const std::string &label) {
    if (!isDir)
      return;

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 12.0f);

    std::string uniqueID = path.string() + "###treenode";

    std::string tree_label = label.empty() ? path.filename().string() + "###" + uniqueID + label + path.string()
                                           : label + "###" + uniqueID + label + path.string();

    ImVec2 pos = CherryGUI::GetCursorScreenPos();

    bool hidden = IsHidden(path);
    ImU32 baseColor = Cherry::HexToImU32(GetFileBrowserFolderColor(path.string()));
    ImU32 folderColor = hidden ? DarkenColor(baseColor, 0.45f) : baseColor;

    CherryGUI::SetItemAllowOverlap();
    CherryKit::WidgetSmallFolder(Cherry::ImU32ToHex(folderColor), 12.0f, 10.0f);

    CherryGUI::SameLine();

    if (hidden) {
      CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
    }

    if (CherryGUI::TreeNode(tree_label.c_str())) {
      if (hidden) {
        CherryGUI::PopStyleColor();
      }

      ChangeDirectory(path);

      try {
        std::vector<std::filesystem::directory_entry> entries;
        for (auto &dirEntry : std::filesystem::directory_iterator(path)) {
          if (dirEntry.is_directory()) {
            entries.push_back(dirEntry);
          }
        }

        std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
          return a.path().filename() < b.path().filename();
        });

        for (const auto &dirEntry : entries) {
          try {
            const std::filesystem::path &otherPath = dirEntry.path();
            DrawHierarchy(otherPath, dirEntry.is_directory());
          } catch (const std::exception &e) {
            std::cerr << "Error while display the directory " << dirEntry.path() << " - " << e.what() << std::endl;
            continue;
          }
        }
      } catch (const std::exception &e) {
        std::cerr << "Error while display the directory " << path << " - " << e.what() << std::endl;
      }

      CherryGUI::TreePop();
    } else if (hidden) {
      CherryGUI::PopStyleColor();
    }
  }

  void AssetFinder::RenderSideBar() {
    CherryKit::SeparatorText("Install Target");
    CherryNextComponent.SetProperty("size_x", CherryGUI::GetContentRegionAvail().x);

    CherryKit::SeparatorText("Quick Access");

    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);

    auto quickAccessButton = [this](const char *label, const char *icon, const std::string &xdgName) {
      CherryNextProp("color_bg", "#00000000");
      CherryNextProp("color_border", "#00000000");
      CherryNextProp("padding_x", "2");
      CherryNextProp("padding_y", "2");
      CherryNextProp("size_x", "20");
      CherryNextProp("size_y", "20");
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
      if (CherryKit::ButtonImageText(label, Cherry::GetPath(icon)).GetData("isClicked") == "true") {
        m_CurrentDirectory = GetUserDirectory(xdgName);
        ClearSelection();
      }
    };

    quickAccessButton("Home", "resources/imgs/icons/misc/icon_home.png", "");
    quickAccessButton("Desktop", "resources/imgs/icons/misc/icon_desktop.png", "DESKTOP");
    quickAccessButton("Downloads", "resources/imgs/icons/misc/icon_star.png", "DOWNLOAD");
    quickAccessButton("Documents", "resources/imgs/icons/misc/icon_documents.png", "DOCUMENTS");
    quickAccessButton("Pictures", "resources/imgs/icons/misc/icon_pictures.png", "PICTURES");
    quickAccessButton("Music", "resources/imgs/icons/misc/icon_music.png", "MUSIC");

    CherryKit::SeparatorText("Custom Folders");
    Cherry::SetNextComponentProperty("size_x", "220.0f");
    Cherry::SetNextComponentProperty("padding_y", "4.0f");
    CherryKit::HeaderImageTextButton(
        "Favorite", Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"), [this]() {
          for (auto custom_dir : m_FavoriteFolders) {
            DrawHierarchy(custom_dir, true);
          }
        });

    Cherry::SetNextComponentProperty("size_x", "220.0f");
    Cherry::SetNextComponentProperty("padding_y", "4.0f");
    CherryKit::HeaderImageTextButton(
        "Main", Cherry::Application::CookPath("resources/imgs/icons/misc/icon_home.png"), [this]() {
          DrawHierarchy(m_BaseDirectory, true, "Main");
        });

    Cherry::SetNextComponentProperty("size_x", "220.0f");
    CherryKit::HeaderImageTextButton(
        "Pools & Collections", Cherry::Application::CookPath("resources/imgs/icons/misc/icon_collection.png"), [this]() {
          CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 2.0f));

          CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));
          if (!pool_add_mode) {
            if (CherryGUI::ImageButtonWithText(
                    Cherry::Application::Get().GetCurrentRenderedWindow()->GetTexture(
                        "/usr/local/include/Vortex/imgs/vortex.png"),
                    "Add pool",
                    ImVec2(0, 0),
                    ImVec2(0, 0),
                    ImVec2(1, 1),
                    -1,
                    ImVec4(0, 0, 0, 0),
                    ImVec4(1, 1, 1, 1))) {
              pool_add_mode = true;
            }
          } else {
            CherryGUI::Text("Please enter a path");
            CherryGUI::SetNextItemWidth(-FLT_MIN);
            CherryGUI::InputText("###AddPool", pool_add_path, sizeof(pool_add_path));
            if (CherryGUI::ImageButtonWithText(
                    Cherry::Application::Get().GetCurrentRenderedWindow()->GetTexture(
                        "/usr/local/include/Vortex/imgs/vortex.png"),
                    "Add",
                    ImVec2(0, 0),
                    ImVec2(0, 0),
                    ImVec2(1, 1),
                    -1,
                    ImVec4(0, 0, 0, 0),
                    ImVec4(1, 1, 1, 1))) {
              pool_add_mode = false;
            }
            CherryGUI::SameLine();
            if (CherryGUI::ImageButtonWithText(
                    Cherry::Application::Get().GetCurrentRenderedWindow()->GetTexture(
                        "/usr/local/include/Vortex/imgs/vortex.png"),
                    "Cancel",
                    ImVec2(0, 0),
                    ImVec2(0, 0),
                    ImVec2(1, 1),
                    -1,
                    ImVec4(0, 0, 0, 0),
                    ImVec4(1, 1, 1, 1))) {
              pool_add_mode = false;
            }
          }
          CherryGUI::PopStyleVar();
          CherryGUI::PopStyleColor();
        });

    CherryGUI::PopStyleVar(3);

    for (auto custom_dir : m_Pools) {
      std::size_t lastSlashPos = custom_dir.find_last_of("/\\");
      std::string name = custom_dir.substr(lastSlashPos + 1);
      DrawHierarchy(custom_dir, true, name);
    }
  }

  bool AssetFinder::DrawItemCard(const AssetFinderItemParams &p) {
    bool pressed = false;

    float logoSize = 60.0f;
    float extraHeight = 80.0f;
    float pad = 10.0f;
    float separatorHeight = 2.0f;
    float textOffsetY = 5.0f;
    float versionBoxWidth = 10.0f;
    float versionBoxHeight = 20.0f;
    float thumbnailIconOffsetY = 30.0f;

    ImU32 bgColor = p.style.bg_color;
    ImU32 borderColor = p.style.border_color;

    if (p.selected) {
      bgColor = IM_COL32(80, 80, 240, 255);
      borderColor = IM_COL32(150, 150, 255, 255);
    }

    ImVec2 squareSize(logoSize, logoSize);

    std::string truncatedText = p.content.name;
    if (CherryGUI::CalcTextSize(p.content.name.c_str()).x > p.style.max_text_width) {
      truncatedText = p.content.name.substr(0, 20);
      if (CherryGUI::CalcTextSize(truncatedText.c_str()).x > p.style.max_text_width) {
        truncatedText = p.content.name.substr(0, 10) + "\n" + p.content.name.substr(10, 10);
      }
    } else {
      truncatedText = p.content.name + "\n";
    }

    ImVec2 fixedSize(p.style.max_text_width + pad * 2, logoSize + extraHeight + pad * 2);
    ImVec2 cursorPos = CherryGUI::GetCursorScreenPos();

    std::string button_id = "AssetFinderCard_" + p.content.path;
    if (CherryGUI::InvisibleButton(button_id.c_str(), fixedSize)) {
      pressed = true;
    }

    if (CherryGUI::IsItemHovered()) {
      CherryGUI::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    if (CherryGUI::IsItemHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
      SelectSingle(p.content.path);
    }

    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (CherryGUI::BeginPopupContextItem("AssetFinderItemContext")) {
      if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
        SelectSingle(p.content.path);
        pathToRename = p.content.path;
        strncpy(pathRename, p.content.name.c_str(), sizeof(pathRename));
        pathRename[sizeof(pathRename) - 1] = '\0';
      }
      if (CherryGUI::MenuItem("Copy", "Ctrl + C")) {
        if (m_CopyPathsCallback) {
          m_CopyPathsCallback({ p.content.path });
        }
      }
      if (CherryGUI::MenuItem("Delete", "Suppr")) {
        if (m_DeletePathCallback) {
          m_DeletePathCallback(p.content.path);
        }
      }
      CherryGUI::EndPopup();
    }

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);

    ImDrawList *drawList = CherryGUI::GetWindowDrawList();

    drawList->AddRectFilled(
        cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y), bgColor, p.style.border_radius);
    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + thumbnailIconOffsetY + squareSize.y),
        IM_COL32(26, 26, 26, 255),
        p.style.border_radius,
        ImDrawFlags_RoundCornersTop);
    drawList->AddRect(
        cursorPos,
        ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
        borderColor,
        p.style.border_radius,
        0,
        1.0f);

    ImVec2 logoPos(cursorPos.x + (fixedSize.x - squareSize.x) / 2, cursorPos.y + pad);
    ImTextureID logoTexture = Cherry::Application::GetCurrentRenderedWindow()->GetTexture(p.content.logo);
    drawList->AddImage(logoTexture, logoPos, ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

    ImVec2 sizePos(cursorPos.x + pad, cursorPos.y + squareSize.y + thumbnailIconOffsetY - 20 + textOffsetY);
    CherryGUI::SetCursorScreenPos(sizePos);
    CherryGUI::GetFont()->Scale = 0.7f;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(p.style.max_text_width);
    CherryGUI::TextWrapped("%s", p.content.size.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();
    CherryGUI::GetFont()->Scale = 1.0f;
    CherryGUI::PopFont();

    ImVec2 lineStart(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
    ImVec2 lineEnd(cursorPos.x + fixedSize.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY + separatorHeight);
    drawList->AddLine(lineStart, lineEnd, p.style.line_color, separatorHeight);

    ImVec2 textPos(cursorPos.x + pad, cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
    CherryGUI::SetCursorScreenPos(textPos);
    CherryGUI::GetFont()->Scale = 0.9f;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushItemWidth(p.style.max_text_width);

    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
    ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);

    if (pathToRename == p.content.path) {
      CherryGUI::SetItemAllowOverlap();
      CherryGUI::PushID(p.content.path.c_str());
      bool done = CherryGUI::InputText("", pathRename, sizeof(pathRename), ImGuiInputTextFlags_EnterReturnsTrue);
      if (done || CherryGUI::IsItemDeactivatedAfterEdit()) {
        // vxe::submit_rename(pathToRename, pathRename);
        pathToRename = "";
      }
      CherryGUI::PopID();
    } else {
      DrawHighlightedText(
          drawList, textPos, truncatedText.c_str(), ProjectSearch, highlightColor, textColor, highlightTextColor);
    }

    CherryGUI::PopItemWidth();
    CherryGUI::GetFont()->Scale = 1.0f;
    CherryGUI::PopFont();

    ImVec2 descriptionPos(cursorPos.x + pad, cursorPos.y + squareSize.y + thumbnailIconOffsetY + 35 + textOffsetY);
    CherryGUI::SetCursorScreenPos(descriptionPos);
    CherryGUI::GetFont()->Scale = 0.7f;
    CherryGUI::PushFont(CherryGUI::GetFont());
    CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    CherryGUI::PushItemWidth(p.style.max_text_width);
    CherryGUI::TextWrapped("%s", p.content.description.c_str());
    CherryGUI::PopItemWidth();
    CherryGUI::PopStyleColor();
    CherryGUI::GetFont()->Scale = 1.0f;
    CherryGUI::PopFont();

    ImVec2 smallRectPos(
        cursorPos.x + fixedSize.x - versionBoxWidth - pad, cursorPos.y + fixedSize.y - versionBoxHeight - pad);
    drawList->AddRectFilled(
        smallRectPos,
        ImVec2(smallRectPos.x + versionBoxWidth, smallRectPos.y + versionBoxHeight),
        IM_COL32(0, 0, 0, 255),
        p.style.border_radius);

    float windowVisibleX2 = CherryGUI::GetWindowPos().x + CherryGUI::GetWindowContentRegionMax().x;
    if (cursorPos.x + fixedSize.x < windowVisibleX2)
      CherryGUI::SameLine();

    CherryGUI::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + pad));

    return pressed;
  }

  void AssetFinder::RenderContentBarThumbnails() {
    float cellSize = thumbnailSize + padding;
    float panelWidth = CherryGUI::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1)
      columnCount = 1;

    if (!std::filesystem::exists(m_CurrentDirectory)) {
      return;
    }

    std::vector<std::filesystem::directory_entry> directories;
    std::vector<std::filesystem::directory_entry> files;

    std::vector<std::pair<std::shared_ptr<AssetFinderItem>, std::string>> recognized_modules_items;

    for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
      if (directoryEntry.is_directory()) {
        bool recognized = false;
        std::string path = directoryEntry.path().string();
        for (auto &item : m_ItemToReconize) {
          if (item->f_Detect && item->f_Detect(path)) {
            recognized_modules_items.push_back({ item, path });
            recognized = true;
            break;
          }
        }
        if (!recognized) {
          directories.push_back(directoryEntry);
        }
      } else {
        files.push_back(directoryEntry);
      }
    }

    std::sort(directories.begin(), directories.end(), [](const auto &a, const auto &b) {
      return a.path().filename().string() < b.path().filename().string();
    });
    std::sort(files.begin(), files.end(), [](const auto &a, const auto &b) {
      return a.path().filename().string() < b.path().filename().string();
    });

    CherryGUI::Columns(columnCount, 0, false);

    if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
      ClearSelection();
      CherryGUI::OpenPopup("AssetFinderEmptySpacePopup");
    }
    if (CherryGUI::IsWindowHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Left) &&
        !CherryGUI::IsAnyItemHovered()) {
      ClearSelection();
    }

    if (CherryGUI::BeginPopup("AssetFinderEmptySpacePopup")) {
      if (CherryGUI::MenuItem("Paste") && m_PastePathsCallback) {
        m_PastePathsCallback({ m_CurrentDirectory });
      }
      CherryGUI::EndPopup();
    }

    // Folders
    if (m_ShowFolders) {
      for (auto &directoryEntry : directories) {
        const auto &path = directoryEntry.path();
        std::string filenameString = path.filename().string();
        bool hidden = IsHidden(path);

        if (!m_ShowHidden && hidden)
          continue;

        if (!(areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch)))
          continue;

        CherryGUI::PushID(filenameString.c_str());

        float reducedThumbnailSize = thumbnailSize * 0.9f;
        float availableWidth = CherryGUI::GetContentRegionAvail().x;

        bool selected = (GetSingleSelected() == path.string());

        if (selected) {
          ImVec2 cellPos = CherryGUI::GetCursorScreenPos();
          ImVec2 cellSize(availableWidth, reducedThumbnailSize + 30.0f);
          CherryGUI::GetWindowDrawList()->AddRectFilled(
              cellPos, ImVec2(cellPos.x + cellSize.x, cellPos.y + cellSize.y), IM_COL32(40, 90, 220, 80), 4.0f);
        }

        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        CherryGUI::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        CherryGUI::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        ImVec2 folderSize(reducedThumbnailSize, reducedThumbnailSize);
        float folderPosX = (availableWidth - folderSize.x) * 0.5f;
        CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + folderPosX);

        ImU32 baseColor = Cherry::HexToImU32(GetFileBrowserFolderColor(path.string()));
        ImU32 folderColor = hidden ? DarkenColor(baseColor, 0.45f) : baseColor;

        FolderButton("folder_icon", folderSize, folderColor, path.string());

        if (CherryGUI::IsItemClicked(ImGuiMouseButton_Left)) {
          SelectSingle(path.string());
        }
        if (CherryGUI::IsItemHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
          SelectSingle(path.string());
        }

        CherryGUI::PopStyleVar(2);
        CherryGUI::PopStyleColor(3);

        if (CherryGUI::BeginPopupContextItem("AssetFinderFolderContext")) {
          if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
            pathToRename = path.string();
            strncpy(pathRename, filenameString.c_str(), sizeof(pathRename));
            pathRename[sizeof(pathRename) - 1] = '\0';
          }
          if (CherryGUI::MenuItem("Delete", "Suppr")) {
            if (m_DeletePathCallback) {
              m_DeletePathCallback(path.string());
            }
          }
          CherryGUI::EndPopup();
        }

        float textWidth = CherryGUI::CalcTextSize(filenameString.c_str()).x;
        float textOffsetX = (availableWidth - textWidth) * 0.5f;
        CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + textOffsetX);

        if (hidden) {
          CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
        }
        CherryGUI::TextWrapped("%s", filenameString.c_str());
        if (hidden) {
          CherryGUI::PopStyleColor();
        }

        CherryGUI::PopID();
        CherryGUI::NextColumn();
      }
    }

    // Recognized module items
    if (m_ShowItems) {
      for (auto &itemEntry : recognized_modules_items) {
        const auto &path = itemEntry.second;
        std::filesystem::path fsPath(path);
        std::string filenameString = fsPath.filename().string();

        if (!(areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch)))
          continue;

        std::uintmax_t folderSize = getDirectorySize(path);
        std::string folderSizeString = formatFileSize(folderSize);

        CherryGUI::PushID(filenameString.c_str());

        AssetFinderItemParams p;
        p.content.name = filenameString;
        p.content.path = path;
        p.content.description = itemEntry.first->m_Description;
        p.content.size = folderSizeString;
        p.content.logo = Cherry::Application::CookPath("resources/imgs/icons/files/icon_picture_file.png");
        p.content.item_ident = itemEntry.first;
        p.style.line_color = IM_COL32(
            (int)itemEntry.first->m_LineColor.x,
            (int)itemEntry.first->m_LineColor.y,
            (int)itemEntry.first->m_LineColor.z,
            (int)itemEntry.first->m_LineColor.w);
        p.selected = (GetSingleSelected() == path);

        if (DrawItemCard(p)) {
          SelectSingle(path);
        }

        CherryGUI::PopID();
        CherryGUI::NextColumn();
      }
    }

    // Regular files (hidden ones drawn darker)
    if (m_ShowFiles) {
      for (auto &fileEntry : files) {
        const auto &path = fileEntry.path();
        std::string filenameString = path.filename().string();
        bool hidden = IsHidden(path);

        if (!m_ShowHidden && hidden)
          continue;

        if (!(areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch)))
          continue;

        std::error_code ec;
        size_t fileSize = fileEntry.is_regular_file(ec) ? std::filesystem::file_size(path, ec) : 0;
        std::string fileSizeString = formatFileSize(fileSize);

        FileTypes type = detect_file(path.string());
        const auto &info = GetFileTypeInfo(type);

        CherryGUI::PushID(filenameString.c_str());

        AssetFinderItemParams p;
        p.content.name = filenameString;
        p.content.path = path.string();
        p.content.description = info.label;
        p.content.size = fileSizeString;
        p.content.logo = info.icon;
        p.style.line_color = info.color;
        p.selected = (GetSingleSelected() == path.string());

        if (hidden) {
          p.style.bg_color = DarkenColor(p.style.bg_color, 0.35f);
        }

        if (DrawItemCard(p)) {
          SelectSingle(path.string());
        }

        CherryGUI::PopID();
        CherryGUI::NextColumn();
      }
    }

    CherryGUI::Columns(1);
  }

  void AssetFinder::RenderContentBarList() {
    if (!std::filesystem::exists(m_CurrentDirectory)) {
      return;
    }

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    if (!CherryGUI::BeginTable("AssetFinderTable", 4, flags)) {
      return;
    }

    CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
    CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 90.0f);
    CherryGUI::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 90.0f);
    CherryGUI::TableSetupColumn("Last Updated", ImGuiTableColumnFlags_WidthFixed, 130.0f);
    CherryGUI::TableHeadersRow();

    auto getModifiedTime = [](const std::filesystem::path &path) -> std::time_t {
      std::error_code ec;
      auto ftime = std::filesystem::last_write_time(path, ec);
      if (ec)
        return 0;
      auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
          ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
      return std::chrono::system_clock::to_time_t(sctp);
    };

    auto drawRow = [&](const std::string &filename,
                       const std::filesystem::path &path,
                       bool isFolder,
                       size_t size,
                       std::time_t cftime,
                       const std::string &logo,
                       const std::string &typeLabel,
                       bool hidden) {
      float rowHeight = 20.0f;
      CherryGUI::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
      CherryGUI::TableSetColumnIndex(0);

      bool selected = (GetSingleSelected() == path.string());

      if (hidden) {
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
      }

      if (CherryGUI::Selectable(
              ("##row_" + path.string()).c_str(),
              selected,
              ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick,
              ImVec2(0, rowHeight))) {
        SelectSingle(path.string());

        if (CherryGUI::IsMouseDoubleClicked(0) && isFolder) {
          ChangeDirectory(path);
        }
      }

      if (CherryGUI::IsItemHovered() && CherryGUI::IsMouseReleased(ImGuiMouseButton_Right)) {
        SelectSingle(path.string());
      }

      if (CherryGUI::BeginPopupContextItem(("AssetFinderRowContext_" + path.string()).c_str())) {
        if (CherryGUI::MenuItem("Rename", "Ctrl + R")) {
          pathToRename = path.string();
          strncpy(pathRename, filename.c_str(), sizeof(pathRename));
          pathRename[sizeof(pathRename) - 1] = '\0';
        }
        if (!isFolder && CherryGUI::MenuItem("Copy", "Ctrl + C")) {
          if (m_CopyPathsCallback) {
            m_CopyPathsCallback({ path.string() });
          }
        }
        if (CherryGUI::MenuItem("Delete", "Suppr")) {
          if (m_DeletePathCallback) {
            m_DeletePathCallback(path.string());
          }
        }
        CherryGUI::EndPopup();
      }

      CherryGUI::SameLine();

      if (isFolder) {
        ImU32 baseColor = Cherry::HexToImU32(GetFileBrowserFolderColor(path.string()));
        ImU32 folderColor = hidden ? DarkenColor(baseColor, 0.45f) : baseColor;
        DrawFolderIcon(CherryGUI::GetCursorScreenPos(), ImVec2(16, 16), folderColor);
        CherryGUI::Dummy(ImVec2(20, 16));
      } else {
        ImTextureID tex = Cherry::Application::GetCurrentRenderedWindow()->GetTexture(logo);
        if (tex) {
          CherryGUI::Image(tex, ImVec2(18, 18));
        }
      }
      CherryGUI::SameLine();

      ImU32 textColor = IM_COL32(255, 255, 255, 255);
      ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
      ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
      static std::unordered_map<std::string, char[256]> renameBuffers;

      if (pathToRename == path.string()) {
        CherryGUI::PushID(path.string().c_str());
        auto &buffer = renameBuffers[path.string()];
        if (buffer[0] == '\0') {
          std::strncpy(buffer, filename.c_str(), sizeof(buffer));
          buffer[sizeof(buffer) - 1] = '\0';
          CherryGUI::SetKeyboardFocusHere();
        }
        bool done = CherryGUI::InputText("##rename_input", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue);
        if (done || CherryGUI::IsItemDeactivatedAfterEdit()) {
          // vxe::submit_rename(pathToRename, buffer);
          renameBuffers.erase(path.string());
          pathToRename = "";
        }
        CherryGUI::PopID();
      } else {
        ImDrawList *drawList = CherryGUI::GetWindowDrawList();
        DrawHighlightedText(
            drawList,
            CherryGUI::GetCursorScreenPos(),
            filename.c_str(),
            ProjectSearch,
            highlightColor,
            textColor,
            highlightTextColor);
        CherryGUI::Dummy(CherryGUI::CalcTextSize(filename.c_str()));
      }

      if (hidden) {
        CherryGUI::PopStyleColor();
      }

      CherryGUI::TableSetColumnIndex(1);
      CherryGUI::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", typeLabel.c_str());

      CherryGUI::TableSetColumnIndex(2);
      CherryGUI::TextUnformatted(isFolder ? "---" : formatFileSize(size).c_str());

      CherryGUI::TableSetColumnIndex(3);
      if (cftime != 0) {
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", std::localtime(&cftime));
        CherryGUI::TextUnformatted(buffer);
      } else {
        CherryGUI::TextUnformatted("---");
      }
    };

    if (!m_CurrentDirectory.empty() && m_CurrentDirectory != m_BaseDirectory.string()) {
      std::filesystem::path parent = std::filesystem::path(m_CurrentDirectory).parent_path();
      drawRow("...", parent, true, 0, 0, "", "Folder", false);
    }

    std::vector<std::filesystem::directory_entry> directories;
    std::vector<std::filesystem::directory_entry> files;

    std::vector<std::pair<std::shared_ptr<AssetFinderItem>, std::string>> recognized_modules_items;

    for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
      if (directoryEntry.is_directory()) {
        bool recognized = false;
        std::string path = directoryEntry.path().string();
        for (auto &item : m_ItemToReconize) {
          if (item->f_Detect && item->f_Detect(path)) {
            recognized_modules_items.push_back({ item, path });
            recognized = true;
            break;
          }
        }
        if (!recognized) {
          directories.push_back(directoryEntry);
        }
      } else {
        files.push_back(directoryEntry);
      }
    }

    std::sort(directories.begin(), directories.end(), [](const auto &a, const auto &b) {
      return a.path().filename().string() < b.path().filename().string();
    });
    std::sort(files.begin(), files.end(), [](const auto &a, const auto &b) {
      return a.path().filename().string() < b.path().filename().string();
    });

    if (m_ShowFolders) {
      for (auto &directoryEntry : directories) {
        const auto &path = directoryEntry.path();
        std::string filenameString = path.filename().string();
        bool hidden = IsHidden(path);
        if (!m_ShowHidden && hidden)
          continue;
        if (!(areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch)))
          continue;

        drawRow(filenameString, path, true, 0, getModifiedTime(path), "", "Folder", hidden);
      }
    }

    if (m_ShowItems) {
      for (auto &itemEntry : recognized_modules_items) {
        std::filesystem::path path(itemEntry.second);
        std::string filenameString = path.filename().string();
        if (!(areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch)))
          continue;

        std::uintmax_t sz = getDirectorySize(path);
        drawRow(
            filenameString,
            path,
            false,
            sz,
            getModifiedTime(path),
            Cherry::Application::CookPath("resources/imgs/icons/files/icon_picture_file.png"),
            itemEntry.first->m_Description,
            false);
      }
    }

    if (m_ShowFiles) {
      for (auto &fileEntry : files) {
        const auto &path = fileEntry.path();
        std::string filenameString = path.filename().string();
        bool hidden = IsHidden(path);
        if (!m_ShowHidden && hidden)
          continue;
        if (!(areStringsSimilar(filenameString, ProjectSearch, threshold) || isOnlySpacesOrEmpty(ProjectSearch)))
          continue;

        std::error_code ec;
        size_t fsize = fileEntry.is_regular_file(ec) ? std::filesystem::file_size(path, ec) : 0;

        FileTypes type = detect_file(path.string());
        const auto &info = GetFileTypeInfo(type);

        drawRow(filenameString, path, false, fsize, getModifiedTime(path), info.icon, info.label, hidden);
      }
    }

    CherryGUI::EndTable();
  }

  void AssetFinder::RenderContentBar() {
    ImGuiStyle &style = CherryGUI::GetStyle();
    ImVec4 originalChildBgColor = style.Colors[ImGuiCol_ChildBg];
    ImVec4 originalBorderColor = style.Colors[ImGuiCol_Border];
    ImVec4 originalBorderShadowColor = style.Colors[ImGuiCol_BorderShadow];

    style.Colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    if (m_ShowMode == AssetFinderShowMode::Thumbnails) {
      RenderContentBarThumbnails();
    } else {
      RenderContentBarList();
    }

    style.Colors[ImGuiCol_ChildBg] = originalChildBgColor;
    style.Colors[ImGuiCol_Border] = originalBorderColor;
    style.Colors[ImGuiCol_BorderShadow] = originalBorderShadowColor;
  }

  std::shared_ptr<Cherry::AppWindow> &AssetFinder::GetAppWindow() {
    return m_AppWindow;
  }

  std::shared_ptr<AssetFinder> AssetFinder::Create(const std::string &name, const std::string &base_path) {
    auto instance = std::shared_ptr<AssetFinder>(new AssetFinder(name, base_path));
    instance->SetupRenderCallback();
    return instance;
  }

  void AssetFinder::SetupRenderCallback() {
    auto self = shared_from_this();
    m_AppWindow->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  void AssetFinder::Render() {
    const float splitterWidth = 4.0f;
    const float minChildWidth = 80.0f;

    auto &children = m_Childs;
    ImVec2 availableSize = CherryGUI::GetContentRegionAvail();

    if (!m_ChildsInitialized) {
      float totalAvailableWidth = availableSize.x - ((float)children.size() - 1.0f) * splitterWidth;

      float totalDefault = 0.0f;
      int childrenWithoutDefault = 0;
      for (auto &child : children) {
        if (child.m_DefaultSize > 0.0f) {
          totalDefault += child.m_DefaultSize;
        } else {
          childrenWithoutDefault++;
        }
      }

      float remaining = (std::max)(totalAvailableWidth - totalDefault, 0.0f);

      for (auto &child : children) {
        if (child.m_DefaultSize > 0.0f) {
          child.m_Size = child.m_DefaultSize;
        } else {
          child.m_Size = childrenWithoutDefault > 0 ? remaining / childrenWithoutDefault : remaining;
        }
      }

      m_ChildsInitialized = true;
    }

    float totalChildSize = 0.0f;
    for (auto &child : children) {
      totalChildSize += child.m_Size;
    }
    totalChildSize += ((float)children.size() - 1.0f) * splitterWidth;

    if (totalChildSize > availableSize.x && totalChildSize > 0.0f) {
      float scaleFactor = availableSize.x / totalChildSize;
      for (auto &child : children) {
        child.m_Size = (std::max)(child.m_Size * scaleFactor, minChildWidth);
      }
    } else if (totalChildSize < availableSize.x) {
      float extraSpace = availableSize.x - totalChildSize;
      float totalResizable = 0.0f;
      for (auto &child : children) {
        totalResizable += child.m_Size;
      }
      if (totalResizable > 0.0f) {
        for (auto &child : children) {
          float ratio = child.m_Size / totalResizable;
          child.m_Size += ratio * extraSpace;
        }
      }
    }

    for (size_t i = 0; i < children.size(); ++i) {
      auto &child = children[i];

      CherryGUI::PushStyleColor(ImGuiCol_ChildBg, child.m_BackgroundColor);
      CherryGUI::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

      CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 0.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 0.0f));

      std::string childname = child.m_Name + "##af_child_" + m_AppWindow->m_Name;

      CherryGUI::BeginChild(childname.c_str(), ImVec2(child.m_Size, availableSize.y), true);

      child.m_Child();

      CherryGUI::EndChild();
      CherryGUI::PopStyleVar(4);
      CherryGUI::PopStyleColor(2);

      if (i + 1 < children.size()) {
        auto &next_child = children[i + 1];

        CherryGUI::SameLine();

        std::string splitterId = child.m_Name + m_AppWindow->m_Name + "##af_splitter";
        CherryGUI::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        CherryGUI::Button(splitterId.c_str(), ImVec2(splitterWidth, -1));
        CherryGUI::PopStyleColor();

        if (CherryGUI::IsItemHovered()) {
          CherryGUI::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (CherryGUI::IsItemActive()) {
          float delta = CherryGUI::GetIO().MouseDelta.x;

          if ((child.m_Size >= minChildWidth || delta > 0.0f) && (next_child.m_Size >= minChildWidth || delta < 0.0f)) {
            child.m_Size += delta;
            next_child.m_Size -= delta;
          }

          child.m_Size = (std::max)(child.m_Size, minChildWidth);
          next_child.m_Size = (std::max)(next_child.m_Size, minChildWidth);
        }

        CherryGUI::SameLine();
      }
    }
  }
}  // namespace vxe