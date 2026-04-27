#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

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