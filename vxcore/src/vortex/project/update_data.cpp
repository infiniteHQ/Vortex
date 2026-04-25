#include "../../../include/vortex.h"

VORTEX_API void vxe::UpdateProjectData(const std::string &old_name,
                                       const std::string &path) {
  std::string dataPath = vxe::getHomeDirectory() + "/.vx/data/";
  vxe::createFolderIfNotExists(dataPath);

  std::string configFile = dataPath + "/projects.json";
}