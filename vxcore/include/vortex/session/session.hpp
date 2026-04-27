//
//  session.hpp
//  Headers for user session features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_SESSION_HPP
#define VORTEX_SESSION_HPP

namespace vxe {
  VORTEX_API void create_session_topic(const std::string &post_topic);
  VORTEX_API void delete_session_topic(const std::string &post_topic);
  VORTEX_API void post_session_state(const std::string &post_topic);
  VORTEX_API void post_session_core_dump(const std::string &post_topic);
  VORTEX_API nlohmann::json get_last_module_of_last_session(const std::string &post_topic);
  VORTEX_API nlohmann::json get_last_session(const std::string &post_topic);
}  // namespace vxe

#endif  // VORTEX_SESSION_HPP