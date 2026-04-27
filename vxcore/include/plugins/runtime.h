#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

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