//
//  render_instances.hpp
//  Headers for plugin rendering instances.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../vortex.h"
#include "../vortex_internals.h"

#ifndef PLUGIN_RENDERINSTANCE_H
#define PLUGIN_RENDERINSTANCE_H

class PluginRenderInstance {
 public:
  virtual void render() { };
};

#endif