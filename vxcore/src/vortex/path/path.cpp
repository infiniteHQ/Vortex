#include "../../../include/vortex.h"

void vxe::createFolderIfNotExists(const std::string &path) {
  try {
    if (!fs::exists(path)) {
      fs::create_directories(path);
    }
  } catch (const std::exception &e) {
    vxe::LogError("Core", "Error while creating folder '" + path + "' (" +
                              e.what() + ")");
  }
}

void vxe::createJsonFileIfNotExists(const std::string &filename,
                                    const nlohmann::json &defaultData) {
  if (!fs::exists(filename)) {
    try {
      std::ofstream file(filename);
      file << std::setw(4) << defaultData << std::endl;
      vxe::LogInfo("Core",
                   "JSON file '" + filename + "' created with default data.");
    } catch (const std::exception &ex) {
      vxe::LogError("Core",
                    "Error while creating JSON file '" + filename + "'");
      vxe::LogError("Core", ex.what());
    }
  } else {
    vxe::LogInfo("Core", "JSON file '" + filename + "' already exists.");
  }
}