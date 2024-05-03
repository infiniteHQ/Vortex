#include "../../../include/vortex.h"

VORTEX_API void VortexMaker::UpdateProjectData()
{
    std::string dataPath = VortexMaker::getHomeDirectory() + "/.vx/data/";
    VortexMaker::createFolderIfNotExists(dataPath);

    std::string configFile = dataPath + "/projects.json";
}