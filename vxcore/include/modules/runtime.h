#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_RUNTIME_H
#define MODULE_RUNTIME_H

namespace vxe {
  VORTEX_API void start_module(const std::string &module_name);
  VORTEX_API void start_all_modules();
  VORTEX_API void bootstrapp_all_modules();
}  // namespace vxe

#endif