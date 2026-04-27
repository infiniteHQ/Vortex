//
//  populate.cpp
//  Sources for populating a file with json values
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::populate_json(const nlohmann::json &json_data, const std::string &file) {
  // Check if the directory of the file exists before opening it
  std::filesystem::path filePath(file);
  std::filesystem::path directory = filePath.parent_path();

  if (!directory.empty() && !std::filesystem::exists(directory)) {
    // Create the directory if it does not exist
    std::filesystem::create_directories(directory);
  }

  // Open the file for writing
  std::ofstream fichier(file);

  // Check if the file is opened successfully
  if (!fichier.is_open()) {
    // Throw an exception if the file cannot be opened
    vxe::log_error("Core", "Error while opening file " + file);
    return;
  }

  // Write the JSON data to the file
  fichier << json_data.dump(4);  // Pretty-print with 4 spaces indent

  // Close the file
  fichier.close();
}
