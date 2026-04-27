//
//  install.hpp
//  Headers for plugin installation features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.hpp"

#ifndef PLUGIN_INSTALL_H
#define PLUGIN_INSTALL_H

namespace vxe {
  VORTEX_API void install_plugin_to_system(const std::string &path);
  VORTEX_API void install_plugin(const std::string &plugin_name, const std::string &version, bool &restart_plugins);
  VORTEX_API void add_plugin_to_project(const std::string &plugin_name);
  VORTEX_API std::shared_ptr<PluginInterface> find_plugin_in_directory(const std::string &directory);
  VORTEX_API std::vector<std::shared_ptr<PluginInterface>> find_plugins_in_directory(const std::string &directory);
}  // namespace vxe

#endif