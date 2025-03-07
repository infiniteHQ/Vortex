#include "../../../include/vortex.h"

void VortexMaker::createFolderIfNotExists(const std::string &path)
{
    try
    {
        if (!fs::exists(path))
        {
            fs::create_directories(path);
        }
    }
    catch (const std::exception &e)
    {
        VortexMaker::LogError("Core", "Error while creating folder '" + path + "' (" + e.what() + ")");
    }
}

void VortexMaker::createJsonFileIfNotExists(const std::string &filename, const nlohmann::json &defaultData)
{
    if (!fs::exists(filename))
    {
        try
        {
            std::ofstream file(filename);
            file << std::setw(4) << defaultData << std::endl;
            VortexMaker::LogInfo("Core", "JSON file '" + filename + "' created with default data.");
        }
        catch (const std::exception &ex)
        {
            VortexMaker::LogError("Core", "Error while creating JSON file '" + filename + "'");
            VortexMaker::LogError("Core", ex.what());
        }
    }
    else
    {
        VortexMaker::LogInfo("Core", "JSON file '" + filename + "' already exists.");
    }
}