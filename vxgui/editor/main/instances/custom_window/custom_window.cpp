//
//  custom_window.cpp
//  Sources for custom windows (intended for plugins and modular uses)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "custom_window.hpp"

namespace vxe {

  CustomWindow::CustomWindow(const std::string &name, const std::shared_ptr<PluginInterface> &plugin) : plugin_(plugin) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);

    app_window_->SetClosable(true);
    app_window_->SetCloseCallback([this]() { Cherry::DeleteAppWindow(app_window_); });
  }

  CustomWindow::~CustomWindow() {
    if (lua_func_ref_ != LUA_NOREF) {
      lua_State *L = vxe::script::get_scripting_engine().get_state();
      luaL_unref(L, LUA_REGISTRYINDEX, lua_func_ref_);
      lua_func_ref_ = LUA_NOREF;
    }
  }

  std::shared_ptr<Cherry::AppWindow> &CustomWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<CustomWindow> CustomWindow::create_from_function(
      const std::string &name,
      const std::shared_ptr<PluginInterface> &plugin,
      int lua_func_ref) {
    auto instance = std::shared_ptr<CustomWindow>(new CustomWindow(name, plugin));
    instance->lua_func_ref_ = lua_func_ref;
    instance->setup_render_callback();
    return instance;
  }

  std::shared_ptr<CustomWindow> CustomWindow::create_from_file(
      const std::string &name,
      const std::shared_ptr<PluginInterface> &plugin,
      const std::string &lua_file_path) {
    auto instance = std::shared_ptr<CustomWindow>(new CustomWindow(name, plugin));
    instance->lua_file_path_ = lua_file_path;
    instance->setup_render_callback();
    return instance;
  }

  void CustomWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void CustomWindow::render() {
    if (!plugin_)
      return;

    if (!lua_file_path_.empty()) {
      lua_State *cherry_L = Cherry::Script::GetScriptingEngine().GetState();

      vxe::script::set_active_plugin_context(cherry_L, &plugin_);
      Cherry::Script::RenderLuaFreshScript(lua_file_path_);
      vxe::script::clear_active_plugin_context(cherry_L);

    } else if (lua_func_ref_ != LUA_NOREF) {
      lua_State *vx_L = vxe::script::get_scripting_engine().get_state();

      vxe::script::set_active_plugin_context(vx_L, &plugin_);

      lua_rawgeti(vx_L, LUA_REGISTRYINDEX, lua_func_ref_);
      if (lua_pcall(vx_L, 0, 0, 0) != LUA_OK) {
        vxe::log_error("Vortex.Window", lua_tostring(vx_L, -1));
        lua_pop(vx_L, 1);
      }

      vxe::script::clear_active_plugin_context(vx_L);
    }
  }

}  // namespace vxe