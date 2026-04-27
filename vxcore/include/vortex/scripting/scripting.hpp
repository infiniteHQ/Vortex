#pragma once
#include <plugins/interface.h>
#include <vortex.h>

#include "lua_helpers.hpp"

#ifndef VORTEX_LUA_SCRIPTING_HPP
#define VORTEX_LUA_SCRIPTING_HPP

#include <functional>
#include <string>
#include <vector>

namespace vxe {
  namespace script {

    class ScriptingEngine {
     public:
      ScriptingEngine();
      ~ScriptingEngine();

      bool execute(const std::string &code);
      int load_file(const std::string &path, int nargs = 0);
      void reload_file(const std::string &path);

      static int InternalRenderScript(const std::string &path, bool fresh, int nargs);

      std::string get_last_output() const;
      std::string get_last_error() const;
      const std::vector<std::string> &get_output_history() const;
      void clear_output();

      void enable_hot_reload(const std::string &scriptDir);
      void check_for_changes();

      void register_vortex_api();

      lua_State *get_state() {
        return L;
      }

      void load_file_for_plugin(const std::string &path, const std::shared_ptr<PluginInterface> &plugin) {
        auto *stored = new std::shared_ptr<PluginInterface>(plugin);
        lua_pushlightuserdata(L, (void *)stored);
        lua_rawsetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);

        bool wasHotReload = m_HotReloadEnabled;
        m_HotReloadEnabled = false;
        load_file(path);
        m_HotReloadEnabled = wasHotReload;

        lua_pushnil(L);
        lua_rawsetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);
        delete stored;
      }

     private:
      lua_State *L;
      std::string m_LastError;
      std::vector<std::string> m_OutputHistory;

      bool m_HotReloadEnabled = false;
      std::string m_ScriptDir;
      std::unordered_map<std::string, long long> m_FileTimestamps;

      void capture_output(const std::string &text);
      static int lua_print(lua_State *L);  // Override print()
    };

    VORTEX_API ScriptingEngine &get_scripting_engine();

    void register_main_api(lua_State *L);
    void register_plugin_api(lua_State *L);
    void render_lua_script(const std::string &lua_file_path);
    void render_lua_fresh_script(const std::string &lua_file_path);

  }  // namespace script
}  // namespace vxe

namespace vxe {
  VORTEX_API void execute_start_script();
}

#endif  // VORTEX_LUA_SCRIPTING_HPP