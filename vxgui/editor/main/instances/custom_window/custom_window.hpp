//
//  custom_window.hpp
//  Header and declarations for custom windows (intended for plugins and modular uses)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include <plugins/interface.hpp>
#include <vortex/scripting/scripting.hpp>

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef VORTEX_EDITOR_CUSTOM_WINDOW_HPP
#define VORTEX_EDITOR_CUSTOM_WINDOW_HPP

namespace vxe {
  class CustomWindow : public std::enable_shared_from_this<CustomWindow> {
   public:
    ~CustomWindow();

    std::shared_ptr<Cherry::AppWindow> &get_app_window();

    static std::shared_ptr<CustomWindow>
    create_from_function(const std::string &name, const std::shared_ptr<PluginInterface> &plugin, int lua_func_ref);

    static std::shared_ptr<CustomWindow> create_from_file(
        const std::string &name,
        const std::shared_ptr<PluginInterface> &plugin,
        const std::string &lua_file_path);

    void setup_render_callback();
    void render();

   private:
    CustomWindow(const std::string &name, const std::shared_ptr<PluginInterface> &plugin);

    std::shared_ptr<Cherry::AppWindow> app_window_;
    std::shared_ptr<PluginInterface> plugin_;
    int lua_func_ref_ = LUA_NOREF;
    std::string lua_file_path_;
  };
}  // namespace vxe

#endif  // VORTEX_EDITOR_CUSTOM_WINDOW_HPP