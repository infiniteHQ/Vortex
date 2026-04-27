#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef MODULE_DELETE_H
#define MODULE_DELETE_H

namespace vxe {
  VORTEX_API void delete_project_module(const std::string &name, const std::string &version);
}

#endif