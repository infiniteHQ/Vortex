#pragma once
#include <vortex.h>

#ifndef VORTEX_JSON_HPP
#define VORTEX_JSON_HPP

namespace vxe {
  VORTEX_API nlohmann::json dump_json(const std::string &file);
  VORTEX_API void populate_json(const nlohmann::json &json_data, const std::string &file);

}  // namespace vxe

#endif  // VORTEX_JSON_HPP