#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef MODULE_INSTALL_H
#define MODULE_INSTALL_H

namespace vxe {
  VORTEX_API std::shared_ptr<ModuleInterface> find_module_in_directory(const std::string &directory);
  VORTEX_API std::vector<std::shared_ptr<ModuleInterface>> find_modules_in_directory(const std::string &directory);
  VORTEX_API void install_module_to_system(const std::string &path);
  VORTEX_API void install_module(const std::string &module_name, const std::string &version, bool &restart_modules);
  VORTEX_API void add_module_to_project(const std::string &module_name);
}  // namespace vxe

#endif