#include <plugins/interface.h>
#include <vortex.h>
#include <vortex/scripting/scripting.hpp>

namespace VortexMaker {
namespace Script {

static std::shared_ptr<PluginInterface> GetActivePlugin(lua_State *L) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, (void *)&ACTIVE_PLUGIN_KEY);
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
  auto handler = std::make_shared<LuaItemHandler>(ref, L);

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
void RegisterPluginAPI(lua_State *L) {
  VXLUA_REGISTER_AS(L, PluginLog, "Log");
  VXLUA_REGISTER_AS(L, GetPluginPath, "GetPath");
  VXLUA_REGISTER_AS(L, PluginLogWarn, "LogWarn");
  VXLUA_REGISTER_AS(L, PluginSetCreditsFile, "SetCreditsFile");
  VXLUA_REGISTER_AS(L, PluginAddContentBrowserItemHandler,
                    "AddContentBrowserItemHandler");
}

} // namespace Script
} // namespace VortexMaker