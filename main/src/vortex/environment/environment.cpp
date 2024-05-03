#include "../../../include/vortex.h"

VORTEX_API void VortexMaker::InitEnvironment()
{
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        VortexMaker::createFolderIfNotExists(path);
    }
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/modules/";
        VortexMaker::createFolderIfNotExists(path);
    }
    {
        std::string path = VortexMaker::getHomeDirectory() + "/.vx/templates/";
        VortexMaker::createFolderIfNotExists(path);
    }

    {

        std::string path = VortexMaker::getHomeDirectory() + "/.vx/data/";
        std::string file = path + "projects.json";

        nlohmann::json default_data = {
            {"projects", nlohmann::json::array()}
        };

        VortexMaker::createJsonFileIfNotExists(file, default_data);
    }
}