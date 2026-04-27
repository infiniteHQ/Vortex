#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef PLUGIN_INSTALL_H
#define PLUGIN_INSTALL_H

namespace vxe {
  VORTEX_API void InstallPluginToSystem(const std::string &path);
  VORTEX_API void InstallPlugin(const std::string &plugin_name, const std::string &version, bool &restart_plugins);
  VORTEX_API void AddPluginToProject(const std::string &plugin_name);
  VORTEX_API std::shared_ptr<PluginInterface> FindPluginInDirectory(const std::string &directory);
  VORTEX_API std::vector<std::shared_ptr<PluginInterface>> FindPluginsInDirectory(const std::string &directory);
}  // namespace vxe

#endif