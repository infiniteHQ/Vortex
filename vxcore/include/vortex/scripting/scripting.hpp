#pragma once
#include "lua_helpers.hpp"
#include <vortex.h>

#ifndef VORTEX_LUA_SCRIPTING_HPP
#define VORTEX_LUA_SCRIPTING_HPP

#include <functional>
#include <string>
#include <vector>

namespace VortexMaker {
namespace Script {

class ScriptingEngine {
public:
  ScriptingEngine();
  ~ScriptingEngine();

  bool Execute(const std::string &code);
  int LoadFile(const std::string &path, int nargs = 0);
  void ReloadFile(const std::string &path);

  static int InternalRenderScript(const std::string &path, bool fresh,
                                  int nargs);

  std::string GetLastOutput() const;
  std::string GetLastError() const;
  const std::vector<std::string> &GetOutputHistory() const;
  void ClearOutput();

  void EnableHotReload(const std::string &scriptDir);
  void CheckForChanges();

  void RegisterVortexAPI();

  lua_State *GetState() { return L; }

private:
  lua_State *L;
  std::string m_LastError;
  std::vector<std::string> m_OutputHistory;

  bool m_HotReloadEnabled = false;
  std::string m_ScriptDir;
  std::unordered_map<std::string, long long> m_FileTimestamps;

  void CaptureOutput(const std::string &text);
  static int LuaPrint(lua_State *L); // Override print()
};

VORTEX_API ScriptingEngine &GetScriptingEngine();

void RegisterLogicAPI(lua_State *L);
void RenderLuaScript(const std::string &lua_file_path);
void RenderLuaFreshScript(const std::string &lua_file_path);

} // namespace Script

} // namespace VortexMaker

#endif // VORTEX_LUA_SCRIPTING_HPP