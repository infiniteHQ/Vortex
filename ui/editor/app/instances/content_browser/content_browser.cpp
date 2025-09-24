#include "./content_browser.hpp"

using namespace Cherry;

static float padding = 30.0f;
static float thumbnailSize = 94.0f;

static std::string pathToRename = "";
static std::string pathRename;

static bool pool_add_mode = false;
static char pool_add_path[512];
static char pool_add_name[512];

static bool ShowViewModal = false;
static std::vector<std::shared_ptr<ItemHandlerInterface>> item_handles;
static std::vector<std::pair<std::string, std::string>> item_paths;
static bool item_context_menu_opened = false;

static std::string _parent;
static std::string ProjectSearch;
static float threshold = 0.4;

static ImU32 folder_color = IM_COL32(150, 128, 50, 255);

static std::pair<std::string, ImU32> current_editing_folder;
static bool current_editing_folder_is_favorite;

static float c_SideBarWidth = 250.0f;
static float c_FilterBarWidth = 250.0f;

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

static void DrawHighlightedText(ImDrawList *drawList, ImVec2 textPos,
                                const char *text, const char *search,
                                ImU32 highlightColor, ImU32 textColor,
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
      textPos.x += ImGui::CalcTextSize(preText.c_str()).x;
    }

    ImVec2 highlightPos = textPos;
    ImVec2 highlightSize = ImGui::CalcTextSize(search);
    drawList->AddRectFilled(highlightPos,
                            ImVec2(highlightPos.x + highlightSize.x,
                                   highlightPos.y + highlightSize.y),
                            highlightColor);
    drawList->AddText(textPos, highlightTextColor, search);
    textPos.x += highlightSize.x;

    start = found + strlen(search);
    found = strstr(start, search);
  }

  if (*start) {
    drawList->AddText(textPos, textColor, start);
  }
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
        dp[i][j] = (std::min)({dp[i - 1][j] + 1, dp[i][j - 1] + 1,
                               dp[i - 1][j - 1] + cost});
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

static bool areStringsSimilar(const std::string &s1, const std::string &s2,
                              double threshold) {
  std::string lower_s1 = toLowerCase(s1);
  std::string lower_s2 = toLowerCase(s2);

  int dist = levenshteinDistance(lower_s1, lower_s2);
  int maxLength = (std::max)(lower_s1.size(), lower_s2.size());
  double similarity = 1.0 - (static_cast<double>(dist) / maxLength);

  if (std::strlen(ProjectSearch.c_str()) < 5) {
    return similarity >= threshold || hasCommonLetters(lower_s1, lower_s2);
  }

  return similarity >= threshold;
}
bool Splitter(bool split_vertically, float thickness, float *sizebefore,
              float *size, float *size_after, float min_size1,
              float min_size2) {
  using namespace ImGui;
  ImGuiContext &g = *GImGui;
  ImGuiWindow *window = g.CurrentWindow;
  ImDrawList *draw_list = GetWindowDrawList();

  ImVec2 backup_pos = window->DC.CursorPos;
  if (split_vertically)
    window->DC.CursorPos =
        ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y);
  else
    window->DC.CursorPos =
        ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y);

  PushID("#Splitter");
  InvisibleButton("##Splitter",
                  ImVec2(split_vertically ? thickness : window->Size.x,
                         split_vertically ? window->Size.y : thickness));
  PopID();

  bool hovered = IsItemHovered();
  bool held = IsItemActive();
  if (hovered || held)
    SetMouseCursor(split_vertically ? ImGuiMouseCursor_ResizeEW
                                    : ImGuiMouseCursor_ResizeNS);

  bool updated = false;
  if (held) {
    float mouse_delta =
        split_vertically ? g.IO.MouseDelta.x : g.IO.MouseDelta.y;
    if (mouse_delta != 0.0f) {
      if (*size + mouse_delta < min_size1)
        mouse_delta = -(*size - min_size1);
      if (*sizebefore - mouse_delta < min_size2)
        mouse_delta = *sizebefore - min_size2;

      *sizebefore += mouse_delta;
      *size -= mouse_delta;
      updated = true;
    }
  }

  window->DC.CursorPos = backup_pos;

  ImU32 color = IM_COL32(45, 45, 45, 255);
  ImVec2 pos = split_vertically ? ImVec2(backup_pos.x, backup_pos.y)
                                : ImVec2(backup_pos.x, backup_pos.y);
  ImVec2 end_pos = split_vertically
                       ? ImVec2(pos.x + thickness, pos.y + window->Size.y)
                       : ImVec2(pos.x + window->Size.x, pos.y + thickness);
  draw_list->AddRectFilled(pos, end_pos, color);

  return updated;
}

bool ColorPicker3U32(const char *label, ImU32 *color,
                     ImGuiColorEditFlags flags = 0) {
  float col[3];
  col[0] = (float)((*color >> 0) & 0xFF) / 255.0f;
  col[1] = (float)((*color >> 8) & 0xFF) / 255.0f;
  col[2] = (float)((*color >> 16) & 0xFF) / 255.0f;

  bool result = ImGui::ColorPicker3(label, col, flags);

  if (result) {
    *color = ((ImU32)(col[0] * 255.0f)) | ((ImU32)(col[1] * 255.0f) << 8) |
             ((ImU32)(col[2] * 255.0f) << 16) | ((ImU32)(255) << 24);
  }

  ImVec4 buttonColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
  ImVec4 buttonHoveredColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  ImVec4 buttonActiveColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  ImVec4 cancelButtonColor = ImVec4(0.4f, 0.2f, 0.2f, 1.0f);

  ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActiveColor);

  if (ImGui::Button("Cancel", ImVec2(75.0f, 0.0f))) {
    //
  }

  ImGui::PopStyleColor(3);

  ImGui::SameLine();

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f));

  if (ImGui::Button("Done", ImVec2(75.0f, 0.0f))) {
    *color = ImColor(col[0], col[1], col[2]);
    VortexMaker::PublishContentBrowserCustomFolder(
        current_editing_folder.first, VortexMaker::ImU32ToHex(*color),
        current_editing_folder_is_favorite);
  }

  ImGui::PopStyleColor(3);

  return result;
}

bool CollapsingHeaderWithIcon(const char *label, ImTextureID icon) {
  ImGui::PushID(label);

  bool open =
      ImGui::CollapsingHeader("##hidden", ImGuiTreeNodeFlags_AllowItemOverlap);

  ImVec2 textPos = ImGui::GetCursorPos();
  ImGui::SameLine();
  ImGui::SetCursorPosX(textPos.x + ImGui::GetStyle().FramePadding.x);

  ImGui::Image(icon, ImVec2(16, 16));
  ImGui::SameLine();

  ImGui::SetCursorPosY(textPos.y);
  ImGui::TextUnformatted(label);

  ImGui::PopID();

  return open;
}

static std::uintmax_t
getDirectorySize(const std::filesystem::path &directoryPath) {
  std::uintmax_t size = 0;

  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(directoryPath)) {
    if (std::filesystem::is_regular_file(entry.path())) {
      size += std::filesystem::file_size(entry.path());
    }
  }

  return size;
}

static std::string formatFileSize(size_t size) {
  const char *units[] = {"o", "ko", "Mo", "Go", "To"};
  int unitIndex = 0;
  double displaySize = static_cast<double>(size);

  while (displaySize >= 1024 && unitIndex < 4) {
    displaySize /= 1024;
    ++unitIndex;
  }

  char formattedSize[20];
  snprintf(formattedSize, sizeof(formattedSize), "%.2f %s", displaySize,
           units[unitIndex]);
  return std::string(formattedSize);
}

static ImU32 DarkenColor(ImU32 color, float amount) {
  int r = (color & 0xFF000000) >> 24;
  int g = (color & 0x00FF0000) >> 16;
  int b = (color & 0x0000FF00) >> 8;
  int a = color & 0x000000FF;

  r = static_cast<int>(r * (1.0f - amount));
  g = static_cast<int>(g * (1.0f - amount));
  b = static_cast<int>(b * (1.0f - amount));

  return IM_COL32(r, g, b, a);
}

namespace VortexEditor {
std::string get_extension(const std::string &path) {
  size_t dot_pos = path.find_last_of('.');
  if (dot_pos == std::string::npos)
    return "";
  return path.substr(dot_pos + 1);
}

FileTypes detect_file(const std::string &path) {
  static const std::unordered_map<std::string, FileTypes> extension_map = {
      // Very low level
      {"asm", FileTypes::File_ASM},
      {"bin", FileTypes::File_BIN},

      // Programming languages
      {"c", FileTypes::File_C},
      {"h", FileTypes::File_H},
      {"cpp", FileTypes::File_CPP},
      {"hpp", FileTypes::File_HPP},
      {"inl", FileTypes::File_INL},
      {"rs", FileTypes::File_RUST},
      {"zig", FileTypes::File_ZIG},
      {"go", FileTypes::File_GO},
      {"java", FileTypes::File_JAVA},
      {"js", FileTypes::File_JAVASCRIPT},
      {"ts", FileTypes::File_JAVASCRIPT}, // TypeScript as JavaScript
      {"cob", FileTypes::File_COBOL},
      {"p", FileTypes::File_PASCAL},
      {"pas", FileTypes::File_PASCAL},
      {"carbon", FileTypes::File_CARBON},
      {"py", FileTypes::File_PYTHON},
      {"rb", FileTypes::File_RUBY},
      {"php", FileTypes::File_PHP},
      {"swift", FileTypes::File_SWIFT},
      {"cs", FileTypes::File_CSHARP},
      {"vb", FileTypes::File_VB},
      {"dart", FileTypes::File_DART},
      {"kt", FileTypes::File_KOTLIN},
      {"scala", FileTypes::File_SCALA},
      {"pl", FileTypes::File_PERL},
      {"sh", FileTypes::File_SHELL},
      {"bat", FileTypes::File_BATCH},
      {"lua", FileTypes::File_LUA},
      {"r", FileTypes::File_R},
      {"m", FileTypes::File_MATLAB},
      {"jl", FileTypes::File_JULIA},
      {"sql", FileTypes::File_SQL},
      {"groovy", FileTypes::File_GROOVY},
      {"f90", FileTypes::File_FORTRAN},
      {"erl", FileTypes::File_ERLANG},
      {"ex", FileTypes::File_ELIXIR},
      {"ml", FileTypes::File_OCAML},

      // Web and Markup
      {"html", FileTypes::File_HTML},
      {"htm", FileTypes::File_HTML},
      {"css", FileTypes::File_CSS},
      {"scss", FileTypes::File_CSS},
      {"xml", FileTypes::File_XML},
      {"json", FileTypes::File_JSON},
      {"yaml", FileTypes::File_YAML},
      {"yml", FileTypes::File_YAML},

      // Config
      {"cfg", FileTypes::File_CFG},
      {"ini", FileTypes::File_INI},
      {"env", FileTypes::File_INI},

      // Git
      {"gitignore", FileTypes::File_GIT},
      {"gitmodules", FileTypes::File_GIT},
      {"gitattributes", FileTypes::File_GIT},

      // Documents
      {"txt", FileTypes::File_TXT},
      {"md", FileTypes::File_MD},
      {"rst", FileTypes::File_MD},
      {"doc", FileTypes::File_DOC},
      {"docx", FileTypes::File_DOC},
      {"pdf", FileTypes::File_PDF},
      {"odt", FileTypes::File_DOC},

      // Pictures
      {"png", FileTypes::File_PICTURE},
      {"jpg", FileTypes::File_PICTURE},
      {"jpeg", FileTypes::File_PICTURE},
      {"bmp", FileTypes::File_PICTURE},
      {"gif", FileTypes::File_PICTURE},
      {"svg", FileTypes::File_PICTURE},
      {"webp", FileTypes::File_PICTURE},
      {"ico", FileTypes::File_PICTURE},

      // Videos
      {"mp4", FileTypes::File_VIDEO},
      {"avi", FileTypes::File_VIDEO},
      {"mkv", FileTypes::File_VIDEO},
      {"mov", FileTypes::File_VIDEO},
      {"wmv", FileTypes::File_VIDEO},
      {"flv", FileTypes::File_VIDEO},

      // Audio
      {"mp3", FileTypes::File_AUDIO},
      {"wav", FileTypes::File_AUDIO},
      {"flac", FileTypes::File_AUDIO},
      {"aac", FileTypes::File_AUDIO},
      {"ogg", FileTypes::File_AUDIO},

      // Archives
      {"zip", FileTypes::File_ARCHIVE},
      {"rar", FileTypes::File_ARCHIVE},
      {"7z", FileTypes::File_ARCHIVE},
      {"tar", FileTypes::File_ARCHIVE},
      {"gz", FileTypes::File_ARCHIVE},
      {"bz2", FileTypes::File_ARCHIVE},

      // Miscellaneous
      {"log", FileTypes::File_LOG},
      {"bak", FileTypes::File_BACKUP},
      {"tmp", FileTypes::File_TEMP},
      {"dat", FileTypes::File_DATA},
  };

  std::string extension = get_extension(path);
  auto it = extension_map.find(extension);
  if (it != extension_map.end()) {
    return it->second;
  } else {
    return FileTypes::File_UNKNOWN;
  }
}

static std::vector<
    std::pair<std::shared_ptr<ItemIdentifierInterface>, std::string>>
    recognized_modules_items;

void ContentBrowserAppWindow::DrawPathBar(const std::string &path) {
  std::string homePath =
      VortexMaker::GetCurrentContext()->projectDataPath.string();
  bool FirstPathPartIsHome = false;
  std::string displayPath = path;

  if (path.rfind(homePath, 0) == 0) {
    FirstPathPartIsHome = true;
    displayPath = path.substr(homePath.length());
    if (!displayPath.empty() &&
        (displayPath[0] == '\\' || displayPath[0] == '/')) {
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

  float availableWidth = ImGui::GetContentRegionAvail().x;
  float synthStart = 0.5f;
  float maxWidth = (std::max)(availableWidth * synthStart, 100.0f);

  float totalWidth = 0.0f;
  const float sepWidth = ImGui::CalcTextSize("/").x;
  const float ellipsisWidth = ImGui::CalcTextSize("...").x;

  std::vector<float> widths;
  for (const auto &el : elements)
    widths.push_back(ImGui::CalcTextSize(el.c_str()).x);

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
    ImGui::Image(Cherry::GetTexture(Cherry::GetPath(
                     "resources/imgs/icons/misc/icon_home.png")),
                 ImVec2(15.0f, 15.0f));
    CherryStyle::RemoveMarginY(11.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    ImGui::TextUnformatted(separator.c_str());
    ImGui::PopStyleColor();
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
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
      CherryStyle::RemoveMarginY(12.0f);
      ImGui::TextUnformatted("...");
      CherryStyle::AddMarginY(12.0f);
      ImGui::PopStyleColor();

      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        std::string hidden;
        for (size_t k = 1; k < elements.size() - 3; ++k) {
          if (k != 1)
            hidden += separator;
          hidden += elements[k];
        }
        ImGui::SetTooltip("%s", hidden.c_str());
      }
    } else {
      auto buttonId = this->GetAppWindow()->m_IdName + el + std::to_string(i);
      if (CherryKit::ButtonText(CherryID(buttonId), el).GetData("isClicked") ==
          "true") {
        std::string return_path = FirstPathPartIsHome ? homePath : "";

        size_t clickedElementIndex = elementIndices[i];
        if (clickedElementIndex != static_cast<size_t>(-1)) {
          for (size_t j = 0; j <= clickedElementIndex; ++j)
            return_path += separator + elements[j];

          ChangeDirectory(return_path);
        }
      }

      {
        std::string test_path = FirstPathPartIsHome ? homePath : "";
        size_t clickedElementIndex = elementIndices[i];
        if (clickedElementIndex != static_cast<size_t>(-1)) {
          for (size_t j = 0; j <= clickedElementIndex; ++j)
            test_path += separator + elements[j];
        }

        for (auto &item : item_paths) {
          if (test_path == item.first) {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();

            ImVec2 p1(min.x, max.y + 1.0f);
            ImVec2 p2(max.x, max.y + 1.0f);

            ImU32 color = Cherry::HexToImU32(item.second);

            ImGui::GetWindowDrawList()->AddLine(p1, p2, color, 2.0f);
            break;
          }
        }
      }
    }

    CherryStyle::RemoveMarginY(8.0f);
    CherryStyle::RemoveMarginY(3.0f);
    if (i < displayElements.size() - 1) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1));
      ImGui::TextUnformatted(separator.c_str());
      ImGui::PopStyleColor();
    }
    CherryStyle::AddMarginY(3.0f);
  }

  CherryStyle::AddMarginY(2.0f);
}

void ContentBrowserAppWindow::RefreshCustomFolders() {
  VortexMaker::FetchCustomFolders();
  m_FavoriteFolders.clear();
  m_FolderColors.clear();
  for (auto custom_folder :
       VortexMaker::GetCurrentContext()->IO.contentbrowser_customfolders) {
    if (custom_folder->m_IsFav) {
      m_FavoriteFolders.push_back(custom_folder->path);
    }

    if (custom_folder->m_Color != "#fdaa00" ||
        custom_folder->m_Color != m_DefaultFolderColor) {
      if (custom_folder->m_Color.size() <= 7) {
        custom_folder->m_Color =
            custom_folder->m_Color +
            "ff"; // + ff to add a opaque bg if not provided
      }
      m_FolderColors.push_back({custom_folder->path, custom_folder->m_Color});
    }
  }
}

void ContentBrowserAppWindow::RefreshPools() {
  VortexMaker::FetchPools();
  m_Pools.clear();
  for (auto pool : VortexMaker::GetCurrentContext()->IO.contentbrowser_pools) {
    m_Pools.push_back(pool);
  }
}

void ContentBrowserAppWindow::RenderRightMenubar() {
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));

  {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");

    if (CherryKit::ButtonImageText(
            "Search",
            GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"))
            .GetDataAs<bool>("isClicked")) {
      m_SearchBar = !m_SearchBar;
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
      if (btn->Render("LogicContentManager")) {
        m_SearchBar = !m_SearchBar;

        if (!m_SearchBar) {
          memset(ProjectSearch, 0, sizeof ProjectSearch);
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
      if (CherryKit::ButtonImageTextDropdown(
              "Settings",
              GetPath("resources/imgs/icons/misc/icon_settings.png"))
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
        CherryKit::CheckboxText("Show SSET pannel", &m_ShowFilterPannel);
        CherryGUI::EndPopup();
      }

      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      CherryNextComponent.SetProperty("disable_callback", "true");
      if (CherryKit::ButtonImageTextDropdown(
              "View", GetPath("resources/imgs/icons/misc/icon_eye.png"))
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
      ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // TODO : Props
      ImVec4 graySeparatorColor =
          ImVec4(0.4f, 0.4f, 0.4f, 0.5f); // TODO : Props
      ImVec4 darkBackgroundColor =
          ImVec4(0.15f, 0.15f, 0.15f, 1.0f);                    // TODO : Props
      ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // TODO : Props

      CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
      CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
      CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

      if (m_WillShowFilterPannel != m_ShowFilterPannel) {
        m_ShowFilterPannel = m_WillShowFilterPannel;
      }

      if (CherryGUI::BeginPopup("ViewMenuPopup")) {
        CherryKit::SeparatorText("Pannels");
        CherryKit::CheckboxText("Show Filter pannel", &m_ShowFilterPannel);
        CherryKit::CheckboxText("Show Thumbnail pannel",
                                &m_ShowThumbnailVisualizer);
        CherryKit::SeparatorText("Content bar view");

        int default_index = 0;

        if (m_ShowMode == ContentShowMode::Thumbmails) {
          default_index = 0;
        } else if (m_ShowMode == ContentShowMode::List) {
          default_index = 1;
        } else if (m_ShowMode == ContentShowMode::Objects) {
          default_index = 2;
        }

        switch (
            CherryNextComponent.SetProperty("size_x", 150.0f);
            CherryKit::ComboImageText(
                "",
                {{"Thumbnails",
                  GetPath("resources/imgs/icons/misc/icon_thumbnails.png")},
                 {"List", GetPath("resources/imgs/icons/misc/icon_lines.png")},
                 {"Objects",
                  GetPath("resources/imgs/icons/misc/icon_objects.png")}},
                default_index)
                .GetPropertyAs<int>("selected")) {
        case 0: {
          m_ShowMode = ContentShowMode::Thumbmails;
          break;
        }
        case 1: {
          m_ShowMode = ContentShowMode::List;
          break;
        }
        case 2: {
          m_ShowMode = ContentShowMode::Objects;
          break;
        }
        default: {
          m_ShowMode = ContentShowMode::Thumbmails;
          break;
        }
        }

        CherryGUI::EndPopup();
      }

      if (m_WillShowFilterPannel != m_ShowFilterPannel) {
        m_WillShowFilterPannel = m_ShowFilterPannel;
      }

      CherryGUI::PopStyleVar();
      CherryGUI::PopStyleColor(2);
    }
  }

  CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 3.0f);

  CherryGUI::PopStyleColor();
  CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() - 1.5f);
}

void ContentBrowserAppWindow::CreateFile() {
  auto path = VortexMaker::CreateFile(m_CurrentDirectory.string());
  pathToRename = path;
}

void ContentBrowserAppWindow::CreateFolder() {
  auto path = VortexMaker::CreateFolder(m_CurrentDirectory.string());
  pathToRename = path;
}

void ContentBrowserAppWindow::SpawnImportWindow() {}

void ContentBrowserAppWindow::RenderMenubar() {

  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  CherryNextComponent.SetProperty("padding_x", "10.0f");

  if (CherryKit::ButtonImageText(
          GetLocale("loc.window.content.content_browser.add"),
          GetPath("resources/imgs/icons/misc/icon_add.png"))
          .GetDataAs<bool>("isClicked")) {
    SpawnAddWindow();
  }

  CherryNextComponent.SetProperty("color_border", "#00000000");
  CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
  CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  CherryKit::ButtonImageText(
      GetLocale("loc.window.content.content_browser.save"),
      GetPath("resources/imgs/icons/misc/icon_save.png"));

  CherryNextComponent.SetProperty("color_border", "#00000000");
  CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
  CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  CherryKit::ButtonImageText(
      GetLocale("loc.window.content.content_browser.import"),
      GetPath("resources/imgs/icons/misc/icon_import.png"));

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_Separator, Cherry::HexToRGBA("#444444AA"));
  ImGui::Separator();
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 12));

  static bool wasButtonX1Pressed = false;
  static bool wasButtonX2Pressed = false;

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
  if (m_BackHistory.empty()) {
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("size_x", "15.0f");
    CherryNextComponent.SetProperty("size_y", "15.0f");
    if (CherryKit::ButtonImage(
            GetPath("resources/imgs/icons/misc/icon_arrow_l_disabled.png"))
            .GetDataAs<bool>("isClicked")) {
      //
    }
  } else {

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("size_x", "15.0f");
    CherryNextComponent.SetProperty("size_y", "15.0f");
    if (CherryKit::ButtonImage(
            GetPath("resources/imgs/icons/misc/icon_arrow_l_enabled.png"))
            .GetDataAs<bool>("isClicked")) {
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
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("size_x", "15.0f");
    CherryNextComponent.SetProperty("size_y", "15.0f");
    if (CherryKit::ButtonImage(
            GetPath("resources/imgs/icons/misc/icon_arrow_r_disabled.png"))
            .GetDataAs<bool>("isClicked")) {
    }

  } else {
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("size_x", "15.0f");
    CherryNextComponent.SetProperty("size_y", "15.0f");
    if (CherryKit::ButtonImage(
            GetPath("resources/imgs/icons/misc/icon_arrow_r_enabled.png"))
            .GetDataAs<bool>("isClicked")) {
      GoForward();
    }

    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    bool isButtonX2Pressed = mouseState & SDL_BUTTON(SDL_BUTTON_X2);
    if (isButtonX2Pressed && !wasButtonX2Pressed) {
      GoForward();
    }
    wasButtonX2Pressed = isButtonX2Pressed;
  }

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.5f);
  ImGui::PopStyleVar();

  ImGui::Separator();
  ImGui::PopStyleColor(2);

  this->DrawPathBar(m_CurrentDirectory.string());
}

ContentBrowserAppWindow::ContentBrowserAppWindow(
    const std::string &name, const std::string &start_path) {
  m_AppWindow = std::make_shared<AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_collection.png"));
  std::shared_ptr<AppWindow> win = m_AppWindow;

  RefreshCustomFolders();
  RefreshPools();

  m_AppWindow->SetLeftMenubarCallback([this]() { RenderMenubar(); });
  m_AppWindow->m_Closable = true;
  m_AppWindow->SetCloseCallback(
      [this]() { Cherry::DeleteAppWindow(m_AppWindow); });

  m_AppWindow->SetRightMenubarCallback([this]() { RenderRightMenubar(); });
  m_AppWindow->SetLeftBottombarCallback([this]() {
    if (m_Selected.size() > 0) {
      std::string terminaison = m_Selected.size() >= 0 ? "s" : "";
      std::string label = std::to_string(m_Selected.size()) + " element" +
                          terminaison + " selected.";
      ImGui::Text(label.c_str());
    }

    if (m_CopySelection.size() > 0) {
      std::string terminaison = m_CopySelection.size() > 0 ? "s" : "";
      std::string label = std::to_string(m_CopySelection.size()) + " element" +
                          terminaison + " copied.";
      ImGui::Text(label.c_str());
    }
  });

  m_BaseDirectory = start_path;
  m_CurrentDirectory = m_BaseDirectory;

  ContentBrowserChild sidebar("RenderSideBar", [this]() { RenderSideBar(); });
  sidebar.Enable();
  sidebar.m_DefaultSize = c_FilterBarWidth;
  sidebar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");

  AddChild(sidebar);

  ContentBrowserChild filterbar("RenderFiltersBar",
                                [this]() { RenderFiltersBar(); });
  filterbar.Disable();
  filterbar.m_DefaultSize = 250.0f;
  filterbar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");
  AddChild(filterbar);

  ContentBrowserChild contentbar("RenderContentBar",
                                 [this]() { RenderContentBar(); });
  contentbar.Enable();
  contentbar.m_DefaultSize = 0.0;
  AddChild(ContentBrowserChild(contentbar));

  ContentBrowserChild detailsbar("RenderDetailsBar",
                                 [this]() { RenderDetailsBar(); });
  detailsbar.Disable();
  detailsbar.m_DefaultSize = 250.0f;
  AddChild(ContentBrowserChild(detailsbar));
}

void ContentBrowserAppWindow::AddChild(const ContentBrowserChild &child) {
  m_Childs.push_back(child);
}

static const std::unordered_map<FileTypes, const char *> kFileIconMap = {
    {FileTypes::File_PICTURE,
     "resources/imgs/icons/files/icon_picture_file.png"},
    {FileTypes::File_GIT, "resources/imgs/icons/files/icon_git_file.png"},
    {FileTypes::File_H, "resources/imgs/icons/files/icon_c_h_file.png"},
    {FileTypes::File_C, "resources/imgs/icons/files/icon_c_file.png"},
    {FileTypes::File_HPP, "resources/imgs/icons/files/icon_cpp_h_file.png"},
    {FileTypes::File_CPP, "resources/imgs/icons/files/icon_cpp_file.png"},
    {FileTypes::File_INI, "resources/imgs/icons/files/icon_ini_file.png"}};

std::string ContentBrowserAppWindow::GetFileTypeStr(FileTypes type) {
  switch (type) {
  // Very low level
  case FileTypes::File_ASM:
    return "file_asm";
  case FileTypes::File_BIN:
    return "file_bin";

  // Programming languages
  case FileTypes::File_C:
    return "file_c";
  case FileTypes::File_H:
    return "file_h";
  case FileTypes::File_CPP:
    return "file_cpp";
  case FileTypes::File_HPP:
    return "file_hpp";
  case FileTypes::File_INL:
    return "file_inl";
  case FileTypes::File_RUST:
    return "file_rust";
  case FileTypes::File_ZIG:
    return "file_zig";
  case FileTypes::File_GO:
    return "file_go";
  case FileTypes::File_JAVA:
    return "file_java";
  case FileTypes::File_JAVASCRIPT:
    return "file_javascript";
  case FileTypes::File_COBOL:
    return "file_cobol";
  case FileTypes::File_PASCAL:
    return "file_pascal";
  case FileTypes::File_CARBON:
    return "file_carbon";
  case FileTypes::File_PYTHON:
    return "file_python";
  case FileTypes::File_RUBY:
    return "file_ruby";
  case FileTypes::File_PHP:
    return "file_php";
  case FileTypes::File_SWIFT:
    return "file_swift";
  case FileTypes::File_CSHARP:
    return "file_csharp";
  case FileTypes::File_VB:
    return "file_vb";
  case FileTypes::File_DART:
    return "file_dart";
  case FileTypes::File_KOTLIN:
    return "file_kotlin";
  case FileTypes::File_SCALA:
    return "file_scala";
  case FileTypes::File_PERL:
    return "file_perl";
  case FileTypes::File_SHELL:
    return "file_shell";
  case FileTypes::File_BATCH:
    return "file_batch";
  case FileTypes::File_LUA:
    return "file_lua";
  case FileTypes::File_R:
    return "file_r";
  case FileTypes::File_MATLAB:
    return "file_matlab";
  case FileTypes::File_JULIA:
    return "file_julia";
  case FileTypes::File_SQL:
    return "file_sql";
  case FileTypes::File_GROOVY:
    return "file_groovy";
  case FileTypes::File_FORTRAN:
    return "file_fortran";
  case FileTypes::File_ERLANG:
    return "file_erlang";
  case FileTypes::File_ELIXIR:
    return "file_elixir";
  case FileTypes::File_OCAML:
    return "file_ocaml";

  // Web and Markup
  case FileTypes::File_HTML:
    return "file_html";
  case FileTypes::File_CSS:
    return "file_css";
  case FileTypes::File_XML:
    return "file_xml";

  // Config
  case FileTypes::File_CFG:
    return "file_cfg";
  case FileTypes::File_JSON:
    return "file_json";
  case FileTypes::File_YAML:
    return "file_yaml";
  case FileTypes::File_INI:
    return "file_ini";

  // Git
  case FileTypes::File_GIT:
    return "file_git";

  // Documents
  case FileTypes::File_TXT:
    return "file_txt";
  case FileTypes::File_MD:
    return "file_md";
  case FileTypes::File_DOC:
    return "file_doc";
  case FileTypes::File_PDF:
    return "file_pdf";

  // Pictures
  case FileTypes::File_PICTURE:
    return "file_picture";

  // Videos
  case FileTypes::File_VIDEO:
    return "file_video";

  // Audio
  case FileTypes::File_AUDIO:
    return "file_audio";

  // Archives
  case FileTypes::File_ARCHIVE:
    return "file_archive";

  // Miscellaneous
  case FileTypes::File_LOG:
    return "file_log";
  case FileTypes::File_BACKUP:
    return "file_backup";
  case FileTypes::File_TEMP:
    return "file_temp";
  case FileTypes::File_DATA:
    return "file_data";

  // Other
  case FileTypes::File_UNKNOWN:
    return "file_unknown";
  }

  return "file_unknown"; // fallback
}

void ContentBrowserAppWindow::FolderButton(
    const char *id, ImVec2 size, const std::string &path,
    const std::string &color = "#c2a24c") {
  auto cmp = CherryKit::WidgetFolder(color, size.x, size.y - 2);
  if (cmp.GetData("isClicked") == "true") {
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
        ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
      m_Selected.push_back(path);
    } else {
      m_Selected.clear();
      m_Selected.push_back(path);
    }
  }
  if (cmp.GetData("isDoubleClicked") == "true") {
    ChangeDirectory(path);
  }
}

void ContentBrowserAppWindow::FolderIcon(ImVec2 size, ImU32 color) {
  CherryKit::WidgetFolder("#c2a24c", size.x, size.y - 2);
}

void ContentBrowserAppWindow::GoBack() {
  if (!m_BackHistory.empty()) {
    m_ForwardHistory.push(m_CurrentDirectory.string());
    m_CurrentDirectory = m_BackHistory.top();
    m_BackHistory.pop();
  }
}

void ContentBrowserAppWindow::GoForward() {
  if (!m_ForwardHistory.empty()) {
    m_BackHistory.push(m_CurrentDirectory.string());
    m_CurrentDirectory = m_ForwardHistory.top();
    m_ForwardHistory.pop();
  }
}

void ContentBrowserAppWindow::ChangeDirectory(
    const std::filesystem::path &newDirectory) {
  if (newDirectory != m_CurrentDirectory.string()) {
    if (!m_CurrentDirectory.empty()) {
      m_BackHistory.push(m_CurrentDirectory.string());

      while (!m_ForwardHistory.empty()) {
        m_ForwardHistory.pop();
      }
    }
    m_CurrentDirectory = newDirectory;
  }
}

bool ContentBrowserAppWindow::HorizontalItemCard(
    const std::string &name, const std::string &path,
    const std::string &description, const std::string &size, bool selected,
    const std::string &logo, ImU32 bgColor = IM_COL32(100, 100, 100, 255),
    ImU32 borderColor = IM_COL32(150, 150, 150, 255),
    ImU32 lineColor = IM_COL32(255, 255, 0, 255), float maxTextWidth = 300.0f,
    float borderRadius = 5.0f, bool isFolder = false,
    const std::shared_ptr<ItemIdentifierInterface> &item_ident = nullptr) {
  bool pressed = false;

  float logoSize = 40.0f;
  float padding = 10.0f;
  float separatorHeight = 2.0f;
  float versionBoxWidth = 10.0f;
  float versionBoxHeight = 20.0f;

  float oldFontScale = ImGui::GetFont()->Scale;
  ImFont *oldFont = ImGui::GetFont();

  if (selected) {
    bgColor = IM_COL32(80, 80, 240, 255);
    borderColor = IM_COL32(150, 150, 255, 255);
  }

  ImVec2 cursorPos = ImGui::GetCursorScreenPos();
  float contentWidth = ImGui::GetContentRegionAvail().x;
  ImVec2 cardSize(contentWidth, logoSize + padding * 2 + 10);

  std::string button_id = "HorizontalCard_" + path;
  if (ImGui::InvisibleButton(button_id.c_str(), cardSize)) {
    pressed = true;
  }

  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
      if (isFolder) {
        ChangeDirectory(path);
      } else {
        // default file action
      }
    }
  }

  bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
  bool alt = CherryApp.IsKeyPressed(CherryKey::ALT);
  bool shift = CherryApp.IsKeyPressed(CherryKey::SHIFT);

  bool shortcutRename = ctrl && ImGui::IsKeyPressed(ImGuiKey_R);
  bool shortcutCut = ctrl && ImGui::IsKeyPressed(ImGuiKey_X);
  bool shortcutCutAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::X);
  bool shortcutCopy = ctrl && CherryApp.IsKeyPressed(CherryKey::C);
  bool shortcutCopyAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::C);
  bool shortcutDelete = CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);
  bool shortcutDeleteMulti =
      alt && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);

  if (shortcutRename && !pathToRename.empty()) {
    pathToRename = path;
  }

  if (shortcutCut && !m_Selected.empty()) {
    if (m_CutPathsCallback) {
      m_CopySelection.clear();
      m_CutSelection.clear();
      m_CutPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CutSelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutCutAdd && !m_Selected.empty()) {
    if (m_CutPathsCallback) {
      m_CopySelection.clear();
      m_CutPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CutSelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutCopy && !m_Selected.empty()) {
    if (m_CopyPathsCallback) {
      m_CopySelection.clear();
      m_CutSelection.clear();
      m_CopyPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CopySelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutCopyAdd && !m_Selected.empty()) {
    if (m_CopyPathsCallback) {
      m_CutSelection.clear();
      m_CopyPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CopySelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutDelete && m_Selected.size() == 1) {
    if (m_DeletePathCallback)
      m_DeletePathCallback(m_Selected[0]);
    m_Selected.clear();
  }

  if (shortcutDeleteMulti && m_Selected.size() > 1) {
    if (m_DeletePathCallback) {
      for (const auto &path : m_Selected)
        m_DeletePathCallback(path);
    }
    m_Selected.clear();
  }
  if (ImGui::BeginPopupContextItem(("context_" + path).c_str())) {
    ImGui::GetFont()->Scale = 0.9;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

    item_context_menu_opened = true;

    FileTypes fileType = detect_file(path);

    if (item_handles.empty()) {
      if (item_ident) {
        item_handles = VortexMaker::GetAllItemHandlersFor(item_ident->m_Name);
      } else {
        item_handles =
            VortexMaker::GetAllItemHandlersFor(GetFileTypeStr(fileType));
      }
    }

    if (!item_handles.empty()) {
      CherryKit::SeparatorText("Actions");
    }

    for (auto ih : item_handles) {
      if (ImGui::MenuItem(ih->title.c_str(), ih->description.c_str(),
                          Cherry::GetTexture(ih->logo), false)) {
        ih->handler(path);
      }
    }

    CherryKit::SeparatorText("Main");

    ImGui::GetFont()->Scale = oldFontScale;
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

    if (item_ident) {
      if (ImGui::MenuItem(
              "Open item", "",
              Cherry::GetTexture(Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_foldersearch.png")),
              NULL)) {
        ChangeDirectory(path);
        item_paths.push_back({path, item_ident->m_LineColor});
        ImGui::CloseCurrentPopup();
      }
    }

    if (ImGui::MenuItem("Copy", "Ctrl + C",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_copy.png")),
                        NULL)) {
      if (m_CopyPathsCallback) {
        m_CopyPathsCallback(m_Selected, false);
      }

      m_Selected.clear();
      ImGui::CloseCurrentPopup();
    }

    if (m_Selected.size() > 0) {
      std::string label =
          "Copy in addition (" + std::to_string(m_Selected.size()) + " copies)";
      if (ImGui::MenuItem(label.c_str(), "Ctrl + C",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_copy.png")),
                          NULL)) {
        if (m_CopyPathsCallback) {
          m_CopyPathsCallback(m_Selected, true);
        }

        m_Selected.clear();
        ImGui::CloseCurrentPopup();
      }
    }
    if (ImGui::MenuItem("Cut", "Ctrl + X",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_cut.png")),
                        NULL)) {
      ChangeDirectory(path);
      ImGui::CloseCurrentPopup();
    }

    CherryKit::SeparatorText("Customize");

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

    if (ImGui::MenuItem("Change color", "Ctrl + X",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_palette.png")),
                        NULL)) {
      //
    }
    if (ImGui::MenuItem("Mark as favorite", "Ctrl + X",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_heart.png")),
                        NULL)) {
      //
    }

    ImGui::EndPopup();
  }

  ImDrawList *drawList = ImGui::GetWindowDrawList();

  drawList->AddRectFilled(
      cursorPos, ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
      bgColor, borderRadius);
  drawList->AddRect(cursorPos,
                    ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
                    borderColor, borderRadius, 0, 1.0f);

  ImVec2 logoPos(cursorPos.x + padding, cursorPos.y + padding);
  ImVec2 logoEnd(logoPos.x + logoSize, logoPos.y + logoSize);
  static ImTextureID logotexture =
      Application::GetCurrentRenderedWindow()->get_texture(logo);
  drawList->AddImage(logotexture, logoPos, logoEnd);

  ImVec2 textPos(logoEnd.x + padding, cursorPos.y + padding);
  float textWidth = contentWidth - logoSize - padding * 3 - versionBoxWidth;

  ImGui::SetCursorScreenPos(textPos);
  ImGui::GetFont()->Scale = 0.9;
  ImGui::PushFont(ImGui::GetFont());

  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
  ImGui::PushItemWidth(textWidth);

  static std::unordered_map<std::string, char[256]> renameBuffers;
  if (pathToRename == path) {
    ImGui::PushID(path.c_str());
    auto &buffer = renameBuffers[path];
    if (buffer[0] == '\0') {
      std::strncpy(buffer, name.c_str(), sizeof(buffer));
    }
    if (ImGui::InputText("##rename_input", buffer, sizeof(buffer),
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
      VortexMaker::SubmitRename(pathToRename, buffer);
      renameBuffers.erase(pathToRename);
      pathToRename.clear();
    }
    ImGui::PopID();
  } else {
    ImGui::TextWrapped("%s", name.c_str());
  }

  ImGui::PopItemWidth();
  ImGui::PopStyleColor();
  ImGui::GetFont()->Scale = oldFontScale;
  ImGui::PopFont();

  ImVec2 descPos(textPos.x, textPos.y + 20);
  ImGui::SetCursorScreenPos(descPos);
  ImGui::GetFont()->Scale = 0.7;
  ImGui::PushFont(ImGui::GetFont());

  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
  ImGui::PushItemWidth(textWidth);
  ImGui::TextWrapped("%s", description.c_str());
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();
  ImGui::GetFont()->Scale = oldFontScale;
  ImGui::PopFont();

  float lineOffset = versionBoxWidth + padding * 1.5f;
  ImVec2 lineStart(cursorPos.x + cardSize.x - lineOffset,
                   cursorPos.y + padding);
  ImVec2 lineEnd(cursorPos.x + cardSize.x - lineOffset,
                 cursorPos.y + cardSize.y - padding);
  drawList->AddLine(lineStart, lineEnd, lineColor, 3.0f);

  ImVec2 versionPos(cursorPos.x + cardSize.x - versionBoxWidth - padding,
                    cursorPos.y + cardSize.y - versionBoxHeight - padding);
  drawList->AddRectFilled(
      versionPos,
      ImVec2(versionPos.x + versionBoxWidth, versionPos.y + versionBoxHeight),
      IM_COL32(0, 0, 0, 255), borderRadius);

  ImGui::SetCursorScreenPos(
      ImVec2(cursorPos.x, cursorPos.y + cardSize.y + padding));
  return pressed;
}

bool ContentBrowserAppWindow::ItemCard(
    const std::string &name, const std::string &path,
    const std::string &description, const std::string &size, bool selected,
    const std::string &logo, ImU32 bgColor = IM_COL32(100, 100, 100, 255),
    ImU32 borderColor = IM_COL32(150, 150, 150, 255),
    ImU32 lineColor = IM_COL32(255, 255, 0, 255), float maxTextWidth = 100.0f,
    float borderRadius = 5.0f,
    const std::shared_ptr<ItemIdentifierInterface> &item_ident = nullptr) {
  bool pressed = false;

  float logoSize = 60.0f;
  float extraHeight = 80.0f;
  float padding = 10.0f;
  float separatorHeight = 2.0f;
  float textOffsetY = 5.0f;
  float versionBoxWidth = 10.0f;
  float versionBoxHeight = 20.0f;
  float thumbnailIconOffsetY = 30.0f;

  float oldfontsize = ImGui::GetFont()->Scale;

  if (selected) {
    bgColor = IM_COL32(80, 80, 240, 255);
    borderColor = IM_COL32(150, 150, 255, 255);
  }

  ImVec2 squareSize(logoSize, logoSize);

  const char *originalText = name.c_str();
  std::string truncatedText = name;

  if (ImGui::CalcTextSize(originalText).x > maxTextWidth) {
    truncatedText = name.substr(0, 20);
    if (ImGui::CalcTextSize(truncatedText.c_str()).x > maxTextWidth) {
      truncatedText = name.substr(0, 10) + "\n" + name.substr(10, 10);
    }
  } else {
    truncatedText = name + "\n";
  }

  ImVec2 fixedSize(maxTextWidth + padding * 2,
                   logoSize + extraHeight + padding * 2);

  ImVec2 cursorPos = ImGui::GetCursorScreenPos();

  std::string button_id = name + "squareButtonWithText" + name;
  if (ImGui::InvisibleButton(button_id.c_str(), fixedSize)) {
    pressed = true;
  }

  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
  ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
  ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

  ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

  ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

  static bool open_deletion_modal = false;

  static bool delete_single_file = false;
  static std::string delete_single_file_path = "";

  if (open_deletion_modal) {
    ImGui::SetNextWindowSize(ImVec2(300, 200));

    static ImGuiTableFlags window_flags = ImGuiWindowFlags_NoResize;
    if (ImGui::BeginPopupModal("Delete file(s)", 0, window_flags)) {
      static char path_input_all[512];

      if (delete_single_file) {
        ImGui::TextWrapped("WARNING, one file");
      } else {
        ImGui::TextWrapped("WARNING, if you click on the Delete button, the "
                           "project will be erase forever.");
      }

      ImGui::SetItemDefaultFocus();

      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        open_deletion_modal = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 0.9f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            ImVec4(1.0f, 0.2f, 0.2f, 1.8f));
      if (ImGui::Button("Delete", ImVec2(120, 0))) {
        if (delete_single_file) {
          if (m_DeletePathCallback) {
            m_DeletePathCallback(delete_single_file_path);
          }
          delete_single_file_path = "";
          delete_single_file = false;
        } else {
          for (auto item : m_Selected) {
            m_DeletePathCallback(item);
          }
        }

        open_deletion_modal = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor(3);
      ImGui::EndPopup();
    }
  }
  if (open_deletion_modal)
    ImGui::OpenPopup("Delete file(s)");

  if (ImGui::IsItemHovered() &&
      ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    m_Selected.push_back(path);
  }

  bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
  bool alt = CherryApp.IsKeyPressed(CherryKey::ALT);
  bool shift = CherryApp.IsKeyPressed(CherryKey::SHIFT);

  bool shortcutRename = ctrl && ImGui::IsKeyPressed(ImGuiKey_R);
  bool shortcutCut = ctrl && ImGui::IsKeyPressed(ImGuiKey_X);
  bool shortcutCutAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::X);
  bool shortcutCopy = ctrl && CherryApp.IsKeyPressed(CherryKey::C);
  bool shortcutCopyAdd = ctrl && alt && CherryApp.IsKeyPressed(CherryKey::C);
  bool shortcutDelete = CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);
  bool shortcutDeleteMulti =
      alt && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);

  if (shortcutRename && !pathToRename.empty()) {
    pathToRename = path;
  }

  if (shortcutCut && !m_Selected.empty()) {
    if (m_CutPathsCallback) {
      m_CopySelection.clear();
      m_CutSelection.clear();
      m_CutPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CutSelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutCutAdd && !m_Selected.empty()) {
    if (m_CutPathsCallback) {
      m_CopySelection.clear();
      m_CutPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CutSelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutCopy && !m_Selected.empty()) {
    if (m_CopyPathsCallback) {
      m_CopySelection.clear();
      m_CutSelection.clear();
      m_CopyPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CopySelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutCopyAdd && !m_Selected.empty()) {
    if (m_CopyPathsCallback) {
      m_CutSelection.clear();
      m_CopyPathsCallback(m_Selected, true);
      for (const auto &path : m_Selected)
        m_CopySelection.push_back(path);
    }
    m_Selected.clear();
  }

  if (shortcutDelete && m_Selected.size() == 1) {
    if (m_DeletePathCallback)
      m_DeletePathCallback(m_Selected[0]);
    m_Selected.clear();
  }

  if (shortcutDeleteMulti && m_Selected.size() > 1) {
    if (m_DeletePathCallback) {
      for (const auto &path : m_Selected)
        m_DeletePathCallback(path);
    }
    m_Selected.clear();
  }
  item_context_menu_opened = false;
  if (ImGui::BeginPopupContextItem("ThumbmailsItemContextPopup")) {
    ImGui::GetFont()->Scale = 0.9;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

    item_context_menu_opened = true;

    FileTypes fileType = detect_file(path);

    if (item_handles.empty()) {
      if (item_ident) {
        item_handles = VortexMaker::GetAllItemHandlersFor(item_ident->m_Name);
      } else {
        item_handles =
            VortexMaker::GetAllItemHandlersFor(GetFileTypeStr(fileType));
      }
    }

    if (!item_handles.empty()) {
      CherryKit::SeparatorText("Actions");
    }

    for (auto ih : item_handles) {
      if (ImGui::MenuItem(ih->title.c_str(), ih->description.c_str(),
                          Cherry::GetTexture(ih->logo), false)) {
        ih->handler(path);
      }
    }

    CherryKit::SeparatorText("Main");

    ImGui::GetFont()->Scale = oldfontsize;
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

    if (item_ident) {
      if (ImGui::MenuItem(
              "Open item", "",
              Cherry::GetTexture(Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_foldersearch.png")),
              NULL)) {
        ChangeDirectory(path);
        item_paths.push_back({path, item_ident->m_LineColor});
        ImGui::CloseCurrentPopup();
      }
    }

    if (ImGui::MenuItem("Copy", "Ctrl + C",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_copy.png")),
                        NULL)) {
      if (m_CopyPathsCallback) {
        m_CopyPathsCallback(m_Selected, false);
      }

      m_Selected.clear();
      ImGui::CloseCurrentPopup();
    }

    if (m_Selected.size() > 0) {
      std::string label =
          "Copy in addition (" + std::to_string(m_Selected.size()) + " copies)";
      if (ImGui::MenuItem(label.c_str(), "Ctrl + C",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_copy.png")),
                          NULL)) {
        if (m_CopyPathsCallback) {
          m_CopyPathsCallback(m_Selected, true);
        }

        m_Selected.clear();
        ImGui::CloseCurrentPopup();
      }
    }
    if (ImGui::MenuItem("Cut", "Ctrl + X",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_cut.png")),
                        NULL)) {
      ChangeDirectory(path);
      ImGui::CloseCurrentPopup();
    }

    CherryKit::SeparatorText("Customize");

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

    if (ImGui::MenuItem("Change color", "Ctrl + X",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_palette.png")),
                        NULL)) {
      //
    }
    if (ImGui::MenuItem("Mark as favorite", "Ctrl + X",
                        Cherry::GetTexture(Cherry::GetPath(
                            "resources/imgs/icons/misc/icon_heart.png")),
                        NULL)) {
      //
    }

    ImGui::EndPopup();
  }

  if (!item_context_menu_opened) {
    item_handles.clear();
  }
  /*if (ImGui::BeginPopupContextItem("ContextPopup")) {
    CherryKit::SeparatorText("Main");

    if (ImGui::MenuItem("Rename", "Ctrl + R")) {
      pathToRename = path;
    }

    {
      std::string label =
          "Cut (" + std::to_string(m_Selected.size()) + ") selected";

      if (ImGui::MenuItem(label.c_str(), "Ctrl + X")) {
        if (m_CutPathsCallback) {
          m_CopySelection.clear();
          m_CutSelection.clear();
          m_CutPathsCallback(m_Selected, true);
          for (auto &path : m_Selected) {
            m_CutSelection.push_back(path);
          }
        }

        m_Selected.clear();

        ImGui::CloseCurrentPopup();
      }
    }

    if (m_CutSelection.size() > 0) {
      std::string label = "Cut in addition (" +
                          std::to_string(m_CutSelection.size()) + " copies)";
      if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
        if (m_CutPathsCallback) {
          m_CopySelection.clear();
          m_CutPathsCallback(m_Selected, true);

          for (auto &path : m_Selected) {
            m_CutSelection.push_back(path);
          }
        }
        m_Selected.clear();
        ImGui::CloseCurrentPopup();
      }
    }

    {
      std::string label =
          "Copy (" + std::to_string(m_Selected.size()) + ") selected";

      if (ImGui::MenuItem(label.c_str(), "Ctrl + C")) {
        if (m_CopyPathsCallback) {
          m_CopySelection.clear();
          m_CutSelection.clear();
          m_CopyPathsCallback(m_Selected, true);
          for (auto &path : m_Selected) {
            m_CopySelection.push_back(path);
          }
        }

        m_Selected.clear();

        ImGui::CloseCurrentPopup();
      }
    }

    if (m_CopySelection.size() > 0) {
      std::string label = "Copy in addition (" +
                          std::to_string(m_CopySelection.size()) + " copies)";
      if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
        if (m_CopyPathsCallback) {
          m_CutSelection.clear();
          m_CopyPathsCallback(m_Selected, true);

          for (auto &path : m_Selected) {
            m_CopySelection.push_back(path);
          }
        }
        m_Selected.clear();
        ImGui::CloseCurrentPopup();
      }
    }

    if (ImGui::MenuItem("Delete", "Suppr")) {
      if (m_DeletePathCallback) {
        m_DeletePathCallback(path);
      }
      m_Selected.clear();
      ImGui::CloseCurrentPopup();
    }

    if (m_Selected.size() > 1) {
      std::string label =
          "Delete (" + std::to_string(m_Selected.size()) + " selected)";
      if (ImGui::MenuItem(label.c_str(), "Alt + Suppr")) {
        if (m_DeletePathCallback) {
          for (auto &path : m_Selected) {
            m_DeletePathCallback(path);
          }
        }
        m_Selected.clear();

        ImGui::CloseCurrentPopup();
      }
    }

    CherryKit::SeparatorText("Customizations");

    ImGui::EndPopup();
  }
*/

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(2);

  ImDrawList *drawList = ImGui::GetWindowDrawList();

  drawList->AddRectFilled(
      cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
      bgColor, borderRadius);
  drawList->AddRectFilled(
      cursorPos,
      ImVec2(cursorPos.x + fixedSize.x,
             cursorPos.y + thumbnailIconOffsetY + squareSize.y),
      IM_COL32(26, 26, 26, 255), borderRadius, ImDrawFlags_RoundCornersTop);
  drawList->AddRect(
      cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
      borderColor, borderRadius, 0, 1.0f);

  ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2,
                          cursorPos.y + padding);

  ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y +
                                                     thumbnailIconOffsetY - 20 +
                                                     textOffsetY);
  ImGui::SetCursorScreenPos(sizePos);

  static ImTextureID logotexture =
      Application::GetCurrentRenderedWindow()->get_texture(logo);
  drawList->AddImage(
      logotexture, logoPos,
      ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

  ImGui::GetFont()->Scale = 0.7;
  ImGui::PushFont(ImGui::GetFont());
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
  ImGui::PushItemWidth(maxTextWidth);
  ImGui::TextWrapped(size.c_str());
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();

  ImGui::GetFont()->Scale = oldfontsize;
  ImGui::PopFont();

  ImVec2 lineStart =
      ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                              separatorHeight);
  ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x,
                          cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                              separatorHeight);
  drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

  ImGui::GetFont()->Scale = 0.9;
  ImGui::PushFont(ImGui::GetFont());

  ImVec2 textPos =
      ImVec2(cursorPos.x + padding,
             cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
  ImGui::SetCursorScreenPos(textPos);
  ImGui::PushItemWidth(maxTextWidth);
  ImU32 textColor = IM_COL32(255, 255, 255, 255);
  ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
  ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
  static std::unordered_map<std::string, char[256]> renameBuffers;

  if (pathToRename == path) {
    ImGui::SetItemAllowOverlap();
    ImGui::PushID(path.c_str());

    auto &buffer = renameBuffers[path];
    if (buffer[0] == '\0') {
      std::string filename = path.substr(path.find_last_of("/\\") + 1);
      std::strncpy(buffer, filename.c_str(), sizeof(buffer));
      buffer[sizeof(buffer) - 1] = '\0';
      ImGui::SetKeyboardFocusHere();
    }

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    bool renamed =
        ImGui::InputText("##rename_input", buffer, sizeof(buffer), flags);

    bool renameConfirmed = false;
    bool renameCancelled = false;

    if (renamed) {
      renameConfirmed = true;
    }

    if (!ImGui::IsItemActive() && ImGui::IsItemDeactivatedAfterEdit()) {
      renameConfirmed = true;
    }

    if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      renameCancelled = true;
    }

    if (renameConfirmed) {
      VortexMaker::SubmitRename(pathToRename, buffer);
      renameBuffers.erase(pathToRename);
      pathToRename.clear();
    }

    if (renameCancelled) {
      renameBuffers.erase(pathToRename);
      pathToRename.clear();
    }

    ImGui::PopID();
  } else {
    DrawHighlightedText(drawList, textPos, truncatedText.c_str(),
                        ProjectSearch.c_str(), highlightColor, textColor,
                        highlightTextColor);
  }

  ImGui::PopItemWidth();

  ImGui::GetFont()->Scale = oldfontsize;
  ImGui::PopFont();

  ImVec2 descriptionPos = ImVec2(cursorPos.x + padding,
                                 cursorPos.y + squareSize.y +
                                     thumbnailIconOffsetY + 35 + textOffsetY);
  ImGui::SetCursorScreenPos(descriptionPos);

  ImGui::GetFont()->Scale = 0.7;
  ImGui::PushFont(ImGui::GetFont());
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
  ImGui::PushItemWidth(maxTextWidth);
  ImGui::TextWrapped(description.c_str());
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();

  ImGui::GetFont()->Scale = oldfontsize;
  ImGui::PopFont();

  ImVec2 smallRectPos =
      ImVec2(cursorPos.x + fixedSize.x - versionBoxWidth - padding,
             cursorPos.y + fixedSize.y - versionBoxHeight - padding);
  drawList->AddRectFilled(smallRectPos,
                          ImVec2(smallRectPos.x + versionBoxWidth,
                                 smallRectPos.y + versionBoxHeight),
                          IM_COL32(0, 0, 0, 255), borderRadius);

  float windowVisibleX2 =
      ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
  if (cursorPos.x + fixedSize.x < windowVisibleX2)
    ImGui::SameLine();

  ImGui::SetCursorScreenPos(
      ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

  return pressed;
}

void ContentBrowserAppWindow::DrawFolderIcon(ImVec2 pos, ImVec2 size,
                                             ImU32 color) {
  ImDrawList *drawList = ImGui::GetWindowDrawList();

  float folderFlapHeight = size.y * 0.2f;
  float flapSlopeWidth = size.x * 0.15f;
  float borderRadius = size.y * 0.1f;

  ImVec2 flapTopLeft = pos;
  ImVec2 flapBottomRight =
      ImVec2(pos.x + size.x * 0.6f, pos.y + folderFlapHeight);
  ImVec2 flapSlopeEnd =
      ImVec2(flapBottomRight.x + flapSlopeWidth, flapBottomRight.y);

  ImVec2 bodyTopLeft = ImVec2(pos.x, pos.y + folderFlapHeight);
  ImVec2 bodyBottomRight = ImVec2(pos.x + size.x, pos.y + size.y);

  drawList->AddRectFilled(bodyTopLeft, bodyBottomRight, color);

  ImVec2 centerRectTopLeft =
      ImVec2(pos.x + size.x * 0.2f, pos.y + 0.2f + size.y * 0.2f + 6.8f);
  ImVec2 centerRectBottomRight =
      ImVec2(pos.x + size.x * 0.8f, pos.y + size.y * 0.8f - 2.8f);
  drawList->AddRectFilled(centerRectTopLeft, centerRectBottomRight,
                          IM_COL32_WHITE, 0.0f, 0);

  ImVec2 secondRectTopLeft =
      ImVec2(pos.x + size.x * 0.2f, pos.y + 0.2f + size.y * 0.2f + 11.8f);
  ImVec2 secondRectBottomRight = ImVec2(pos.x + size.x, pos.y + size.y);
  drawList->AddRectFilled(secondRectTopLeft, secondRectBottomRight, color, 0.0f,
                          0);

  drawList->AddRectFilled(flapTopLeft, flapBottomRight, color, borderRadius,
                          ImDrawFlags_RoundCornersTopLeft);
  drawList->AddTriangleFilled(flapBottomRight, flapSlopeEnd,
                              ImVec2(flapBottomRight.x - 3, flapTopLeft.y),
                              color);
}

void ContentBrowserAppWindow::MyFolderButton(const char *id, ImVec2 size,
                                             ImU32 color,
                                             const std::string &path) {
  ImVec2 pos = ImGui::GetCursorScreenPos();

  ImGui::InvisibleButton(id, size);
  if (ImGui::IsItemHovered() &&
      ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
    ChangeDirectory(path);
  }

  float thumbmailWidth = thumbnailSize;
  float folderWidth = size.x;
  float offsetX = (thumbmailWidth - folderWidth) / 2.0f;

  ImVec2 iconPos = ImVec2(pos.x + offsetX + 7.5f, pos.y);

  DrawFolderIcon(iconPos, size, color);

  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }
}

void WidgetFolderT(const std::string &colorHex, float width, float height,
                   ImVec2 pos) {
  ImDrawList *drawList = ImGui::GetWindowDrawList();
  ImU32 color = HexToImU32(colorHex);

  float flapHeight = height * 0.2f;
  float flapWidth = width * 0.4f;
  float borderRadius = height * 0.05f;

  ImVec4 colVec = ImGui::ColorConvertU32ToFloat4(color);
  ImU32 flapColor = ImGui::ColorConvertFloat4ToU32(
      ImVec4(colVec.x * 0.8f, colVec.y * 0.8f, colVec.z * 0.8f, colVec.w));
  ImU32 shadowColor = ImGui::ColorConvertFloat4ToU32(ImVec4(
      colVec.x * 0.7f, colVec.y * 0.7f, colVec.z * 0.7f, colVec.w * 0.2f));

  ImVec2 flapTopLeft = ImVec2(pos.x + width * 0.001f, pos.y - 1.0f);
  ImVec2 flapBottomRight =
      ImVec2(flapTopLeft.x + flapWidth, pos.y + flapHeight);

  ImVec2 flapBarTopLeft = ImVec2(pos.x, pos.y + flapHeight - height * 0.14f);
  ImVec2 flapBarBottomRight =
      ImVec2(pos.x + width, pos.y + flapHeight + height * 0.07f);

  ImVec2 bodyTopLeft = ImVec2(pos.x, pos.y + flapHeight);
  ImVec2 bodyBottomRight = ImVec2(pos.x + width, pos.y + height);

  drawList->AddRectFilled(flapTopLeft, flapBottomRight, flapColor, borderRadius,
                          ImDrawFlags_RoundCornersTopLeft);
  drawList->AddRectFilled(flapBarTopLeft, flapBarBottomRight, flapColor,
                          borderRadius);
  drawList->AddRectFilled(bodyTopLeft, bodyBottomRight, color, borderRadius);

  float lineOffset = height * 0.05f;
  for (int i = 1; i <= 3; ++i) {
    float yPos = bodyBottomRight.y - (lineOffset * i);
    drawList->AddLine(ImVec2(bodyTopLeft.x + width * 0.01f, yPos),
                      ImVec2(bodyBottomRight.x - width * 0.01f, yPos),
                      shadowColor, 2.0f);
  }
}

void ContentBrowserAppWindow::DrawHierarchy(std::filesystem::path path,
                                            bool isDir,
                                            const std::string &label = "") {
  if (!isDir)
    return;

  std::string tree_label = (label.empty() ? path.filename().string() : label);
  std::string unique_id = path.string() + "##treenode";

  ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap |
                                     ImGuiTreeNodeFlags_SpanAvailWidth |
                                     ImGuiTreeNodeFlags_FramePadding;

  ImVec2 nodePos = ImGui::GetCursorScreenPos();

  bool opened = ImGui::TreeNodeEx(unique_id.c_str(), treeNodeFlags, "%s",
                                  tree_label.c_str());

  ImVec2 itemSize = ImGui::GetItemRectSize();
  ImVec2 iconSize = ImVec2(12, 12);
  ImVec2 iconPos =
      ImVec2(nodePos.x + 2.0f, nodePos.y + (itemSize.y - iconSize.y) * 0.5f);

  WidgetFolderT(GetContentBrowserFolderColor(path.string()), iconSize.x,
                iconSize.y, iconPos);

  if (ImGui::IsItemClicked()) {
    ChangeDirectory(path);
  }

  if (opened) {
    try {
      std::vector<std::filesystem::directory_entry> entries;
      for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
          entries.push_back(entry);
        }
      }

      std::sort(entries.begin(), entries.end(),
                [](const auto &a, const auto &b) {
                  return a.path().filename() < b.path().filename();
                });

      for (const auto &entry : entries) {
        try {
          DrawHierarchy(entry.path(), entry.is_directory());
        } catch (const std::exception &e) {
          std::cerr << "Error displaying " << entry.path() << ": " << e.what()
                    << std::endl;
        }
      }
    } catch (const std::exception &e) {
      std::cerr << "Error reading directory " << path << ": " << e.what()
                << std::endl;
    }

    ImGui::TreePop();
  }
}

void ContentBrowserAppWindow::RenderSideBar() {
  const float header_width = c_SideBarWidth - 46.0f;

  CherryStyle::RemoveMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("size_y", 4.0f);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Favorite",
      Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),
      [this]() {
        for (auto custom_dir : m_FavoriteFolders) {
          DrawHierarchy(custom_dir, true);
        }
      });

  CherryStyle::RemoveMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("size_y", 4.0f);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Main", Application::CookPath("resources/imgs/icons/misc/icon_home.png"),
      [this]() { DrawHierarchy(m_BaseDirectory, true, "Main"); });

  CherryStyle::RemoveMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("size_y", 4.0f);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Pools & Collections",
      Application::CookPath("resources/imgs/icons/misc/icon_collection.png"),
      [this]() {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 2.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));
        if (!pool_add_mode) {
          if (ImGui::ImageButtonWithText(
                  Cherry::GetTexture(Cherry::GetPath(
                      "resources/imgs/icons/misc/icon_add.png")),
                  "Add pool", ImVec2(0, 0), ImVec2(0, 0), ImVec2(1, 1), -1,
                  ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
            pool_add_mode = true;
          }
        } else {
          ImGui::Text("Please enter a path");
          ImGui::SetNextItemWidth(-FLT_MIN);
          ImGui::Text("Name ");
          ImGui::SameLine();
          ImGui::InputText("###AddPoolName", pool_add_name,
                           sizeof(pool_add_name));

          ImGui::Text("Path ");
          ImGui::SameLine();
          ImGui::InputText("###AddPoolPath", pool_add_path,
                           sizeof(pool_add_path));
          if (ImGui::ImageButtonWithText(
                  Cherry::GetTexture(Cherry::GetPath(
                      "resources/imgs/icons/misc/icon_add.png")),
                  "Add", ImVec2(0, 0), ImVec2(0, 0), ImVec2(1, 1), -1,
                  ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
            VortexMaker::PublishPool(pool_add_path, pool_add_name);
            RefreshPools();
            pool_add_mode = false;
          }
          ImGui::SameLine();
          if (ImGui::ImageButtonWithText(
                  Cherry::GetTexture(Cherry::GetPath(
                      "resources/imgs/icons/misc/icon_return.png")),
                  "Cancel", ImVec2(0, 0), ImVec2(0, 0), ImVec2(1, 1), -1,
                  ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
            pool_add_mode = false;
          }
        }

        for (auto pool : m_Pools) {
          DrawHierarchy(pool.first, true, pool.second);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
      });
}

void ContentBrowserAppWindow::RenderFiltersBar() {
  const float header_width = c_FilterBarWidth - 46.0f;

  CherryStyle::RemoveMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("size_y", 4.0f);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Basic filters",
      Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),
      [this]() {
        CherryKit::CheckboxText("All files", &ShowFiles);
        CherryKit::CheckboxText("All folders", &ShowFolders);
        CherryKit::CheckboxText("All items", &ShowItems);
        CherryKit::CheckboxText("Hidden contents", &ShowHidden);
      });

  CherryKit::SeparatorText("Advanced filters");

  CherryStyle::RemoveMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("size_y", 4.0f);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");

  CherryKit::HeaderImageText(
      "Extensions filters",
      Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),
      [this]() {
        std::vector<std::pair<std::string, std::vector<std::string>>>
            categories = {{"Configuration", {".cfg", ".ini", ".config"}},
                          {"Text based formats", {".txt", ".json"}}};

        for (auto &category : categories) {
          CherryKit::SeparatorText(category.first);

          for (auto &ext : category.second) {
            bool state = HasExtension(ext);
            CherryKit::CheckboxText(ext.c_str(), &state);
            ToggleExtension(ext, &state);
          }
        }
      });
}

void ContentBrowserAppWindow::PathChanged() {
  // Fetch Modules item hamdlers
}

void ContentBrowserAppWindow::RenderContentBar() {
  ImGuiStyle &style = ImGui::GetStyle();
  ImVec4 originalChildBgColor = style.Colors[ImGuiCol_ChildBg];
  ImVec4 originalBorderColor = style.Colors[ImGuiCol_Border];
  ImVec4 originalBorderShadowColor = style.Colors[ImGuiCol_BorderShadow];
  float oldfontsize = ImGui::GetFont()->Scale;

  style.Colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
  style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

  float cellSize = thumbnailSize + padding;

  float panelWidth = ImGui::GetContentRegionAvail().x;
  int columnCount = (int)(panelWidth / cellSize);
  if (columnCount < 1)
    columnCount = 1;
  if (m_SearchBar) {

    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText(
            "Filters", GetPath("resources/imgs/icons/misc/icon_filter.png"))
            .GetDataAs<bool>("isClicked")) {
      m_WillShowFilterPannel = !m_WillShowFilterPannel;
    }

    CherryGUI::SameLine();

    CherryNextComponent.SetProperty("size_x", "240");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("description", "Search content...");
    CherryNextComponent.SetProperty(
        "description_logo",
        GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
    CherryNextComponent.SetProperty("description_logo_place", "r");
    CherryKit::InputString("", &ProjectSearch);

    CherryKit::Separator();
  }

  ImGui::Spacing();

  std::vector<std::filesystem::directory_entry> directories;
  std::vector<std::filesystem::directory_entry> files;

  recognized_modules_items.clear();

  for (auto &directoryEntry :
       std::filesystem::directory_iterator(m_CurrentDirectory)) {
    bool isItem = false;
    for (auto em : VortexMaker::GetCurrentContext()->IO.em) {
      for (auto item : em->GetContentBrowserItemIdentifiers()) {
        std::string path = directoryEntry.path().string();
        if (item->f_Detect(path)) {
          recognized_modules_items.push_back({item, path});
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

  std::sort(
      directories.begin(), directories.end(), [](const auto &a, const auto &b) {
        return a.path().filename().string() < b.path().filename().string();
      });

  static bool pasteKeyDown = false;

  bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
  bool vKey = CherryApp.IsKeyPressed(CherryKey::V);
  bool shortcutPaste = ctrl && vKey;

  if (shortcutPaste && !pasteKeyDown) {
    pasteKeyDown = true;

    if (m_PastePathsCallback) {
      m_PastePathsCallback({m_CurrentDirectory.string()});
    }
  }

  if (!ctrl || !vKey) {
    pasteKeyDown = false;
  }

  if (m_ShowMode == ContentShowMode::Thumbmails) {
    ImGui::Columns(columnCount, 0, false);

    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

    ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (ImGui::IsWindowHovered() &&
        ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
      m_Selected.clear();
      ImGui::OpenPopup("EmptySpacePopup");
    }

    if (ImGui::IsWindowHovered() &&
        ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
      m_Selected.clear();
    }

    if (ImGui::BeginPopup("EmptySpacePopup")) {
      ImGui::GetFont()->Scale = 0.9;
      ImGui::PushFont(ImGui::GetFont());

      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

      CherryKit::SeparatorText("Add");
      if (ImGui::MenuItem("Add", "Add a component",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_add.png")),
                          NULL)) {
        SpawnAddWindow();
      }
      if (ImGui::MenuItem("Import", "Import a component",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_import.png")),
                          NULL)) {
      }

      CherryKit::SeparatorText("Create");
      if (ImGui::MenuItem("Create new folder", "Create a folder here",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_add_folder.png")),
                          NULL)) {
        CreateFolder();
      }
      if (ImGui::MenuItem("Create new file", "Create empty file here",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_add_file.png")),
                          NULL)) {
        CreateFile();
      }

      CherryKit::SeparatorText("Actions");

      if (ImGui::MenuItem("Paste selection", "Paste selection here",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_down.png")),
                          NULL)) {
        if (m_PastePathsCallback) {
          m_PastePathsCallback({m_CurrentDirectory.string()});
        }
      }

      if (ImGui::MenuItem("Select All", "Select eveything on this directory",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_stack.png")),
                          NULL)) {
      }

      ImGui::GetFont()->Scale = oldfontsize;
      ImGui::PopFont();

      ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    if (ShowFolders) {
      for (auto &directoryEntry : directories) {
        const auto &path = directoryEntry.path();
        std::string filenameString = path.filename().string();

        bool selected = false;

        if (std::find(m_Selected.begin(), m_Selected.end(), path) !=
            m_Selected.end()) {
          selected = true;
        }

        if (areStringsSimilar(filenameString, ProjectSearch, threshold) ||
            isOnlySpacesOrEmpty(ProjectSearch.c_str())) {
          ImGui::PushID(filenameString.c_str());

          bool rename = false;
          if (path == pathToRename) {
            rename = true;
          }

          if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Header,
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.5f));
          }

          if (selected) {
            ImGui::PopStyleColor(3);
          }
          float reducedThumbnailSize = thumbnailSize * 0.9f;
          float availableWidth = ImGui::GetContentRegionAvail().x;
          float imageOffsetX = (availableWidth - reducedThumbnailSize) * 0.5f;

          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
          ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

          // Old folder logo
          /*ImGui::SetCursorPosX(ImGui::GetCursorPosX() + imageOffsetX);
          addTexture(Folder_Logo, Folder_Logo);
          ImGui::ImageButton(getTexture(Folder_Logo), {reducedThumbnailSize,
          reducedThumbnailSize}, {-1, 0}, {0, 1});

          if (ImGui::IsItemHovered() &&
          ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
          {
              ChangeDirectory(path);.string()
          }*/
          CherryStyle::RemoveMarginX(5.0f);
          ImVec2 folderSize(reducedThumbnailSize, reducedThumbnailSize);
          float folderPosX = (availableWidth - folderSize.x) * 0.5f;
          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + folderPosX);

          // GetContentBrowserFolderColor(path.string();
          std::string folder_color = "#c2a24c";

          if (selected) {
            folder_color = "#2424CC";
          }

          if (current_editing_folder.first == path.string()) {
            FolderButton("folder_icon", folderSize, path.string(),
                         Cherry::ImU32ToHex(current_editing_folder.second));
          } else {
            FolderButton("folder_icon", folderSize, path.string(),
                         GetContentBrowserFolderColor(path.string()));
          }

          float oldsize = ImGui::GetFont()->Scale;

          ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
          ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
          ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
          ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

          ImGui::PopStyleVar(2);
          ImGui::PopStyleColor(3);

          ImVec2 cursorPos = ImGui::GetCursorPos();
          float textWidth = ImGui::CalcTextSize(filenameString.c_str()).x;
          float textX = folderPosX + (folderSize.x - textWidth) * 0.5f;
          ImGui::SetCursorPos(
              ImVec2(textX + ImGui::GetCursorPosX(), cursorPos.y));
          // ImGui::TextWrapped(filenameString.c_str());

          ImDrawList *drawList = ImGui::GetWindowDrawList();

          ImU32 textColor = IM_COL32(255, 255, 255, 255);
          ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
          ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
          static std::unordered_map<std::string, char[256]> renameBuffers;

          if (pathToRename == path) {
            ImGui::SetItemAllowOverlap();
            ImGui::PushID(path.c_str());

            auto &buffer = renameBuffers[path.string()];
            if (buffer[0] == '\0') {
              std::string filename =
                  path.string().substr(path.string().find_last_of("/\\") + 1);
              std::strncpy(buffer, filename.c_str(), sizeof(buffer));
              buffer[sizeof(buffer) - 1] = '\0';
              ImGui::SetKeyboardFocusHere();
            }

            ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
            bool renamed = ImGui::InputText("##rename_input", buffer,
                                            sizeof(buffer), flags);

            bool renameConfirmed = false;
            bool renameCancelled = false;

            if (renamed) {
              renameConfirmed = true;
            }

            if (!ImGui::IsItemActive() && ImGui::IsItemDeactivatedAfterEdit()) {
              renameConfirmed = true;
            }

            if (ImGui::IsItemFocused() &&
                ImGui::IsKeyPressed(ImGuiKey_Escape)) {
              renameCancelled = true;
            }

            if (renameConfirmed) {
              VortexMaker::SubmitRename(pathToRename, buffer);
              renameBuffers.erase(pathToRename);
              pathToRename.clear();
            }

            if (renameCancelled) {
              renameBuffers.erase(pathToRename);
              pathToRename.clear();
            }

            ImGui::PopID();
          } else {

            DrawHighlightedText(drawList, ImGui::GetCursorScreenPos(),
                                filenameString.c_str(), ProjectSearch.c_str(),
                                highlightColor, textColor, highlightTextColor);
          }

          if (ImGui::BeginPopupContextItem("ContextPopup")) {

            CherryKit::SeparatorText("Main");

            if (ImGui::MenuItem("Rename", "Ctrl + R")) {
              pathToRename = path.string();
            }

            {
              std::string label =
                  "Cut (" + std::to_string(m_Selected.size()) + ") selected";

              if (ImGui::MenuItem(label.c_str(), "Ctrl + X")) {
                if (m_CutPathsCallback) {
                  m_CutPathsCallback(m_Selected, true);
                  for (auto &path : m_Selected) {
                    m_CutSelection.push_back(path);
                  }
                }

                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            if (m_CutSelection.size() > 0) {
              std::string label = "Cut in addition (" +
                                  std::to_string(m_CutSelection.size()) +
                                  " copies)";
              if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                if (m_CutPathsCallback) {
                  m_CutPathsCallback(m_Selected, true);

                  for (auto &path : m_Selected) {
                    m_CutSelection.push_back(path);
                  }
                }
                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }

            {
              std::string label =
                  "Copy (" + std::to_string(m_Selected.size()) + ") selected";

              if (ImGui::MenuItem(label.c_str(), "Ctrl + C")) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);
                  for (auto &path : m_Selected) {
                    m_CopySelection.push_back(path);
                  }
                }

                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            if (m_CopySelection.size() > 0) {
              std::string label = "Copy in addition (" +
                                  std::to_string(m_CopySelection.size()) +
                                  " copies)";
              if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);

                  for (auto &path : m_Selected) {
                    m_CopySelection.push_back(path);
                  }
                }
                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }

            if (ImGui::MenuItem("Delete", "Suppr")) {
              if (m_DeletePathCallback) {
                m_DeletePathCallback(path.string());
              }
              m_Selected.clear();
              ImGui::CloseCurrentPopup();
            }

            if (m_Selected.size() > 1) {
              std::string label =
                  "Delete (" + std::to_string(m_Selected.size()) + " selected)";
              if (ImGui::MenuItem(label.c_str(), "Alt + Suppr")) {
                if (m_DeletePathCallback) {
                  for (auto &path : m_Selected) {
                    m_DeletePathCallback(path);
                  }
                }
                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            CherryKit::SeparatorText("Customizations");

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

            static bool EditingColor = false;
            static bool ColorChanged = false;

            current_editing_folder_is_favorite =
                IsPathFavorite(directoryEntry.path().string());

            if (ImGui::BeginMenu("Change color")) {
              if (!EditingColor) {
                current_editing_folder = {directoryEntry.path().string(),
                                          Cherry::HexToImU32(folder_color)};

                current_editing_folder = {
                    directoryEntry.path().string(),
                    HexToImU32(GetContentBrowserFolderColor(path.string()))};

                current_editing_folder_is_favorite =
                    IsPathFavorite(directoryEntry.path().string());
              }

              EditingColor = true;

              static bool alpha_preview = true;
              static bool alpha_half_preview = false;
              static bool drag_and_drop = true;
              static bool options_menu = true;
              static bool hdr = false;

              ColorPicker3U32("MyColor", &current_editing_folder.second);

              if (current_editing_folder.second !=
                  Cherry::HexToImU32(folder_color)) {
                ColorChanged = true;
              }

              ImGui::EndMenu();
            } else {
              EditingColor = false;
              current_editing_folder = {"", ImU32()};
            }

            if (ImGui::MenuItem("Mark as favorite", "",
                                current_editing_folder_is_favorite)) {
              current_editing_folder = {directoryEntry.path().string(),
                                        current_editing_folder.second};

              current_editing_folder_is_favorite =
                  !current_editing_folder_is_favorite;
              SetColoredFolder(current_editing_folder.first,
                               ImU32ToHex(current_editing_folder.second));

              VortexMaker::PublishContentBrowserCustomFolder(
                  current_editing_folder.first,
                  Cherry::ImU32ToHex(current_editing_folder.second),
                  current_editing_folder_is_favorite);

              if (current_editing_folder_is_favorite) {
                m_FavoriteFolders.push_back(current_editing_folder.first);
              } else {
                auto it = std::find(m_FavoriteFolders.begin(),
                                    m_FavoriteFolders.end(),
                                    current_editing_folder.first);
                if (it != m_FavoriteFolders.end()) {
                  m_FavoriteFolders.erase(it);
                }
              }
            }

            ImGui::EndPopup();
          }

          ImGui::PopID();
          ImGui::NextColumn();
        }
      }
    }

    if (ShowItems) {
      for (auto &itemEntry : recognized_modules_items) {
        const auto &path = itemEntry.second;
        std::filesystem::path fsPath(path);
        std::string filenameString = fsPath.filename().string();

        bool selected = false;

        if (!ShowHidden && !filenameString.empty() &&
            filenameString[0] == '.') {
          continue;
        }

        if (!ExtentionsFilters.empty()) {
          bool match = false;
          for (const auto &ext : ExtentionsFilters) {
            if (filenameString.size() >= ext.size() &&
                filenameString.compare(filenameString.size() - ext.size(),
                                       ext.size(), ext) == 0) {
              match = true;
              break;
            }
          }

          if (!match) {
            continue;
          }
        }

        if (std::find(m_Selected.begin(), m_Selected.end(), path) !=
            m_Selected.end()) {
          selected = true;
        }

        if (areStringsSimilar(filenameString, ProjectSearch, threshold) ||
            isOnlySpacesOrEmpty(ProjectSearch.c_str())) {
          std::uintmax_t folderSize = getDirectorySize(path);
          std::string folderSizeString = formatFileSize(folderSize);
          ImGui::PushID(filenameString.c_str());

          if (ItemCard(filenameString, path, itemEntry.first->m_Description,
                       folderSizeString, selected,
                       Application::CookPath(
                           "resources/imgs/icons/files/icon_picture_file.png"),
                       IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                       Cherry::HexToImU32(itemEntry.first->m_LineColor), 100.0f,
                       5.0f, itemEntry.first)) {
            if (ImGui::IsMouseDoubleClicked(0)) {
              // itemEntry.first->f_Execute(path);
              // VXERROR("te", "tyr");
            }

            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
              m_Selected.push_back(path);
            } else {
              m_Selected.clear();
              m_Selected.push_back(path);
            }
          }

          ImGui::PopID();
          ImGui::NextColumn();
        }
      }
    }

    if (ShowFiles) {
      for (auto &fileEntry : files) {
        const auto &path = fileEntry.path();
        std::string filenameString = path.filename().string();

        bool selected = false;

        if (!ShowHidden && !filenameString.empty() &&
            filenameString[0] == '.') {
          continue;
        }

        if (!ExtentionsFilters.empty()) {
          bool match = false;
          for (const auto &ext : ExtentionsFilters) {
            if (filenameString.size() >= ext.size() &&
                filenameString.compare(filenameString.size() - ext.size(),
                                       ext.size(), ext) == 0) {
              match = true;
              break;
            }
          }

          if (!match) {
            continue;
          }
        }

        if (std::find(m_Selected.begin(), m_Selected.end(), path) !=
            m_Selected.end()) {
          selected = true;
        }

        if (areStringsSimilar(filenameString, ProjectSearch, threshold) ||
            isOnlySpacesOrEmpty(ProjectSearch.c_str())) {
          std::error_code ec;
          size_t fileSize = 0;

          if (fileEntry.is_regular_file(ec)) {
            fileSize += std::filesystem::file_size(fileEntry.path(), ec);
          }

          std::string fileSizeString = formatFileSize(fileSize);
          ImGui::PushID(filenameString.c_str());

          FileTypes fileType = detect_file(path.string());

          switch (fileType) {
          case FileTypes::File_PICTURE: {
            if (ItemCard(
                    filenameString, path.string(), "Picture file",
                    fileSizeString, selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_picture_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(255, 100, 150, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_GIT: {
            if (ItemCard(filenameString, path.string(), "Git File",
                         fileSizeString, selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_git_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_H: {
            if (ItemCard(filenameString, path.string(), "C Header File",
                         fileSizeString, selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_c_h_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(220, 100, 220, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_C: {
            if (ItemCard(filenameString, path.string(), "C Source File",
                         fileSizeString, selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_c_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_HPP: {
            if (ItemCard(filenameString, path.string(), "C++ Header File",
                         fileSizeString, selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_cpp_h_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_CPP: {
            if (ItemCard(filenameString, path.string(), "C++ Source File",
                         fileSizeString, selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_cpp_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_INI: {
            if (ItemCard(filenameString, path.string(), "Init File",
                         fileSizeString, selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_ini_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(150, 150, 150, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          default: {
            if (ItemCard(filenameString, path.string(), "File", fileSizeString,
                         selected,
                         Application::CookPath(
                             "resources/imgs/icons/files/icon_unknow_file.png"),
                         IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                         IM_COL32(150, 150, 150, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
          }
          float oldsize = ImGui::GetFont()->Scale;

          ImGui::PopID();
          ImGui::NextColumn();
        }
      }
    }
    ImGui::Columns(1);
  }

  if (m_ShowMode == ContentShowMode::List) {
    static ImGuiTableFlags flags =
        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    if (ImGui::BeginTable("ContentBrowserTable", 5, flags)) {
      ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
      ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
      ImGui::TableSetupColumn("Last Updated", ImGuiTableColumnFlags_WidthFixed,
                              120.0f);
      ImGui::TableHeadersRow();
      auto drawRow = [&](const std::string &filename, const fs::path &path,
                         bool isFolder, size_t size = 0, std::time_t cftime = 0,
                         ImTextureID tex = nullptr,
                         const std::shared_ptr<ItemIdentifierInterface>
                             &item_ident = nullptr) {
        std::string filenameString = path.filename().string();
        float rowHeight = 20.0f;
        ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);

        ImGui::TableSetColumnIndex(0);

        bool selected = std::find(m_Selected.begin(), m_Selected.end(),
                                  path.c_str()) != m_Selected.end();

        if (ImGui::Selectable(("##row_" + path.string()).c_str(), selected,
                              ImGuiSelectableFlags_SpanAllColumns |
                                  ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(0, rowHeight))) {
          if (ImGui::GetIO().KeyCtrl) {
            if (!selected)
              m_Selected.push_back(path.string());
          } else {
            m_Selected.clear();
            m_Selected.push_back(path.string());
          }

          if (ImGui::IsMouseClicked(0)) {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
              m_Selected.push_back(path);
            } else {
              m_Selected.clear();
              m_Selected.push_back(path);
            }
          }

          if (ImGui::IsMouseDoubleClicked(0)) {
            if (!isFolder) {
              // Default action
            } else {
              ChangeDirectory(path);
            }
            //
          }
        }

        item_context_menu_opened = false;
        if (!isFolder) {

          bool ctrl = CherryApp.IsKeyPressed(CherryKey::CTRL);
          bool alt = CherryApp.IsKeyPressed(CherryKey::ALT);
          bool shift = CherryApp.IsKeyPressed(CherryKey::SHIFT);

          bool shortcutRename = ctrl && ImGui::IsKeyPressed(ImGuiKey_R);
          bool shortcutCut = ctrl && ImGui::IsKeyPressed(ImGuiKey_X);
          bool shortcutCutAdd =
              ctrl && alt && CherryApp.IsKeyPressed(CherryKey::X);
          bool shortcutCopy = ctrl && CherryApp.IsKeyPressed(CherryKey::C);
          bool shortcutCopyAdd =
              ctrl && alt && CherryApp.IsKeyPressed(CherryKey::C);
          bool shortcutDelete = CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);
          bool shortcutDeleteMulti =
              alt && CherryApp.IsKeyPressed(CherryKey::KEY_DELETE);

          if (shortcutRename && !pathToRename.empty()) {
            pathToRename = path;
          }

          if (shortcutCut && !m_Selected.empty()) {
            if (m_CutPathsCallback) {
              m_CopySelection.clear();
              m_CutSelection.clear();
              m_CutPathsCallback(m_Selected, true);
              for (const auto &path : m_Selected)
                m_CutSelection.push_back(path);
            }
            m_Selected.clear();
          }

          if (shortcutCutAdd && !m_Selected.empty()) {
            if (m_CutPathsCallback) {
              m_CopySelection.clear();
              m_CutPathsCallback(m_Selected, true);
              for (const auto &path : m_Selected)
                m_CutSelection.push_back(path);
            }
            m_Selected.clear();
          }

          if (shortcutCopy && !m_Selected.empty()) {
            if (m_CopyPathsCallback) {
              m_CopySelection.clear();
              m_CutSelection.clear();
              m_CopyPathsCallback(m_Selected, true);
              for (const auto &path : m_Selected)
                m_CopySelection.push_back(path);
            }
            m_Selected.clear();
          }

          if (shortcutCopyAdd && !m_Selected.empty()) {
            if (m_CopyPathsCallback) {
              m_CutSelection.clear();
              m_CopyPathsCallback(m_Selected, true);
              for (const auto &path : m_Selected)
                m_CopySelection.push_back(path);
            }
            m_Selected.clear();
          }

          if (shortcutDelete && m_Selected.size() == 1) {
            if (m_DeletePathCallback)
              m_DeletePathCallback(m_Selected[0]);
            m_Selected.clear();
          }

          if (shortcutDeleteMulti && m_Selected.size() > 1) {
            if (m_DeletePathCallback) {
              for (const auto &path : m_Selected)
                m_DeletePathCallback(path);
            }
            m_Selected.clear();
          }
          if (ImGui::BeginPopupContextItem(
                  ("context_" + path.string()).c_str())) {
            ImGui::GetFont()->Scale = 0.9;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);

            item_context_menu_opened = true;

            FileTypes fileType = detect_file(path);

            if (item_handles.empty()) {
              if (item_ident) {
                item_handles =
                    VortexMaker::GetAllItemHandlersFor(item_ident->m_Name);
              } else {
                item_handles = VortexMaker::GetAllItemHandlersFor(
                    GetFileTypeStr(fileType));
              }
            }

            if (!item_handles.empty()) {
              CherryKit::SeparatorText("Actions");
            }

            for (auto ih : item_handles) {
              if (ImGui::MenuItem(ih->title.c_str(), ih->description.c_str(),
                                  Cherry::GetTexture(ih->logo), false)) {
                ih->handler(path);
              }
            }

            CherryKit::SeparatorText("Main");

            ImGui::GetFont()->Scale = oldfontsize;
            ImGui::PopFont();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

            if (item_ident) {
              if (ImGui::MenuItem(
                      "Open item", "",
                      Cherry::GetTexture(Cherry::GetPath(
                          "resources/imgs/icons/misc/icon_foldersearch.png")),
                      NULL)) {
                ChangeDirectory(path);
                item_paths.push_back({path, item_ident->m_LineColor});
                ImGui::CloseCurrentPopup();
              }
            }

            if (ImGui::MenuItem("Copy", "Ctrl + C",
                                Cherry::GetTexture(Cherry::GetPath(
                                    "resources/imgs/icons/misc/icon_copy.png")),
                                NULL)) {
              if (m_CopyPathsCallback) {
                m_CopyPathsCallback(m_Selected, false);
              }

              m_Selected.clear();
              ImGui::CloseCurrentPopup();
            }

            if (m_Selected.size() > 0) {
              std::string label = "Copy in addition (" +
                                  std::to_string(m_Selected.size()) +
                                  " copies)";
              if (ImGui::MenuItem(
                      label.c_str(), "Ctrl + C",
                      Cherry::GetTexture(Cherry::GetPath(
                          "resources/imgs/icons/misc/icon_copy.png")),
                      NULL)) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);
                }

                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }
            if (ImGui::MenuItem("Cut", "Ctrl + X",
                                Cherry::GetTexture(Cherry::GetPath(
                                    "resources/imgs/icons/misc/icon_cut.png")),
                                NULL)) {
              ChangeDirectory(path);
              ImGui::CloseCurrentPopup();
            }

            CherryKit::SeparatorText("Customize");

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

            if (ImGui::MenuItem(
                    "Change color", "Ctrl + X",
                    Cherry::GetTexture(Cherry::GetPath(
                        "resources/imgs/icons/misc/icon_palette.png")),
                    NULL)) {
              //
            }
            if (ImGui::MenuItem(
                    "Mark as favorite", "Ctrl + X",
                    Cherry::GetTexture(Cherry::GetPath(
                        "resources/imgs/icons/misc/icon_heart.png")),
                    NULL)) {
              //
            }

            ImGui::EndPopup();
          }

        } else {
          if (ImGui::BeginPopupContextItem(
                  ("context_" + path.string()).c_str())) {

            CherryKit::SeparatorText("Main");

            if (ImGui::MenuItem("Rename", "Ctrl + R")) {
              pathToRename = path.string();
            }

            {
              std::string label =
                  "Cut (" + std::to_string(m_Selected.size()) + ") selected";

              if (ImGui::MenuItem(label.c_str(), "Ctrl + X")) {
                if (m_CutPathsCallback) {
                  m_CutPathsCallback(m_Selected, true);
                  for (auto &path : m_Selected) {
                    m_CutSelection.push_back(path);
                  }
                }

                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            if (m_CutSelection.size() > 0) {
              std::string label = "Cut in addition (" +
                                  std::to_string(m_CutSelection.size()) +
                                  " copies)";
              if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                if (m_CutPathsCallback) {
                  m_CutPathsCallback(m_Selected, true);

                  for (auto &path : m_Selected) {
                    m_CutSelection.push_back(path);
                  }
                }
                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }

            {
              std::string label =
                  "Copy (" + std::to_string(m_Selected.size()) + ") selected";

              if (ImGui::MenuItem(label.c_str(), "Ctrl + C")) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);
                  for (auto &path : m_Selected) {
                    m_CopySelection.push_back(path);
                  }
                }

                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            if (m_CopySelection.size() > 0) {
              std::string label = "Copy in addition (" +
                                  std::to_string(m_CopySelection.size()) +
                                  " copies)";
              if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);

                  for (auto &path : m_Selected) {
                    m_CopySelection.push_back(path);
                  }
                }
                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }

            if (ImGui::MenuItem("Delete", "Suppr")) {
              if (m_DeletePathCallback) {
                m_DeletePathCallback(path.string());
              }
              m_Selected.clear();
              ImGui::CloseCurrentPopup();
            }

            if (m_Selected.size() > 1) {
              std::string label =
                  "Delete (" + std::to_string(m_Selected.size()) + " selected)";
              if (ImGui::MenuItem(label.c_str(), "Alt + Suppr")) {
                if (m_DeletePathCallback) {
                  for (auto &path : m_Selected) {
                    m_DeletePathCallback(path);
                  }
                }
                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            CherryKit::SeparatorText("Customizations");

            ImGui::EndPopup();
          }
        }

        if (!item_context_menu_opened) {
          item_handles.clear();
        }

        ImGui::SameLine();
        FileTypes fileType = detect_file(path.string());

        if (!isFolder) {
          auto it = kFileIconMap.find(fileType);
          if (it != kFileIconMap.end()) {
            tex = Cherry::GetTexture(Application::CookPath(it->second));
          } else {
            tex = Cherry::GetTexture(Application::CookPath(
                "resources/imgs/icons/files/icon_unknow_file.png"));
          }

          if (tex) {
            ImGui::Image(tex, ImVec2(24, 24));
            ImGui::SameLine();
          }
        } else {
          if (current_editing_folder.first == path.string()) {
            FolderButton("folder_icon", ImVec2(16, 16), path.string(),
                         GetContentBrowserFolderColor(path.string()));
          } else {
            FolderButton("folder_icon", ImVec2(16, 16), path.string(),
                         GetContentBrowserFolderColor(path.string()));
          }
          ImGui::SameLine();
        }
        // ImGui::TextUnformatted(filename.c_str());

        ImU32 textColor = IM_COL32(255, 255, 255, 255);
        ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
        ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
        static std::unordered_map<std::string, char[256]> renameBuffers;

        if (pathToRename == path.string()) {
          ImGui::SetItemAllowOverlap();
          ImGui::PushID(path.string().c_str());

          auto &buffer = renameBuffers[path.string()];
          if (buffer[0] == '\0') {
            std::string filename =
                path.string().substr(path.string().find_last_of("/\\") + 1);
            std::strncpy(buffer, filename.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
            ImGui::SetKeyboardFocusHere();
          }

          ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
          bool renamed =
              ImGui::InputText("##rename_input", buffer, sizeof(buffer), flags);

          bool renameConfirmed = false;
          bool renameCancelled = false;

          if (renamed) {
            renameConfirmed = true;
          }

          if (!ImGui::IsItemActive() && ImGui::IsItemDeactivatedAfterEdit()) {
            renameConfirmed = true;
          }

          if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            renameCancelled = true;
          }

          if (renameConfirmed) {
            VortexMaker::SubmitRename(pathToRename, buffer);
            renameBuffers.erase(pathToRename);
            pathToRename.clear();
          }

          if (renameCancelled) {
            renameBuffers.erase(pathToRename);
            pathToRename.clear();
          }

          ImGui::PopID();
        } else {
          if (filename == "...") {
            filenameString = "...";
          }

          ImDrawList *drawList = ImGui::GetWindowDrawList();
          DrawHighlightedText(drawList, ImGui::GetCursorScreenPos(),
                              filenameString.c_str(), ProjectSearch.c_str(),
                              highlightColor, textColor, highlightTextColor);
        }

        ImGui::TableSetColumnIndex(1);
        if (item_ident) {
          ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
                             item_ident->m_Description.c_str());
        } else {
          ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f),
                             isFolder ? "Folder" : "File");
        }

        ImGui::TableSetColumnIndex(2);
        ImGui::TextUnformatted(isFolder ? "---" : formatFileSize(size).c_str());

        ImGui::TableSetColumnIndex(3);
        if (cftime != 0) {
          char buffer[64];
          std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M",
                        std::localtime(&cftime));
          ImGui::TextUnformatted(buffer);
        } else {
          ImGui::TextUnformatted("---");
        }
      };

      fs::path projectRoot = VortexMaker::GetCurrentContext()->projectDataPath;

      if (!m_CurrentDirectory.empty()) {
        auto rel = fs::relative(m_CurrentDirectory, projectRoot);

        bool insideProject = !rel.empty() && rel.native().rfind("..", 0) != 0;

        if (insideProject && m_CurrentDirectory != projectRoot) {
          drawRow("...", m_CurrentDirectory.parent_path(), true);
        }
      }

      if (ShowFolders) {
        for (auto &directoryEntry : directories) {
          const auto &path = directoryEntry.path();
          std::string filename = path.filename().string();

          if (!areStringsSimilar(filename, ProjectSearch, threshold) &&
              !isOnlySpacesOrEmpty(ProjectSearch.c_str()))
            continue;

          ImTextureID tex = nullptr;
          FileTypes type = detect_file(path.string());
          switch (type) {
          case FileTypes::File_PICTURE:
            tex = Application::GetCurrentRenderedWindow()->get_texture(
                Application::CookPath(
                    "resources/imgs/icons/files/icon_picture_file.png"));
            break;
          default:
            tex = Application::GetCurrentRenderedWindow()->get_texture(
                Application::CookPath(
                    "resources/imgs/icons/files/icon_default_file.png"));
            break;
          }
          std::error_code ec;
          auto ftime = std::filesystem::last_write_time(path, ec);
          std::time_t cftime = 0;
          if (!ec) {
            auto sctp = std::chrono::time_point_cast<
                std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() +
                std::chrono::system_clock::now());
            cftime = std::chrono::system_clock::to_time_t(sctp);
          }

          drawRow(filename, path, true, 0, cftime, tex);
        }
      }

      if (ShowItems) {
        for (auto &itemEntry : recognized_modules_items) {
          const auto &path = itemEntry.second;
          std::filesystem::path fsPath(path);
          std::filesystem::directory_entry fsDirectoryEntry(path);
          std::string filenameString = fsPath.filename().string();

          if (!ShowHidden && !filenameString.empty() &&
              filenameString[0] == '.') {
            continue;
          }

          if (!ExtentionsFilters.empty()) {
            bool match = false;
            for (const auto &ext : ExtentionsFilters) {
              if (filenameString.size() >= ext.size() &&
                  filenameString.compare(filenameString.size() - ext.size(),
                                         ext.size(), ext) == 0) {
                match = true;
                break;
              }
            }

            if (!match) {
              continue;
            }
          }

          if (!areStringsSimilar(filenameString, ProjectSearch, threshold) &&
              !isOnlySpacesOrEmpty(ProjectSearch.c_str()))
            continue;

          std::uintmax_t folderSize = getDirectorySize(path);
          std::string folderSizeString = formatFileSize(folderSize);
          ImGui::PushID(filenameString.c_str());

          ImTextureID tex =
              Application::GetCurrentRenderedWindow()->get_texture(
                  Application::CookPath(
                      "resources/imgs/icons/files/icon_default_file.png"));

          std::error_code ec;
          size_t fsize = fsDirectoryEntry.is_regular_file(ec)
                             ? std::filesystem::file_size(path, ec)
                             : 0;
          auto ftime = std::filesystem::last_write_time(path, ec);
          std::time_t cftime = 0;
          if (!ec) {
            auto sctp = std::chrono::time_point_cast<
                std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() +
                std::chrono::system_clock::now());
            cftime = std::chrono::system_clock::to_time_t(sctp);
          }

          drawRow(filenameString, path, false, fsize, cftime, tex,
                  itemEntry.first);
        }
      }

      if (ShowFiles) {
        for (auto &fileEntry : files) {
          const auto &path = fileEntry.path();
          std::string filename = path.filename().string();

          if (!ShowHidden && !filename.empty() && filename[0] == '.') {
            continue;
          }

          if (!ExtentionsFilters.empty()) {
            bool match = false;
            for (const auto &ext : ExtentionsFilters) {
              if (filename.size() >= ext.size() &&
                  filename.compare(filename.size() - ext.size(), ext.size(),
                                   ext) == 0) {
                match = true;
                break;
              }
            }

            if (!match) {
              continue;
            }
          }

          if (!areStringsSimilar(filename, ProjectSearch, threshold) &&
              !isOnlySpacesOrEmpty(ProjectSearch.c_str()))
            continue;

          ImTextureID tex =
              Application::GetCurrentRenderedWindow()->get_texture(
                  Application::CookPath(
                      "resources/imgs/icons/files/icon_default_file.png"));
          FileTypes type = detect_file(path.string());
          std::error_code ec;
          size_t fsize = fileEntry.is_regular_file(ec)
                             ? std::filesystem::file_size(path, ec)
                             : 0;
          auto ftime = std::filesystem::last_write_time(path, ec);
          std::time_t cftime = 0;
          if (!ec) {
            auto sctp = std::chrono::time_point_cast<
                std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() +
                std::chrono::system_clock::now());
            cftime = std::chrono::system_clock::to_time_t(sctp);
          }

          drawRow(filename, path.string(), false, fsize, cftime, tex);
        }
      }
      ImGui::EndTable();
    }
  }

  if (m_ShowMode == ContentShowMode::Objects) {
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

    ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (ImGui::IsWindowHovered() &&
        ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
      m_Selected.clear();
      ImGui::OpenPopup("EmptySpacePopup");
    }

    if (ImGui::IsWindowHovered() &&
        ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
      m_Selected.clear();
    }

    if (ImGui::BeginPopup("EmptySpacePopup")) {
      ImGui::GetFont()->Scale = 0.9;
      ImGui::PushFont(ImGui::GetFont());

      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

      CherryKit::SeparatorText("Add");
      if (ImGui::MenuItem("Add", "Add a component",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_add.png")),
                          NULL)) {
        SpawnAddWindow();
      }
      if (ImGui::MenuItem("Import", "Import a component",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_import.png")),
                          NULL)) {
      }

      CherryKit::SeparatorText("Create");
      if (ImGui::MenuItem("Create new folder", "Create a folder here",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_add_folder.png")),
                          NULL)) {
        CreateFolder();
      }
      if (ImGui::MenuItem("Create new file", "Create empty file here",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_add_file.png")),
                          NULL)) {
        CreateFile();
      }

      CherryKit::SeparatorText("Actions");

      if (ImGui::MenuItem("Paste selection", "Paste selection here",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_down.png")),
                          NULL)) {
        if (m_PastePathsCallback) {
          m_PastePathsCallback({m_CurrentDirectory.string()});
        }
      }

      if (ImGui::MenuItem("Select All", "Select eveything on this directory",
                          Cherry::GetTexture(Cherry::GetPath(
                              "resources/imgs/icons/misc/icon_stack.png")),
                          NULL)) {
      }

      ImGui::GetFont()->Scale = oldfontsize;
      ImGui::PopFont();

      ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    if (ShowFolders) {
      for (auto &directoryEntry : directories) {
        const auto &path = directoryEntry.path();
        std::string filenameString = path.filename().string();

        bool selected = (std::find(m_Selected.begin(), m_Selected.end(),
                                   path) != m_Selected.end());

        if (areStringsSimilar(filenameString, ProjectSearch, threshold) ||
            isOnlySpacesOrEmpty(ProjectSearch.c_str())) {
          ImGui::PushID(filenameString.c_str());

          float padding = 10.0f;
          float logoSize = 40.0f;
          float borderRadius = 5.0f;

          float availableWidth = ImGui::GetContentRegionAvail().x;
          float cardHeight = logoSize + padding * 2;
          ImVec2 cursorPos = ImGui::GetCursorScreenPos();
          ImVec2 cardSize(availableWidth, cardHeight);

          if (ImGui::InvisibleButton(("folder_card_" + filenameString).c_str(),
                                     cardSize)) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
              std::cout << "[LOG] Double click on: " << path.string()
                        << std::endl;
              ChangeDirectory(path);
            } else {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path);
              } else {
                m_Selected.clear();
                m_Selected.push_back(path);
              }
            }
          }

          if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
              ChangeDirectory(path);
            }
          }

          if (ImGui::BeginPopupContextItem("ContextPopup")) {

            CherryKit::SeparatorText("Main");

            if (ImGui::MenuItem("Rename", "Ctrl + R")) {
              pathToRename = path.string();
            }

            {
              std::string label =
                  "Cut (" + std::to_string(m_Selected.size()) + ") selected";

              if (ImGui::MenuItem(label.c_str(), "Ctrl + X")) {
                if (m_CutPathsCallback) {
                  m_CutPathsCallback(m_Selected, true);
                  for (auto &path : m_Selected) {
                    m_CutSelection.push_back(path);
                  }
                }

                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            if (m_CutSelection.size() > 0) {
              std::string label = "Cut in addition (" +
                                  std::to_string(m_CutSelection.size()) +
                                  " copies)";
              if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + X")) {
                if (m_CutPathsCallback) {
                  m_CutPathsCallback(m_Selected, true);

                  for (auto &path : m_Selected) {
                    m_CutSelection.push_back(path);
                  }
                }
                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }

            {
              std::string label =
                  "Copy (" + std::to_string(m_Selected.size()) + ") selected";

              if (ImGui::MenuItem(label.c_str(), "Ctrl + C")) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);
                  for (auto &path : m_Selected) {
                    m_CopySelection.push_back(path);
                  }
                }

                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            if (m_CopySelection.size() > 0) {
              std::string label = "Copy in addition (" +
                                  std::to_string(m_CopySelection.size()) +
                                  " copies)";
              if (ImGui::MenuItem(label.c_str(), "Ctrl + Alt + C")) {
                if (m_CopyPathsCallback) {
                  m_CopyPathsCallback(m_Selected, true);

                  for (auto &path : m_Selected) {
                    m_CopySelection.push_back(path);
                  }
                }
                m_Selected.clear();
                ImGui::CloseCurrentPopup();
              }
            }

            if (ImGui::MenuItem("Delete", "Suppr")) {
              if (m_DeletePathCallback) {
                m_DeletePathCallback(path.string());
              }
              m_Selected.clear();
              ImGui::CloseCurrentPopup();
            }

            if (m_Selected.size() > 1) {
              std::string label =
                  "Delete (" + std::to_string(m_Selected.size()) + " selected)";
              if (ImGui::MenuItem(label.c_str(), "Alt + Suppr")) {
                if (m_DeletePathCallback) {
                  for (auto &path : m_Selected) {
                    m_DeletePathCallback(path);
                  }
                }
                m_Selected.clear();

                ImGui::CloseCurrentPopup();
              }
            }

            CherryKit::SeparatorText("Customizations");

            ImGui::EndPopup();
          }

          ImU32 bgColor =
              selected ? IM_COL32(80, 80, 240, 255) : IM_COL32(40, 40, 40, 255);
          ImU32 borderColor = selected ? IM_COL32(150, 150, 255, 255)
                                       : IM_COL32(90, 90, 90, 255);

          ImDrawList *drawList = ImGui::GetWindowDrawList();

          drawList->AddRectFilled(
              cursorPos,
              ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
              bgColor, borderRadius);
          drawList->AddRect(
              cursorPos,
              ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
              borderColor, borderRadius);

          ImVec2 logoPos(cursorPos.x + padding,
                         cursorPos.y + (cardHeight - logoSize) * 0.5f);
          ImGui::SetCursorScreenPos(logoPos);
          FolderButton("folder_icon", ImVec2(logoSize, logoSize), path.string(),
                       GetContentBrowserFolderColor(path.string()));

          ImVec2 logoEnd(logoPos.x + logoSize, logoPos.y + logoSize);

          ImGui::PushFont(ImGui::GetFont());
          ImGui::GetFont()->Scale = 0.9f;

          ImVec2 textPos(logoEnd.x + padding, cursorPos.y + padding);
          ImGui::SetCursorScreenPos(textPos);

          ImU32 textColor = IM_COL32(255, 255, 255, 255);
          ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
          ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
          static std::unordered_map<std::string, char[256]> renameBuffers;

          if (pathToRename == path) {
            ImGui::PushID(path.c_str());
            auto &buffer = renameBuffers[path.string()];
            if (buffer[0] == '\0') {
              std::strncpy(buffer, filenameString.c_str(), sizeof(buffer));
              buffer[sizeof(buffer) - 1] = '\0';
              ImGui::SetKeyboardFocusHere();
            }
            if (ImGui::InputText("##rename_input", buffer, sizeof(buffer),
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
              VortexMaker::SubmitRename(pathToRename, buffer);
              renameBuffers.erase(pathToRename);
              pathToRename.clear();
            }
            ImGui::PopID();
          } else {
            DrawHighlightedText(drawList, textPos, filenameString.c_str(),
                                ProjectSearch.c_str(), highlightColor,
                                textColor, highlightTextColor);
          }

          ImGui::GetFont()->Scale = oldfontsize;
          ImGui::PopFont();

          std::string folderSizeStr = "[dir]";
          std::string lastModifiedStr = "???";

          ImVec2 rightTextPos(cursorPos.x + cardSize.x - padding - 200,
                              cursorPos.y + padding);
          ImGui::SetCursorScreenPos(rightTextPos);
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
          ImGui::Text("%s | %s", folderSizeStr.c_str(),
                      lastModifiedStr.c_str());
          ImGui::PopStyleColor();

          float lineOffset = 15.0f;
          ImVec2 lineStart(cursorPos.x + cardSize.x - lineOffset,
                           cursorPos.y + padding);
          ImVec2 lineEnd(cursorPos.x + cardSize.x - lineOffset,
                         cursorPos.y + cardSize.y - padding);
          drawList->AddLine(lineStart, lineEnd, IM_COL32(255, 200, 0, 255),
                            3.0f);

          float versionBoxWidth = 10.0f, versionBoxHeight = 20.0f;
          ImVec2 versionPos(
              cursorPos.x + cardSize.x - versionBoxWidth - padding,
              cursorPos.y + cardSize.y - versionBoxHeight - padding);
          drawList->AddRectFilled(versionPos,
                                  ImVec2(versionPos.x + versionBoxWidth,
                                         versionPos.y + versionBoxHeight),
                                  IM_COL32(0, 0, 0, 255), borderRadius);

          ImGui::SetCursorScreenPos(
              ImVec2(cursorPos.x, cursorPos.y + cardSize.y + padding));

          ImGui::PopID();
        }
      }
    }

    if (ShowItems) {
      for (auto &itemEntry : recognized_modules_items) {
        const auto &path = itemEntry.second;
        std::filesystem::path fsPath(path);
        std::string filenameString = fsPath.filename().string();

        bool selected = false;

        if (!ShowHidden && !filenameString.empty() &&
            filenameString[0] == '.') {
          continue;
        }

        if (!ExtentionsFilters.empty()) {
          bool match = false;
          for (const auto &ext : ExtentionsFilters) {
            if (filenameString.size() >= ext.size() &&
                filenameString.compare(filenameString.size() - ext.size(),
                                       ext.size(), ext) == 0) {
              match = true;
              break;
            }
          }

          if (!match) {
            continue;
          }
        }

        if (std::find(m_Selected.begin(), m_Selected.end(), path) !=
            m_Selected.end()) {
          selected = true;
        }

        if (areStringsSimilar(filenameString, ProjectSearch, threshold) ||
            isOnlySpacesOrEmpty(ProjectSearch.c_str())) {
          std::uintmax_t folderSize = getDirectorySize(path);
          std::string folderSizeString = formatFileSize(folderSize);
          ImGui::PushID(filenameString.c_str());

          if (HorizontalItemCard(
                  filenameString, path, itemEntry.first->m_Description,
                  folderSizeString, selected,
                  Application::CookPath(
                      "resources/imgs/icons/files/icon_picture_file.png"),
                  IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                  Cherry::HexToImU32(itemEntry.first->m_LineColor), 100.0f,
                  5.0f, false, itemEntry.first)) {
            if (ImGui::IsMouseDoubleClicked(0)) {
              // itemEntry.first->f_Execute(path);
              // VXERROR("te", "tyr");
            }

            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
              m_Selected.push_back(path);
            } else {
              m_Selected.clear();
              m_Selected.push_back(path);
            }
          }

          ImGui::PopID();
          ImGui::NextColumn();
        }
      }
    }

    if (ShowFiles) {
      for (auto &fileEntry : files) {
        const auto &path = fileEntry.path();
        std::string filenameString = path.filename().string();

        bool selected = false;
        if (!ShowHidden && !filenameString.empty() &&
            filenameString[0] == '.') {
          continue;
        }

        if (!ExtentionsFilters.empty()) {
          bool match = false;
          for (const auto &ext : ExtentionsFilters) {
            if (filenameString.size() >= ext.size() &&
                filenameString.compare(filenameString.size() - ext.size(),
                                       ext.size(), ext) == 0) {
              match = true;
              break;
            }
          }

          if (!match) {
            continue;
          }
        }

        if (std::find(m_Selected.begin(), m_Selected.end(), path) !=
            m_Selected.end()) {
          selected = true;
        }

        if (areStringsSimilar(filenameString, ProjectSearch, threshold) ||
            isOnlySpacesOrEmpty(ProjectSearch.c_str())) {
          std::error_code ec;
          size_t fileSize = 0;

          if (fileEntry.is_regular_file(ec)) {
            fileSize += std::filesystem::file_size(fileEntry.path(), ec);
          }

          std::string fileSizeString = formatFileSize(fileSize);
          ImGui::PushID(filenameString.c_str());

          FileTypes fileType = detect_file(path.string());

          switch (fileType) {
          case FileTypes::File_PICTURE: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "Picture file",
                    fileSizeString, selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_picture_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(255, 100, 150, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_GIT: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "Git File", fileSizeString,
                    selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_git_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_H: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "C Header File",
                    fileSizeString, selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_c_h_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(220, 100, 220, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_C: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "C Source File",
                    fileSizeString, selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_c_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_HPP: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "C++ Header File",
                    fileSizeString, selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_cpp_h_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_CPP: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "C++ Source File",
                    fileSizeString, selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_cpp_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(100, 100, 255, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          case FileTypes::File_INI: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "Init File", fileSizeString,
                    selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_ini_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(150, 150, 150, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }
          default: {
            if (HorizontalItemCard(
                    filenameString, path.string(), "File", fileSizeString,
                    selected,
                    Application::CookPath(
                        "resources/imgs/icons/files/icon_unknow_file.png"),
                    IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                    IM_COL32(150, 150, 150, 255))) {
              if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                  ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
                m_Selected.push_back(path.string());
              } else {
                m_Selected.clear();
                m_Selected.push_back(path.string());
              }
            }
            break;
          }

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
          }
          float oldsize = ImGui::GetFont()->Scale;

          ImGui::PopID();
        }
      }
    }
  }
}

void ContentBrowserAppWindow::RenderDetailsBar() {}

std::shared_ptr<Cherry::AppWindow> &ContentBrowserAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<ContentBrowserAppWindow>
ContentBrowserAppWindow::Create(const std::string &name,
                                const std::string &base_path) {
  auto instance = std::shared_ptr<ContentBrowserAppWindow>(
      new ContentBrowserAppWindow(name, base_path));
  instance->SetupRenderCallback();
  return instance;
}

void ContentBrowserAppWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void ContentBrowserAppWindow::Render() {
  const float splitterWidth = 1.5f;
  const float margin = 10.0f;

  auto &children = m_Childs;
  ImVec2 availableSize = ImGui::GetContentRegionAvail();

  for (size_t i = 0; i < children.size(); ++i) {
    auto &child = children[i];

    if (child.m_Name == "RenderFiltersBar") {
      child.m_Disabled = !m_ShowFilterPannel;
    }

    if (child.m_Name == "RenderDetailsBar") {
      child.m_Disabled = !m_ShowThumbnailVisualizer;
    }
  }

  if (m_ShowFilterPannel != m_PreviousFilterPannelState ||
      m_ShowThumbnailVisualizer != m_PreviousThumbnailVisualizerState) {
    m_ChildSizesInitialized = false;
  }

  if (!m_ChildSizesInitialized) {
    float totalAvailableWidth =
        availableSize.x - (children.size() - 1) * splitterWidth;
    int visibleChildrenCount = 0;

    for (auto &child : children) {
      if (!child.m_Disabled)
        visibleChildrenCount++;
    }

    float defaultSize = visibleChildrenCount > 0
                            ? totalAvailableWidth / visibleChildrenCount
                            : 0.0f;

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

    m_ChildSizesInitialized = true;
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

    if (child.m_Name == "RenderSideBar") {
      c_SideBarWidth = child.m_Size;
    }
    if (child.m_Name == "RenderFiltersBar") {
      c_FilterBarWidth = child.m_Size;
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, child.m_BackgroundColor);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    std::string childname =
        child.m_Name + "##cbchildnh" + m_AppWindow->m_Name + child.m_Name;
    ImGui::BeginChild(childname.c_str(), ImVec2(child.m_Size, availableSize.y),
                      true);

    child.m_Child();

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    int nextChildIndex = -1;
    for (size_t j = i + 1; j < children.size(); ++j) {
      if (!children[j].m_Disabled) {
        nextChildIndex = j;
        break;
      }
    }

    if (nextChildIndex != -1) {
      VortexEditor::ContentBrowserChild &next_child = children[nextChildIndex];

      if (i + 1 < children.size() && !children[i].m_Disabled &&
          !next_child.m_Disabled) {
        ImGui::SameLine();

        std::string lab =
            child.m_Name + m_AppWindow->m_Name + "####" + child.m_Name;

        CherryStyle::RemoveMarginX(5.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Button(lab.c_str(), ImVec2(splitterWidth, -1));
        ImGui::PopStyleColor();

        CherryStyle::RemoveMarginX(5.0f);

        if (ImGui::IsItemHovered()) {
          ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (ImGui::IsItemActive()) {
          float delta = ImGui::GetIO().MouseDelta.x;

          if (child.m_Size >= 50.0f || child.m_Size == 0.0f) {
            if (next_child.m_Size >= 50.0f || next_child.m_Size == 0.0f) {
              child.m_Size += delta;
              next_child.m_Size -= delta;
            }
          }

          child.m_Size = (std::max)(child.m_Size, 50.0f);
          next_child.m_Size = (std::max)(next_child.m_Size, 50.0f);
        }

        ImGui::SameLine();
      }
    }
  }

  m_PreviousFilterPannelState = m_ShowFilterPannel;
  m_PreviousThumbnailVisualizerState = m_ShowThumbnailVisualizer;
}

} // namespace VortexEditor