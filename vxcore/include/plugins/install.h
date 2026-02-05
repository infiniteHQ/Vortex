#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef PLUGIN_INSTALL_H
#define PLUGIN_INSTALL_H

namespace VortexMaker {
VORTEX_API void InstallPlugin(const std::string &directory);
VORTEX_API std::shared_ptr<PluginInterface>
FindPluginInDirectory(const std::string &directory);
VORTEX_API std::vector<std::shared_ptr<PluginInterface>>
FindPluginsInDirectory(const std::string &directory);
} // namespace VortexMaker

#endif