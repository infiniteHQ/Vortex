#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_DELETE_H
#define PLUGIN_DELETE_H

namespace vxe {
  VORTEX_API void delete_project_plugin(const std::string &name, const std::string &version);
}

#endif