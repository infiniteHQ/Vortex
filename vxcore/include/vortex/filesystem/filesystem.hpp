//
//  filesystem.hpp
//  Headers for all filesystem manipulation and searching features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_FILESYSTEM_HPP
#define VORTEX_FILESYSTEM_HPP

namespace vxe {
  VORTEX_API void create_folder_if_not_exists(const std::string &path);
  VORTEX_API void create_file_if_not_exists(const std::string &filename, const nlohmann::json &defaultData);

  VORTEX_API std::string get_home_directory();
  VORTEX_API std::vector<std::string> search_files(const std::string &path, const std::string &filename);
  VORTEX_API std::vector<std::string> search_files(const std::string &path, const std::string &filename, int recursions);
  VORTEX_API std::vector<std::string> search_system_files(const std::string &path, const std::string &filename);
  VORTEX_API void search_files_recursive(const fs::path &path, const std::string &filename, std::vector<std::string> &file);
  VORTEX_API std::string search_files_recursive(
      const fs::path &path,
      const std::string &filename,
      std::vector<std::string> &file,
      int recursions,
      int counter);
}  // namespace vxe

#endif  // VORTEX_FILESYSTEM_HPP