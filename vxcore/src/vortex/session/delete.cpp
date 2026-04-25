#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::DeleteSessionTopic(const std::string &post_topic) {
  std::string topic_path =
      vxe::getHomeDirectory() + "/.vx/sessions/" + post_topic;

  {
    std::string cmd = "rm -rf " + topic_path;
    if (std::system(cmd.c_str()) == 0) {
      vxe::LogInfo("Core",
                   "Session \"" + post_topic + "\" deleted with success !");
    } else {
      vxe::LogError("Core", "Failed to delete session \"" + post_topic + "\"!");
    }
  }
}