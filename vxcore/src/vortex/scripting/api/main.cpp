#include <plugins/interface.h>
#include <vortex.h>
#include <vortex/scripting/scripting.hpp>

namespace VortexMaker {
namespace Script {

VXLUA_FUNC(Log) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("Vortex", log);
  return 0;
}

VXLUA_FUNC(LogWarn) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("Vortex", log);
  return 0;
}

VXLUA_FUNC(LogError) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("Vortex", log);
  return 0;
}

VXLUA_FUNC(LogFatal) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("Vortex", log);
  return 0;
}

VXLUA_FUNC(CallOutputEvent) {
  std::string eventName = vxlua_getstring(L, 1);

  if (!lua_istable(L, 2))
    return vxlua_error_msg(L, "ArgumentValues must be a table");

  std::string origin = (lua_gettop(L) >= 3) ? vxlua_getstring(L, 3) : "unknow";

  nlohmann::json j = LuaTableToJson(L, 2);
  ArgumentValues args(j.dump());
  ReturnValues ret;

  VortexMaker::CallOutputEvent(eventName, args, ret, origin);

  if (ret.GetValue() != "null") {
    JsonToLuaTable(L, ret.GetJsonValue());
    return 1;
  }

  return 0;
}

VXLUA_FUNC(CallInputEvent) {
  std::string moduleName = vxlua_getstring(L, 1);
  std::string eventName = vxlua_getstring(L, 2);

  if (!lua_istable(L, 3))
    return vxlua_error_msg(L, "ArgumentValues must be a table");

  std::string origin = (lua_gettop(L) >= 4) ? vxlua_getstring(L, 4) : "unknow";

  nlohmann::json j = LuaTableToJson(L, 3);
  ArgumentValues args(j.dump());
  ReturnValues ret;

  VortexMaker::CallInputEvent(moduleName, eventName, args, ret, origin);

  JsonToLuaTable(L, ret.GetJsonValue());
  return 1;
}

VXLUA_FUNC(IsLinux) {
  bool res = VortexMaker::IsLinux();

  lua_pushboolean(L, res);
  return 1;
}

VXLUA_FUNC(IsNotLinux) {
  bool res = VortexMaker::IsNotLinux();

  lua_pushboolean(L, res);
  return 1;
}

VXLUA_FUNC(IsWindows) {
  bool res = VortexMaker::IsWindows();

  lua_pushboolean(L, res);
  return 1;
}

VXLUA_FUNC(IsNotWindows) {
  bool res = VortexMaker::IsNotWindows();

  lua_pushboolean(L, res);
  return 1;
}

VXLUA_FUNC(IsMacOS) {
  bool res = VortexMaker::IsMacOs();

  lua_pushboolean(L, res);
  return 1;
}

VXLUA_FUNC(IsNotMacOS) {
  bool res = VortexMaker::IsNotMacOS();

  lua_pushboolean(L, res);
  return 1;
}

VXLUA_FUNC(GetBuildDate) {
  auto res = VortexMaker::GetBuildDate();

  lua_pushstring(L, res.c_str());
  return 1;
}

VXLUA_FUNC(GetGitCommit) {
  auto res = VortexMaker::GetGitCommit();

  lua_pushstring(L, res.c_str());
  return 1;
}

VXLUA_FUNC(GetVortexEditorDist) {
  auto res = VortexMaker::GetVortexEditorDist();

  lua_pushstring(L, res.c_str());
  return 1;
}

VXLUA_FUNC(GetVortexBuildID) {
  auto res = VortexMaker::GetVortexBuildID();

  lua_pushstring(L, res.c_str());
  return 1;
}

VXLUA_FUNC(GetVortexBuildName) {
  auto res = VortexMaker::GetVortexBuildName();

  lua_pushstring(L, res.c_str());
  return 1;
}

VXLUA_FUNC(GetVortexEditorHash) {
  auto res = VortexMaker::GetVortexEditorHash();

  lua_pushstring(L, res.c_str());
  return 1;
}

void RegisterMainAPI(lua_State *L) {
  // Logs & Debug
  VXLUA_REGISTER(L, -1, Log);
  VXLUA_REGISTER(L, -1, LogWarn);
  VXLUA_REGISTER(L, -1, LogError);
  VXLUA_REGISTER(L, -1, LogFatal);

  // Modules/Plugins events
  VXLUA_REGISTER(L, -1, CallOutputEvent);
  VXLUA_REGISTER(L, -1, CallInputEvent);

  // Cross platform functions
  VXLUA_REGISTER(L, -1, IsLinux);
  VXLUA_REGISTER(L, -1, IsNotLinux);
  VXLUA_REGISTER(L, -1, IsWindows);
  VXLUA_REGISTER(L, -1, IsNotWindows);
  VXLUA_REGISTER(L, -1, IsMacOS);
  VXLUA_REGISTER(L, -1, IsNotMacOS);

  // Information functions
  VXLUA_REGISTER(L, -1, GetBuildDate);
  VXLUA_REGISTER(L, -1, GetGitCommit);
  VXLUA_REGISTER(L, -1, GetVortexEditorDist);
  VXLUA_REGISTER(L, -1, GetVortexBuildID);
  VXLUA_REGISTER(L, -1, GetVortexBuildName);
  VXLUA_REGISTER(L, -1, GetVortexEditorHash);
}

} // namespace Script
} // namespace VortexMaker