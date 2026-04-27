//
//  get.cpp
//  Sources of the user session getting features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API nlohmann::json vxe::get_last_module_of_last_session(const std::string &post_topic) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Init the path of the sessions data.
  std::string session_path = vxe::get_home_directory() + "/.vx/sessions";

  // Init the path of current session data folder.
  std::string current_session_path = session_path + "/.vx/sessions/" + post_topic + "/last_module.json";

  std::ifstream file(current_session_path);

  nlohmann::json jsonContenu;

  if (file) {
    file >> jsonContenu;
  }

  return jsonContenu;
}

VORTEX_API nlohmann::json vxe::get_last_session(const std::string &post_topic) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Init the path of the sessions data.
  std::string session_path = vxe::get_home_directory() + "/.vx/sessions";

  // Init the path of current session data folder.
  std::string current_session_path = session_path + "/.vx/sessions/" + post_topic + "/master_session.json";

  std::ifstream file(current_session_path);

  nlohmann::json jsonContenu;

  if (file) {
    file >> jsonContenu;
  }

  return jsonContenu;
}