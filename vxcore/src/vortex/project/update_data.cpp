//
//  update_data.cpp
//  Sources for project data update
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::update_project_data(const std::string &old_name, const std::string &path) {
  std::string dataPath = vxe::get_home_directory() + "/.vx/data/";
  vxe::create_folder_if_not_exists(dataPath);

  std::string configFile = dataPath + "/projects.json";
}