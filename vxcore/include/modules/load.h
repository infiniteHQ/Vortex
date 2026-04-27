#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_LOAD_H
#define MODULE_LOAD_H

namespace vxe {
  VORTEX_API void load_system_modules(std::vector<std::shared_ptr<ModuleInterface>> &sys_modules);
  VORTEX_API void load_editor_modules(
      const std::string &directory,
      std::vector<void *> &plugin_handles,
      std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}  // namespace vxe

#endif