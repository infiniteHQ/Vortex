//
//  load.hpp
//  Headers for modules loading functions
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.hpp"

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