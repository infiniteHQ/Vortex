#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief DumpJSON loads JSON data from a file.
 * 
 * This function reads JSON data from the specified file and returns it as a JSON object.
 * 
 * @param file The path to the JSON file to load.
 * @return A JSON object containing the data from the file.
 * @throws std::runtime_error if the file cannot be opened.
 */
VORTEX_API nlohmann::json VortexMaker::DumpJSON(const std::string &file)
{
    // Open the file for reading
    std::ifstream fichier(file);

    // Check if the file is opened successfully
    if (!fichier.is_open())
    {
        // Throw an exception if the file cannot be opened
        throw std::runtime_error("Error while opening file " + file);
    }

    // JSON object to store the data read from the file
    nlohmann::json json_data;

    // Read data from the file into the JSON object
    fichier >> json_data;

    // Return the JSON object containing the data from the file
    return json_data;
}
