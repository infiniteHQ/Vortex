//
//  json.hpp
//  Headers for some json utilities
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_JSON_HPP
#define VORTEX_JSON_HPP

namespace vxe {
  VORTEX_API nlohmann::json dump_json(const std::string &file);
  VORTEX_API void populate_json(const nlohmann::json &json_data, const std::string &file);

}  // namespace vxe

#endif  // VORTEX_JSON_HPP