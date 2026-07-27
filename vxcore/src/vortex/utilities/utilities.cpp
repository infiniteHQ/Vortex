//
//  utilities.cpp
//  Sources of some usefull utility for the internal API
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API std::string vxe::get_current_timestamp() {
  // Get actual time
  std::time_t currentTime = std::time(nullptr);

  // Convert to tm
  std::tm *localTime = std::localtime(&currentTime);

  // Format timestamp
  std::ostringstream oss;
  oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

VORTEX_API void vxe::open_url(const std::string &url) {
#if defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
  std::string command = "open " + url;
  system(command.c_str());
#elif defined(__linux__)
  if (fork() == 0) {
    execlp("xdg-open", "xdg-open", url.c_str(), (char *)nullptr);
    _exit(1);
  }
#else
  std::cerr << "Unsupported platform: unable to open URL." << std::endl;
#endif
}

VORTEX_API std::string vxe::convert_path_to_windows_style(const std::string &path) {
  std::string windowsPath = path;
  std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
  return windowsPath;
}

VORTEX_API std::string vxe::replace_placeholders(
    const std::string &command,
    const std::unordered_map<std::string, std::string> &replacements) {
  std::string result = command;
  for (const auto &[placeholder, value] : replacements) {
    size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
      result.replace(pos, placeholder.length(), value);
      pos += value.length();
    }
  }
  return result;
}

VORTEX_API std::string vxe::base64_encode(const std::string &data) {
  static const std::string base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

  std::string ret;
  int i = 0, j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];
  size_t in_len = data.size();
  size_t pos = 0;

  while (in_len--) {
    char_array_3[i++] = data[pos++];
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; i < 4; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; j < i + 1; j++)
      ret += base64_chars[char_array_4[j]];

    while (i++ < 3)
      ret += '=';
  }

  return ret;
}

bool vxe::is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string vxe::base64_decode(const std::string &encoded) {
  const std::string base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";

  size_t in_len = encoded.size();
  int i = 0, j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && (encoded[in_] != '=') && vxe::is_base64(encoded[in_])) {
    char_array_4[i++] = encoded[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; i < 3; i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; j < i - 1; j++)
      ret += char_array_3[j];
  }

  return ret;
}

bool vxe::is_module_flashlink(const std::string &str) {
  static const std::regex pattern("^mod:[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$");
  return std::regex_match(str, pattern);
}

bool vxe::is_plugin_flashlink(const std::string &str) {
  static const std::regex pattern("^plu:[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$");
  return std::regex_match(str, pattern);
}

bool vxe::is_content_flashlink(const std::string &str) {
  static const std::regex pattern("^con:[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$");
  return std::regex_match(str, pattern);
}

bool vxe::is_template_flashlink(const std::string &str) {
  static const std::regex pattern("^tem:[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$");
  return std::regex_match(str, pattern);
}

bool vxe::run_command_capture_output(const std::string &cmd, std::string &output) {
#ifdef _WIN32
  FILE *pipe = _popen(cmd.c_str(), "r");
#else
  FILE *pipe = popen(cmd.c_str(), "r");
#endif
  if (!pipe) {
    return false;
  }

  std::array<char, 256> buffer{};
  output.clear();
  while (fgets(buffer.data(), (int)buffer.size(), pipe) != nullptr) {
    output += buffer.data();
  }

#ifdef _WIN32
  _pclose(pipe);
#else
  pclose(pipe);
#endif
  return true;
}

std::string vxe::to_lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}

std::string vxe::trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return "";
  }
  return s.substr(start, end - start + 1);
}

std::string vxe::filename_from_url(const std::string &url) {
  size_t pos = url.find_last_of('/');
  std::string name = (pos == std::string::npos) ? url : url.substr(pos + 1);
  size_t qpos = name.find('?');
  if (qpos != std::string::npos) {
    name = name.substr(0, qpos);
  }
  return name.empty() ? "download.tmp" : name;
}

bool vxe::ensure_directory_exists(const std::string &path) {
  std::error_code ec;
  if (std::filesystem::exists(path, ec)) {
    return std::filesystem::is_directory(path, ec);
  }
  return std::filesystem::create_directories(path, ec);
}