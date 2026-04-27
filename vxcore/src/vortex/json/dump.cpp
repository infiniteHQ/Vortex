//
//  dump.cpp
//  Sources for json utilities (dumping from file)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

// TODO rename to file_to_json
VORTEX_API nlohmann::json vxe::dump_json(const std::string &file) {
  if (!std::filesystem::exists(file)) {
    vxe::log_error("Core", "File not found: " + file);
    return "{}";
  }

  std::ifstream opened_file(file);

  if (!opened_file.is_open()) {
    vxe::log_error("Core", "Error while opening file " + file);
    return "{}";
  }

  nlohmann::json json_data;

  opened_file >> json_data;

  return json_data;
}
