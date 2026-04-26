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