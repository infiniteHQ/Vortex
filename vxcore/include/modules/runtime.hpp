//
//  runtime.hpp
//  Headers for modules runtime, ignition and starting
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.hpp"

#ifndef MODULE_RUNTIME_H
#define MODULE_RUNTIME_H

namespace vxe {
  VORTEX_API void start_module(const std::string &module_name);
  VORTEX_API void start_all_modules();
  VORTEX_API void bootstrapp_all_modules();
  VORTEX_API void final_start_modules(
      const std::string &module_name,
      std::shared_ptr<std::vector<std::string>> processed_modules);
}  // namespace vxe

#endif