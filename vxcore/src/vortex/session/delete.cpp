//
//  create.cpp
//  Sources of the user session deletion feature
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::delete_session_topic(const std::string &post_topic) {
  std::string topic_path = vxe::get_home_directory() + "/.vx/sessions/" + post_topic;

  {
    std::string cmd = "rm -rf " + topic_path;
    if (std::system(cmd.c_str()) == 0) {
      vxe::log_info("Core", "Session \"" + post_topic + "\" deleted with success !");
    } else {
      vxe::log_error("Core", "Failed to delete session \"" + post_topic + "\"!");
    }
  }
}