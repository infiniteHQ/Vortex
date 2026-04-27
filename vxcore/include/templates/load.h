#include "../vortex.h"
#include "../vortex_internals.h"
#include "interface.h"

#ifndef TEMPLATE_LOAD_H
#define TEMPLATE_LOAD_H

namespace vxe {
  VORTEX_API void load_system_templates(std::vector<std::shared_ptr<TemplateInterface>> &sys_templates);
  // VORTEX_API void load_editor_modules(const std::string &directory,
  // std::vector<void *> &plugin_handles,
  // std::vector<std::shared_ptr<ModuleInterface>> &plugins);
}  // namespace vxe

#endif