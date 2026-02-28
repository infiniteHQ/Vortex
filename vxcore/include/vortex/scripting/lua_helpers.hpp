#pragma once
#include <string>

#include "../../../../lib/cherry/lib/lua/src/lua.hpp"
#include "../../../../lib/json/single_include/nlohmann/json.hpp"

#define VXLUA_FUNC(name) static int l_##name(lua_State *L)

#define VXLUA_REGISTER(L, table, name)                                         \
  lua_pushcfunction(L, l_##name);                                              \
  lua_setfield(L, -2, #name)

#define VXLUA_REGISTER_GLOBAL_FUNC(global, name)                               \
  do {                                                                         \
    lua_State *L = Cherry::Script::GetScriptingEngine().GetState();            \
                                                                               \
    lua_getglobal(L, global);                                                  \
    if (lua_isnil(L, -1)) {                                                    \
      lua_pop(L, 1);                                                           \
      lua_newtable(L);                                                         \
      lua_setglobal(L, global);                                                \
      lua_getglobal(L, global);                                                \
    }                                                                          \
                                                                               \
    lua_pushcfunction(L, l_##name);                                            \
    lua_setfield(L, -2, #name);                                                \
                                                                               \
    lua_pop(L, 1);                                                             \
  } while (0)

namespace VortexMaker {

inline std::string vxlua_getstring(lua_State *L, int idx) {
  return luaL_checkstring(L, idx);
}

inline float vxlua_getfloat(lua_State *L, int idx) {
  return static_cast<float>(luaL_checknumber(L, idx));
}

inline int vxlua_getint(lua_State *L, int idx) {
  return static_cast<int>(luaL_checkinteger(L, idx));
}

inline bool vxlua_getbool(lua_State *L, int idx) {
  return lua_toboolean(L, idx) != 0;
}

inline int vxlua_error_msg(lua_State *L, const char *msg) {
  return luaL_error(L, "%s", msg);
}

inline void vxlua_push(lua_State *L, float val) { lua_pushnumber(L, val); }
inline void vxlua_push(lua_State *L, int val) { lua_pushinteger(L, val); }
inline void vxlua_push(lua_State *L, const std::string &val) {
  lua_pushstring(L, val.c_str());
}
inline void vxlua_push(lua_State *L, bool val) { lua_pushboolean(L, val); }

static nlohmann::json LuaTableToJson(lua_State *L, int index) {
  nlohmann::json j = nlohmann::json::object();

  lua_pushnil(L);
  while (lua_next(L, index)) {
    std::string key = luaL_checkstring(L, -2);

    if (lua_isstring(L, -1))
      j[key] = std::string(lua_tostring(L, -1));
    else if (lua_isnumber(L, -1))
      j[key] = lua_tonumber(L, -1);
    else if (lua_isboolean(L, -1))
      j[key] = lua_toboolean(L, -1);
    else
      j[key] = nullptr;

    lua_pop(L, 1);
  }

  return j;
}
static void JsonToLuaTable(lua_State *L, const nlohmann::json &j) {
  lua_newtable(L);

  for (auto &[key, value] : j.items()) {
    lua_pushstring(L, key.c_str());

    if (value.is_string())
      lua_pushstring(L, value.get<std::string>().c_str());
    else if (value.is_number())
      lua_pushnumber(L, value.get<double>());
    else if (value.is_boolean())
      lua_pushboolean(L, value.get<bool>());
    else
      lua_pushnil(L);

    lua_settable(L, -3);
  }
}
} // namespace VortexMaker