//
//  create.cpp
//  Sources for create folders and files on a filesystem
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

void vxe::create_folder_if_not_exists(const std::string &path) {
  try {
    if (!fs::exists(path)) {
      fs::create_directories(path);
    }
  } catch (const std::exception &e) {
    vxe::log_error("Core", "Error while creating folder '" + path + "' (" + e.what() + ")");
  }
}

void vxe::create_file_if_not_exists(const std::string &filename, const nlohmann::json &defaultData) {
  if (!fs::exists(filename)) {
    try {
      std::ofstream file(filename);
      file << std::setw(4) << defaultData << std::endl;
      vxe::log_info("Core", "JSON file '" + filename + "' created with default data.");
    } catch (const std::exception &ex) {
      vxe::log_error("Core", "Error while creating JSON file '" + filename + "'");
      vxe::log_error("Core", ex.what());
    }
  } else {
    vxe::log_info("Core", "JSON file '" + filename + "' already exists.");
  }
}