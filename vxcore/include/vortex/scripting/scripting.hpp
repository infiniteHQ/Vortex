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
  namespace Script {

    class ScriptingEngine {
     public:
      ScriptingEngine();
      ~ScriptingEngine();

      bool Execute(const std::string &code);
      int LoadFile(const std::string &path, int nargs = 0);
      void ReloadFile(const std::string &path);

      static int InternalRenderScript(const std::string &path, bool fresh, int nargs);

      std::string GetLastOutput() const;
      std::string GetLastError() const;
      const std::vector<std::string> &GetOutputHistory() const;
      void ClearOutput();

      void EnableHotReload(const std::string &scriptDir);
      void CheckForChanges();

      void RegisterVortexAPI();

      lua_State *GetState() {
        return L;
      }

      void LoadFileForPlugin(const std::string &path, const std::shared_ptr<PluginInterface> &plugin) {
        auto *stored = new std::shared_ptr<PluginInterface>(plugin);
        lua_pushlightuserdata(L, (void *)stored);
        lua_rawsetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);

        bool wasHotReload = m_HotReloadEnabled;
        m_HotReloadEnabled = false;
        LoadFile(path);
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

      void CaptureOutput(const std::string &text);
      static int LuaPrint(lua_State *L);  // Override print()
    };

    VORTEX_API ScriptingEngine &GetScriptingEngine();

    void RegisterMainAPI(lua_State *L);
    void RegisterPluginAPI(lua_State *L);
    void RenderLuaScript(const std::string &lua_file_path);
    void RenderLuaFreshScript(const std::string &lua_file_path);

  }  // namespace Script
}  // namespace vxe

namespace vxe {
  VORTEX_API void ExecuteStartScript();
}

#endif  // VORTEX_LUA_SCRIPTING_HPP