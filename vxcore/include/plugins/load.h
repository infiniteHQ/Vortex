#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef PLUGIN_LOAD_H
#define PLUGIN_LOAD_H

namespace vxe {
  VORTEX_API void load_system_plugins(std::vector<std::shared_ptr<PluginInterface>> &sys_plugins);
  VORTEX_API void load_editor_plugins(
      const std::string &directory,
      std::vector<void *> &plugin_handles,
      std::vector<std::shared_ptr<PluginInterface>> &plugins);
}  // namespace vxe

#endif