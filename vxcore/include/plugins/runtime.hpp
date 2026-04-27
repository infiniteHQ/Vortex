//
//  runtime.hpp
//  Headers for the runtime of plugins (starting, ignition)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.hpp"

#ifndef PLUGIN_RUNTIME_H
#define PLUGIN_RUNTIME_H

namespace vxe {
  VORTEX_API void start_plugin(const std::string &plugin_name);
  VORTEX_API void start_all_plugins();
  VORTEX_API void bootstrapp_all_plugins();
  VORTEX_API void final_start_plugin(
      const std::string &plugin_name,
      std::shared_ptr<std::vector<std::string>> processed_plugins);
}  // namespace vxe

#endif