#pragma once
#include <vortex.h>

#ifndef VORTEX_UTILITIES_HPP
#define VORTEX_UTILITIES_HPP

namespace vxe {
  VORTEX_API void open_url(const std::string &url);
  VORTEX_API std::string get_current_timestamp();
  VORTEX_API std::string convert_path_to_windows_style(const std::string &path);
  VORTEX_API std::string extract_package_with_tar(const std::string &path, const std::string &tarballName);
  VORTEX_API std::string replace_placeholders(
      const std::string &command,
      const std::unordered_map<std::string, std::string> &replacements);

}  // namespace vxe

#endif  // VORTEX_UTILITIES_HPP