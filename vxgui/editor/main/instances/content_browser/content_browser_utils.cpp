//
//  content_browser_utils.cpp
//  Utilities and stuffs to help high-level features of the content browser
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "content_browser.hpp"

namespace vxe {

  bool ContentBrowser::is_only_spaces_or_empty(const char *str) {
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

  std::string ContentBrowser::format_file_size(size_t size) {
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

  std::uintmax_t ContentBrowser::get_directory_size(const std::filesystem::path &directoryPath) {
    std::uintmax_t size = 0;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
      if (std::filesystem::is_regular_file(entry.path())) {
        size += std::filesystem::file_size(entry.path());
      }
    }

    return size;
  }

  std::string ContentBrowser::to_lower_case(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
  }

  int ContentBrowser::levenshtein_distance(const std::string &s1, const std::string &s2) {
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
          dp[i][j] = (std::min)({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
        }
      }
    }
    return dp[m][n];
  }

  bool ContentBrowser::has_common_letters(const std::string &s1, const std::string &s2) {
    std::unordered_set<char> set1(s1.begin(), s1.end());
    for (char c : s2) {
      if (set1.find(c) != set1.end()) {
        return true;
      }
    }
    return false;
  }

  bool ContentBrowser::are_strings_similar(const std::string &s1, const std::string &s2, double threshold_) {
    std::string lower_s1 = to_lower_case(s1);
    std::string lower_s2 = to_lower_case(s2);

    int dist = levenshtein_distance(lower_s1, lower_s2);
    int maxLength = (std::max)(lower_s1.size(), lower_s2.size());
    double similarity = 1.0 - (static_cast<double>(dist) / maxLength);

    if (std::strlen(project_search_.c_str()) < 5) {
      return similarity >= threshold_ || has_common_letters(lower_s1, lower_s2);
    }

    return similarity >= threshold_;
  }

  std::string ContentBrowser::get_extension(const std::string &path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos)
      return "";
    return path.substr(dot_pos + 1);
  }

  FileTypes ContentBrowser::detect_file(const std::string &path) {
    static const std::unordered_map<std::string, FileTypes> extension_map = {
      // Very low level
      { "asm", FileTypes::File_ASM },
      { "bin", FileTypes::File_BIN },

      // Programming languages
      { "c", FileTypes::File_C },
      { "h", FileTypes::File_H },
      { "cpp", FileTypes::File_CPP },
      { "hpp", FileTypes::File_HPP },
      { "inl", FileTypes::File_INL },
      { "rs", FileTypes::File_RUST },
      { "zig", FileTypes::File_ZIG },
      { "go", FileTypes::File_GO },
      { "java", FileTypes::File_JAVA },
      { "js", FileTypes::File_JAVASCRIPT },
      { "ts", FileTypes::File_TYPESCRIPT },
      { "cob", FileTypes::File_COBOL },
      { "p", FileTypes::File_PASCAL },
      { "pas", FileTypes::File_PASCAL },
      { "carbon", FileTypes::File_CARBON },
      { "py", FileTypes::File_PYTHON },
      { "rb", FileTypes::File_RUBY },
      { "php", FileTypes::File_PHP },
      { "swift", FileTypes::File_SWIFT },
      { "cs", FileTypes::File_CSHARP },
      { "vb", FileTypes::File_VB },
      { "dart", FileTypes::File_DART },
      { "kt", FileTypes::File_KOTLIN },
      { "scala", FileTypes::File_SCALA },
      { "pl", FileTypes::File_PERL },
      { "sh", FileTypes::File_SHELL },
      { "bat", FileTypes::File_BATCH },
      { "lua", FileTypes::File_LUA },
      { "r", FileTypes::File_R },
      { "m", FileTypes::File_MATLAB },
      { "jl", FileTypes::File_JULIA },
      { "sql", FileTypes::File_SQL },
      { "groovy", FileTypes::File_GROOVY },
      { "f90", FileTypes::File_FORTRAN },
      { "erl", FileTypes::File_ERLANG },
      { "ex", FileTypes::File_ELIXIR },
      { "ml", FileTypes::File_OCAML },

      // Web and Markup
      { "html", FileTypes::File_HTML },
      { "htm", FileTypes::File_HTML },
      { "css", FileTypes::File_CSS },
      { "scss", FileTypes::File_CSS },
      { "xml", FileTypes::File_XML },
      { "json", FileTypes::File_JSON },
      { "yaml", FileTypes::File_YAML },
      { "yml", FileTypes::File_YAML },

      // Config
      { "cfg", FileTypes::File_CFG },
      { "ini", FileTypes::File_INI },
      { "env", FileTypes::File_INI },

      // Git
      { "gitignore", FileTypes::File_GIT },
      { "gitmodules", FileTypes::File_GIT },
      { "gitattributes", FileTypes::File_GIT },

      // Documents
      { "txt", FileTypes::File_TXT },
      { "md", FileTypes::File_MD },
      { "rst", FileTypes::File_MD },
      { "doc", FileTypes::File_DOC },
      { "docx", FileTypes::File_DOC },
      { "pdf", FileTypes::File_PDF },
      { "odt", FileTypes::File_DOC },

      // Pictures
      { "png", FileTypes::File_PICTURE },
      { "jpg", FileTypes::File_PICTURE },
      { "jpeg", FileTypes::File_PICTURE },
      { "bmp", FileTypes::File_PICTURE },
      { "gif", FileTypes::File_PICTURE },
      { "svg", FileTypes::File_PICTURE },
      { "webp", FileTypes::File_PICTURE },
      { "ico", FileTypes::File_PICTURE },

      // Videos
      { "mp4", FileTypes::File_VIDEO },
      { "avi", FileTypes::File_VIDEO },
      { "mkv", FileTypes::File_VIDEO },
      { "mov", FileTypes::File_VIDEO },
      { "wmv", FileTypes::File_VIDEO },
      { "flv", FileTypes::File_VIDEO },

      // Audio
      { "mp3", FileTypes::File_AUDIO },
      { "wav", FileTypes::File_AUDIO },
      { "flac", FileTypes::File_AUDIO },
      { "aac", FileTypes::File_AUDIO },
      { "ogg", FileTypes::File_AUDIO },

      // Archives
      { "zip", FileTypes::File_ARCHIVE },
      { "rar", FileTypes::File_ARCHIVE },
      { "7z", FileTypes::File_ARCHIVE },
      { "tar", FileTypes::File_ARCHIVE },
      { "gz", FileTypes::File_ARCHIVE },
      { "bz2", FileTypes::File_ARCHIVE },

      // Miscellaneous
      { "log", FileTypes::File_LOG },
      { "bak", FileTypes::File_BACKUP },
      { "tmp", FileTypes::File_TEMP },
      { "dat", FileTypes::File_DATA },
    };

    std::string filename = path;
    auto slash_pos = path.find_last_of("/\\");
    if (slash_pos != std::string::npos)
      filename = path.substr(slash_pos + 1);

    static const std::unordered_map<std::string, FileTypes> name_map = {
      { "LICENSE", FileTypes::File_LICENSE },
      { "LICENSE.txt", FileTypes::File_LICENSE },
      { "vortex.config", FileTypes::File_VORTEX },
    };

    auto name_it = name_map.find(filename);
    if (name_it != name_map.end())
      return name_it->second;

    std::string extension = get_extension(path);
    auto it = extension_map.find(extension);
    if (it != extension_map.end())
      return it->second;
    else
      return FileTypes::File_UNKNOWN;
  }

  bool ContentBrowser::has_extension(const std::string &ext) {
    return std::find(extentions_filters_.begin(), extentions_filters_.end(), ext) != extentions_filters_.end();
  }

  void ContentBrowser::toggle_extension(const std::string &ext, bool *state) {
    if (*state) {
      if (!has_extension(ext)) {
        extentions_filters_.push_back(ext);
      }
    } else {
      extentions_filters_.erase(
          std::remove(extentions_filters_.begin(), extentions_filters_.end(), ext), extentions_filters_.end());
    }
  }

  void ContentBrowser::refresh_custom_folders() {
    vxe::FetchCustomFolders();
    favorite_folders_.clear();
    folder_colors_.clear();
    for (auto custom_folder : vxe::GetCurrentContext()->IO.contentbrowser_customfolders) {
      if (custom_folder->m_IsFav) {
        favorite_folders_.push_back(custom_folder->path);
      }

      if (custom_folder->m_Color != "#fdaa00" || custom_folder->m_Color != default_folder_color_) {
        if (custom_folder->m_Color.size() <= 7) {
          custom_folder->m_Color = custom_folder->m_Color + "ff";  // + ff to add a opaque bg if not provided
        }
        folder_colors_.push_back({ custom_folder->path, custom_folder->m_Color });
      }
    }
  }

  void ContentBrowser::refresh_pools() {
    vxe::FetchPools();
    pools_.clear();
    for (auto pool : vxe::GetCurrentContext()->IO.contentbrowser_pools) {
      pools_.push_back(pool);
    }
  }

  void ContentBrowser::change_show_mode(ContentShowMode mode) {
    this->show_mode_ = mode;
  }

  std::string ContentBrowser::get_content_browser_folder_color(const std::string &path) {
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

  void ContentBrowser::set_colored_folder(const std::string &path, const std::string &hex_color) {
    // TODO or DELETE ?
  }

  void ContentBrowser::set_delete_path_callback(const std::function<void(const std::string &)> &foo) {
    delete_path_callback_ = foo;
  }

  void ContentBrowser::set_copy_paths_callback(const std::function<void(const std::vector<std::string> &, bool)> &foo) {
    copy_paths_callback_ = foo;
  }

  void ContentBrowser::set_cut_paths_callback(const std::function<void(const std::vector<std::string> &, bool)> &foo) {
    cut_paths_callback_ = foo;
  }

  void ContentBrowser::set_paste_paths_callback(const std::function<void(const std::string &)> &foo) {
    paste_paths_callback_ = foo;
  }

}  // namespace vxe