#include <vortex.h>
#include <vortex/scripting/scripting.hpp>

namespace VortexMaker {
namespace Script {

VXLUA_FUNC(Log) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("test", log);
  return 0;
}

VXLUA_FUNC(LogWarn) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("test", log);
  return 0;
}

VXLUA_FUNC(LogError) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("test", log);
  return 0;
}

VXLUA_FUNC(LogFatal) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("test", log);
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

void RegisterLogicAPI(lua_State *L) {
  // Logs & Debug
  VXLUA_REGISTER(L, -1, Log);
  VXLUA_REGISTER(L, -1, LogWarn);
  VXLUA_REGISTER(L, -1, LogError);
  VXLUA_REGISTER(L, -1, LogFatal);
}
} // namespace Script
} // namespace VortexMaker