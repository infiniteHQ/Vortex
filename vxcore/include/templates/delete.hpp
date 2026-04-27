//
//  delete.hpp
//  Headers for the deletion of templates
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef TEMPLATE_DELETE_H
#define TEMPLATE_DELETE_H

namespace vxe {
  VORTEX_API void delete_system_template(const std::string &name, const std::string &version);
}

#endif