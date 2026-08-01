//
//  import_window.hpp
//  Headers and declarations for the "import window" of the plugin utility
//
//  Copyright (c) 2026 Infinite
//
//  This work is licensed under the terms of the Apache-2.0 license.
//  For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../../../vxcore/include/vortex.h"
#include "../../../../../../../vxcore/include/vortex_internals.h"

#ifndef PLUGINS_UTILITY_IMPORT_WINDOW_HPP
#define PLUGINS_UTILITY_IMPORT_WINDOW_HPP

namespace vxe {

  class ImportPluginWindow : public std::enable_shared_from_this<ImportPluginWindow> {
   public:
    ImportPluginWindow(const std::string &name);

    // window and rendering
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<ImportPluginWindow> create(const std::string &name);
    void setup_render_callback();
    void render();

   private:
    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // PLUGINS_UTILITY_IMPORT_WINDOW_HPP