#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

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
