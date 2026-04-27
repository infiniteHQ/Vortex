//
//  install.hpp
//  Headers for the installation of templates
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.hpp"

#ifndef TEMPLATE_INSTALL_H
#define TEMPLATE_INSTALL_H

namespace vxe {

  // From anywhere to system
  VORTEX_API void install_template_on_system(const std::string &directory);
  // From system to project
  VORTEX_API void install_template(const std::string &name, const std::string &path);
  VORTEX_API std::vector<std::shared_ptr<TemplateInterface>> find_templates_in_directory(const std::string &directory);

}  // namespace vxe

#endif  // TEMPLATE_INSTALL_H