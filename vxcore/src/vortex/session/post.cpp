//
//  post.cpp
//  Sources of the user session posting features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::post_session_state(const std::string &post_topic) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Init the path of the sessions data.
  std::string session_path = vxe::get_home_directory() + "/.vx/sessions";

  // Init the path of current session data folder.
  std::string current_session_path = session_path + "/.vx/sessions/" + post_topic;

  // Create the session folder if not exist
  vxe::create_folder_if_not_exists(session_path);

  // Create the current session folder if not exist
  vxe::create_folder_if_not_exists(current_session_path);

  if (!ctx->state.master_initialized) {
    nlohmann::json json;
    json["version"] = ctx->version;

    // Store JSON into vortex.config file
    vxe::create_file_if_not_exists(session_path + "/vortex.json", json);

    ctx->state.master_initialized = true;
  }

  if (ctx->state.logs_modified) {
  }

  if (ctx->state.last_used_module_modified) {
    // Update logs metrics (logs journal are handle by spdlog)

    nlohmann::json json;
    json["name"] = ctx->state.last_used_module->name();
    json["version"] = ctx->state.last_used_module->version();
    json["author"] = ctx->state.last_used_module->author();

    // Store JSON into vortex.config file
    vxe::create_file_if_not_exists(session_path + "/last_module.json", json);
  }
}

VORTEX_API void vxe::post_session_core_dump(const std::string &post_topic) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Init the path of the sessions data.
  std::string session_path = vxe::get_home_directory() + "/.vx/sessions";

  // Init the path of current session data folder.
  std::string current_session_path = session_path + "/.vx/sessions/" + post_topic;

  // Create the session folder if not exist
  vxe::create_folder_if_not_exists(session_path);

  // Create the current session folder if not exist
  vxe::create_folder_if_not_exists(current_session_path);
}