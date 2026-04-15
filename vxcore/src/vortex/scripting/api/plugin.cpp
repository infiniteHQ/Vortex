#include <plugins/interface.h>
#include <vortex.h>
#include <vortex/scripting/scripting.hpp>

namespace VortexMaker {
namespace Script {

static std::shared_ptr<PluginInterface> GetActivePlugin(lua_State *L) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);

  if (!lua_islightuserdata(L, -1)) {
    lua_pop(L, 1);
    return nullptr;
  }

  auto *stored = (std::shared_ptr<PluginInterface> *)lua_touserdata(L, -1);

  lua_pop(L, 1);

  if (!stored)
    return nullptr;

  return *stored;
}

VXLUA_FUNC(PluginAddContentBrowserItemHandler) {
  if (!lua_isfunction(L, 2))
    return luaL_error(L, "Argument 2 must be a function");

  lua_pushvalue(L, 2);
  int ref = luaL_ref(L, LUA_REGISTRYINDEX);

  if (ref == LUA_NOREF || ref == LUA_REFNIL)
    return luaL_error(L, "Failed to store callback reference");

  std::string type = vxlua_getstring(L, 1);
  std::string label = vxlua_getstring(L, 3);
  std::string description = vxlua_getstring(L, 4);
  std::string icon =
      (lua_gettop(L) >= 5 && lua_isstring(L, 5)) ? vxlua_getstring(L, 5) : "";

  auto plugin = GetActivePlugin(L);
  if (!plugin) {
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    return luaL_error(L, "VortexP called outside of plugin context");
  }

  auto handler = std::make_shared<LuaItemHandler>(ref, L, plugin);

  plugin->AddLuaHandler(handler);

  std::weak_ptr<LuaItemHandler> weak_handler = handler;

  plugin->AddContentBrowserItemHandler(ItemHandlerInterface(
      type,
      [weak_handler](const std::string &path) {
        if (auto h = weak_handler.lock()) {
          h->Call(path);
        }
      },
      label, description, icon));

  return 0;
}
VXLUA_FUNC(PluginLog) {
  std::string log = vxlua_getstring(L, 1);
  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;
  VXINFO(plugin->m_name, log);
  return 0;
}

VXLUA_FUNC(PluginLogWarn) {
  std::string log = vxlua_getstring(L, 1);
  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;
  VXWARN(plugin->m_name, log);
  return 0;
}

VXLUA_FUNC(PluginLogError) {
  std::string log = vxlua_getstring(L, 1);
  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;
  VXERROR(plugin->m_name, log);
  return 0;
}

VXLUA_FUNC(PluginLogFatal) {
  std::string log = vxlua_getstring(L, 1);
  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;
  VXFATAL(plugin->m_name, log);
  return 0;
}
VXLUA_FUNC(PluginSetCreditsFile) {
  std::string path = vxlua_getstring(L, 1);
  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;
  plugin->SetCreditsFile(path);
  return 0;
}

VXLUA_FUNC(GetPluginPath) {
  std::string path = "";
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    path = vxlua_getstring(L, 1);
  }

  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;

  std::string final_path = plugin->GetPath();
  if (!path.empty()) {
    final_path += "/" + path;
  }

  lua_pushstring(L, final_path.c_str());
  return 1;
}

VXLUA_FUNC(GetCookPath) {
  std::string path = "";
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    path = vxlua_getstring(L, 1);
  }

  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;

  std::string final_path = plugin->CookPath(path);
  if (!path.empty()) {
    final_path += "/" + path;
  }

  lua_pushstring(L, final_path.c_str());
  return 1;
}

VXLUA_FUNC(PluginAddLogo) {
  std::string path = "";
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    path = vxlua_getstring(L, 1);
  }

  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;

  plugin->AddLogo(path);

  return 0;
}

VXLUA_FUNC(PluginSetCreditsFiles) {
  std::string path = "";
  if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
    path = vxlua_getstring(L, 1);
  }

  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return 0;

  plugin->SetCreditsFile(path);

  return 0;
}

VXLUA_FUNC(PluginCallOutputEvent) {
  std::string event_name = vxlua_getstring(L, 1);

  auto plugin = GetActivePlugin(L);
  if (!plugin)
    return luaL_error(L, "CallOutputEvent called outside of plugin context");

  // Build args
  ArgumentValues args;
  if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
    if (lua_isstring(L, 2)) {
      args.SetValue(lua_tostring(L, 2));
    } else if (lua_istable(L, 2)) {
      nlohmann::json j;
      lua_pushnil(L);
      while (lua_next(L, 2)) {
        std::string key;
        if (lua_isstring(L, -2))
          key = lua_tostring(L, -2);
        else if (lua_isinteger(L, -2))
          key = std::to_string(lua_tointeger(L, -2));
        else {
          lua_pop(L, 1);
          continue;
        }

        if (lua_isstring(L, -1))
          j[key] = lua_tostring(L, -1);
        else if (lua_isinteger(L, -1))
          j[key] = lua_tointeger(L, -1);
        else if (lua_isnumber(L, -1))
          j[key] = lua_tonumber(L, -1);
        else if (lua_isboolean(L, -1))
          j[key] = (bool)lua_toboolean(L, -1);
        else
          j[key] = nullptr;
        lua_pop(L, 1);
      }
      args.SetValue(j.dump());
    }
  }

  ReturnValues ret;

  VortexMaker::CallOutputEvent(event_name, args, ret, plugin->m_name);

  std::string ret_val = ret.GetValue();
  if (ret_val.empty() || ret_val == "null") {
    lua_pushnil(L);
    return 1;
  }

  try {
    nlohmann::json result = nlohmann::json::parse(ret_val);
    if (result.is_object()) {
      lua_newtable(L);
      for (auto &[key, val] : result.items()) {
        lua_pushstring(L, key.c_str());
        if (val.is_string())
          lua_pushstring(L, val.get<std::string>().c_str());
        else if (val.is_number_integer())
          lua_pushinteger(L, val.get<int64_t>());
        else if (val.is_number())
          lua_pushnumber(L, val.get<double>());
        else if (val.is_boolean())
          lua_pushboolean(L, val.get<bool>());
        else
          lua_pushstring(L, val.dump().c_str());
        lua_settable(L, -3);
      }
      return 1;
    }
  } catch (...) {
  }

  lua_pushstring(L, ret_val.c_str());
  return 1;
}
// TODO AddFunction
// TODO ExecuteFunction (with support of args and return)

void RegisterPluginAPI(lua_State *L) {
  VXLUA_REGISTER_AS(L, PluginLog, "Log");
  VXLUA_REGISTER_AS(L, PluginLogWarn, "LogWarn");
  VXLUA_REGISTER_AS(L, PluginLogError, "LogError");
  VXLUA_REGISTER_AS(L, PluginLogFatal, "LogFatal");

  VXLUA_REGISTER_AS(L, PluginSetCreditsFiles, "SetCreditsFiles");

  VXLUA_REGISTER_AS(L, GetPluginPath, "GetPath");
  VXLUA_REGISTER_AS(L, GetCookPath, "CookPath");
  VXLUA_REGISTER_AS(L, PluginSetCreditsFile, "SetCreditsFile");
  VXLUA_REGISTER_AS(L, PluginAddContentBrowserItemHandler,
                    "AddContentBrowserItemHandler");
  VXLUA_REGISTER_AS(L, PluginAddLogo, "AddLogo");

  VXLUA_REGISTER_AS(L, PluginCallOutputEvent, "CallOutputEvent");
}

} // namespace Script
} // namespace VortexMaker