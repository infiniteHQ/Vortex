//
//  scripting.cpp
//  Scripting engine sources and utilities for LUA scripting
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vortex/scripting/scripting.hpp>

namespace vxe {
  namespace script {

    namespace fs = std::filesystem;

    static long long get_file_mod_time(const std::string &path) {
      if (!fs::exists(path))
        return 0;
      auto time = fs::last_write_time(path);
      return std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    }

    ScriptingEngine &get_scripting_engine() {
      static ScriptingEngine instance;
      return instance;
    }

    ScriptingEngine::ScriptingEngine() {
      L = luaL_newstate();
      luaL_openlibs(L);

      lua_pushlightuserdata(L, this);
      lua_pushcclosure(L, ScriptingEngine::lua_print, 1);
      lua_setglobal(L, "print");

      register_vortex_api();
    }

    ScriptingEngine::~ScriptingEngine() {
      if (L)
        lua_close(L);
    }

    bool ScriptingEngine::execute(const std::string &code) {
      if (luaL_dostring(L, code.c_str()) != LUA_OK) {
        m_LastError = lua_tostring(L, -1);
        capture_output("[Error] " + m_LastError);
        lua_pop(L, 1);
        return false;
      }
      return true;
    }

    int ScriptingEngine::load_file(const std::string &path, int nargs) {
      if (luaL_loadfile(L, path.c_str()) != LUA_OK) {
        m_LastError = lua_tostring(L, -1);
        capture_output("[Error] " + m_LastError);
        lua_pop(L, 1 + nargs);
        return 0;
      }

      if (nargs > 0) {
        lua_insert(L, -(nargs + 1));
      }

      int baseStack = lua_gettop(L) - (nargs + 1);

      if (m_HotReloadEnabled) {
        m_FileTimestamps[path] = get_file_mod_time(path);
      }

      if (lua_pcall(L, nargs, LUA_MULTRET, 0) != LUA_OK) {
        m_LastError = lua_tostring(L, -1);
        capture_output("[Runtime Error] " + m_LastError);
        lua_pop(L, 1);
        return 0;
      }

      int currentStack = lua_gettop(L);
      return currentStack - baseStack;
    }
    void ScriptingEngine::reload_file(const std::string &path) {
      capture_output("[System] Reloading: " + path);
      load_file(path);
    }

    void ScriptingEngine::capture_output(const std::string &text) {
      if (text.find("[Error]") != std::string::npos || text.find("[Runtime Error]") != std::string::npos) {
        vxe::log_error("Lua", text);
      } else {
        vxe::log_info("Lua", text);
      }
    }

    int ScriptingEngine::lua_print(lua_State *L) {
      ScriptingEngine *engine = (ScriptingEngine *)lua_touserdata(L, lua_upvalueindex(1));
      int n = lua_gettop(L);
      std::string out = "";

      for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
          out += lua_tostring(L, i);
        } else if (lua_isboolean(L, i)) {
          out += lua_toboolean(L, i) ? "true" : "false";
        } else {
          out += lua_typename(L, lua_type(L, i));
        }
        if (i < n)
          out += "\t";
      }

      engine->capture_output(out);
      return 0;
    }

    void ScriptingEngine::enable_hot_reload(const std::string &scriptDir) {
      m_HotReloadEnabled = true;
      m_ScriptDir = scriptDir;
    }

    void ScriptingEngine::check_for_changes() {
      if (!m_HotReloadEnabled)
        return;

      for (auto &[path, lastTime] : m_FileTimestamps) {
        auto currentTime = get_file_mod_time(path);
        if (currentTime > lastTime) {
          m_FileTimestamps[path] = currentTime;
          reload_file(path);
        }
      }
    }

    std::string ScriptingEngine::get_last_output() const {
      return m_OutputHistory.empty() ? "" : m_OutputHistory.back();
    }

    std::string ScriptingEngine::get_last_error() const {
      return m_LastError;
    }

    const std::vector<std::string> &ScriptingEngine::get_output_history() const {
      return m_OutputHistory;
    }

    void ScriptingEngine::clear_output() {
      m_OutputHistory.clear();
    }

    int ScriptingEngine::InternalRenderScript(const std::string &path, bool fresh, int nargs) {
      auto &engine = get_scripting_engine();
      static std::unordered_map<std::string, bool> s_InitializedFiles;

      if (fresh && !s_InitializedFiles[path]) {
        std::filesystem::path p(path);
        engine.enable_hot_reload(p.parent_path().string());
        s_InitializedFiles[path] = true;
      }

      return engine.load_file(path, nargs);
    }

    void render_lua_script(const std::string &lua_file_path) {
      ScriptingEngine::InternalRenderScript(lua_file_path, false, 0);
    }

    void render_lua_fresh_script(const std::string &lua_file_path) {
      ScriptingEngine::InternalRenderScript(lua_file_path, true, 0);
    }

    void ScriptingEngine::register_vortex_api() {
      inject_vortex_api(L);
      Cherry::Script::InjectCherryAPI(L);
    }

    void inject_vortex_api(lua_State *L) {
      lua_newtable(L);
      register_plugin_api(L);
      lua_setglobal(L, "Vortex");
    }

    std::shared_ptr<PluginInterface> get_active_plugin(lua_State *L) {
      lua_rawgetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);
      if (!lua_islightuserdata(L, -1)) {
        lua_pop(L, 1);
        return nullptr;
      }
      auto *stored = (std::shared_ptr<PluginInterface> *)lua_touserdata(L, -1);
      lua_pop(L, 1);
      return stored ? *stored : nullptr;
    }

    void set_active_plugin_context(lua_State *L, std::shared_ptr<PluginInterface> *plugin_slot) {
      lua_pushlightuserdata(L, (void *)plugin_slot);
      lua_rawsetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);
    }

    void clear_active_plugin_context(lua_State *L) {
      lua_pushnil(L);
      lua_rawsetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);
    }

  }  // namespace script
}  // namespace vxe

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

void vxe::execute_start_script() {
  auto ctx = vxe::get_current_context();
  const auto &startup_file = ctx->startup_script;

  if (startup_file.empty())
    return;

  lua_State *L = vxe::script::get_scripting_engine().get_state();

  if (luaL_loadfile(L, startup_file.c_str()) != LUA_OK) {
    const char *error = lua_tostring(L, -1);
    VXWARN("Invalid startup script : {}", error);
    lua_pop(L, 1);
    return;
  }

  lua_pop(L, 1);
  vxe::script::get_scripting_engine().InternalRenderScript(startup_file, true, 0);
}