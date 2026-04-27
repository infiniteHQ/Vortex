//
//  plugin.cpp
//  Sources of the scripting API for plugins (in LUA)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex.h>

#include <plugins/interface.hpp>
#include <vortex/scripting/scripting.hpp>

namespace vxe {
  namespace script {

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
      std::string icon = (lua_gettop(L) >= 5 && lua_isstring(L, 5)) ? vxlua_getstring(L, 5) : "";

      auto plugin = GetActivePlugin(L);
      if (!plugin) {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        return luaL_error(L, "VortexP called outside of plugin context");
      }

      auto handler = std::make_shared<LuaItemHandler>(ref, L, plugin);

      plugin->add_lua_handler(handler);

      std::weak_ptr<LuaItemHandler> weak_handler = handler;

      plugin->add_content_browser_item_handler(ItemHandlerInterface(
          type,
          [weak_handler](const std::string &path) {
            if (auto h = weak_handler.lock()) {
              h->Call(path);
            }
          },
          label,
          description,
          icon));

      return 0;
    }
    VXLUA_FUNC(PluginLog) {
      std::string log = vxlua_getstring(L, 1);
      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return 0;
      VXINFO(plugin->name_, log);
      return 0;
    }

    VXLUA_FUNC(PluginLogWarn) {
      std::string log = vxlua_getstring(L, 1);
      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return 0;
      VXWARN(plugin->name_, log);
      return 0;
    }

    VXLUA_FUNC(PluginLogError) {
      std::string log = vxlua_getstring(L, 1);
      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return 0;
      VXERROR(plugin->name_, log);
      return 0;
    }

    VXLUA_FUNC(PluginLogFatal) {
      std::string log = vxlua_getstring(L, 1);
      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return 0;
      VXFATAL(plugin->name_, log);
      return 0;
    }
    VXLUA_FUNC(PluginSetCreditsFile) {
      std::string path = vxlua_getstring(L, 1);
      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return 0;
      plugin->set_credits_file(path);
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

      std::string final_path = plugin->path();
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

      std::string final_path = plugin->cook_path(path);
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

      plugin->add_logo(path);

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

      plugin->set_credits_file(path);

      return 0;
    }

    VXLUA_FUNC(PluginCallOutputEvent) {
      std::string event_name = vxlua_getstring(L, 1);

      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return luaL_error(L, "call_output_event called outside of plugin context");

      // Build args
      ArgumentValues args;
      if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
        if (lua_isstring(L, 2)) {
          args.set(lua_tostring(L, 2));
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
          args.set(j.dump());
        }
      }

      ReturnValues ret;

      vxe::call_output_event(event_name, args, ret, plugin->name_);

      std::string ret_val = ret.get();
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

    VXLUA_FUNC(PluginAddOutputEvent) {
      std::string event_name = vxlua_getstring(L, 1);

      if (!lua_isfunction(L, 2))
        return luaL_error(L, "Argument 2 must be a function");

      lua_pushvalue(L, 2);
      int ref = luaL_ref(L, LUA_REGISTRYINDEX);
      if (ref == LUA_NOREF || ref == LUA_REFNIL)
        return luaL_error(L, "Failed to store callback reference");

      auto plugin = GetActivePlugin(L);
      if (!plugin) {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        return luaL_error(L, "add_output_event called outside of plugin context");
      }

      auto handler = std::make_shared<LuaItemHandler>(ref, L, plugin);
      plugin->add_lua_handler(handler);
      std::weak_ptr<LuaItemHandler> weak_handler = handler;

      // Helper: push args table onto Lua stack
      auto push_args_table = [](lua_State *L, ArgumentValues &args) {
        std::string args_str = args.get();
        try {
          nlohmann::json j = nlohmann::json::parse(args_str);
          if (j.is_object()) {
            lua_newtable(L);
            for (auto &[key, val] : j.items()) {
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
            return;
          }
        } catch (...) {
        }
        lua_pushstring(L, args_str.c_str());
      };

      // Helper: read return value from Lua stack and fill ret
      auto read_ret = [](lua_State *L, ReturnValues &ret) {
        if (lua_isnil(L, -1)) {
          // no return
        } else if (lua_isstring(L, -1)) {
          ret.set(lua_tostring(L, -1));
        } else if (lua_istable(L, -1)) {
          nlohmann::json result;
          lua_pushnil(L);
          while (lua_next(L, -2)) {
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
              result[key] = lua_tostring(L, -1);
            else if (lua_isinteger(L, -1))
              result[key] = lua_tointeger(L, -1);
            else if (lua_isnumber(L, -1))
              result[key] = lua_tonumber(L, -1);
            else if (lua_isboolean(L, -1))
              result[key] = (bool)lua_toboolean(L, -1);
            else
              result[key] = nullptr;
            lua_pop(L, 1);
          }
          ret.set_json(result);
        }
        lua_pop(L, 1);
      };

      std::string mode = "both";
      if (lua_gettop(L) >= 3 && lua_isstring(L, 3)) {
        mode = lua_tostring(L, 3);
      }
      if (mode == "none") {
        plugin->add_output_event(
            [weak_handler, L]() {
              auto h = weak_handler.lock();
              if (!h)
                return;
              // Set active plugin context
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                VXERROR("LuaOutputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
              }
              // Clear active plugin context
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);

      } else if (mode == "args") {
        plugin->add_output_event(
            [weak_handler, L, push_args_table](ArgumentValues &args) {
              auto h = weak_handler.lock();
              if (!h)
                return;
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              push_args_table(L, args);
              if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                VXERROR("LuaOutputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
              }
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);

      } else if (mode == "ret") {
        plugin->add_output_event(
            [weak_handler, L, read_ret](ReturnValues &ret) {
              auto h = weak_handler.lock();
              if (!h)
                return;
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
                VXERROR("LuaOutputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
                lua_pushnil(L);
                lua_rawset(L, LUA_REGISTRYINDEX);
                return;
              }
              read_ret(L, ret);
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);

      } else {  // "both" (default)
        plugin->add_output_event(
            [weak_handler, L, push_args_table, read_ret](ArgumentValues &args, ReturnValues &ret) {
              auto h = weak_handler.lock();
              if (!h)
                return;
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              push_args_table(L, args);
              if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
                VXERROR("LuaOutputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
                lua_pushnil(L);
                lua_rawset(L, LUA_REGISTRYINDEX);
                return;
              }
              read_ret(L, ret);
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);
      }

      return 0;
    }

    VXLUA_FUNC(PluginCallInputEvent) {
      std::string module_name = vxlua_getstring(L, 1);
      std::string event_name = vxlua_getstring(L, 2);

      auto plugin = GetActivePlugin(L);
      if (!plugin)
        return luaL_error(L, "call_input_event called outside of plugin context");

      // Build args
      ArgumentValues args;
      if (lua_gettop(L) >= 3 && !lua_isnil(L, 3)) {
        if (lua_isstring(L, 3)) {
          args.set(lua_tostring(L, 3));
        } else if (lua_istable(L, 3)) {
          nlohmann::json j;
          lua_pushnil(L);
          while (lua_next(L, 3)) {
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
          args.set(j.dump());
        }
      }

      ReturnValues ret;
      vxe::call_input_event(module_name, event_name, args, ret, plugin->name_);

      std::string ret_val = ret.get();
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

    VXLUA_FUNC(PluginAddInputEvent) {
      std::string event_name = vxlua_getstring(L, 1);

      if (!lua_isfunction(L, 2))
        return luaL_error(L, "Argument 2 must be a function");

      lua_pushvalue(L, 2);
      int ref = luaL_ref(L, LUA_REGISTRYINDEX);
      if (ref == LUA_NOREF || ref == LUA_REFNIL)
        return luaL_error(L, "Failed to store callback reference");

      auto plugin = GetActivePlugin(L);
      if (!plugin) {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        return luaL_error(L, "add_input_event called outside of plugin context");
      }

      auto handler = std::make_shared<LuaItemHandler>(ref, L, plugin);
      plugin->add_lua_handler(handler);
      std::weak_ptr<LuaItemHandler> weak_handler = handler;

      // Helper: push args table onto Lua stack
      auto push_args_table = [](lua_State *L, ArgumentValues &args) {
        std::string args_str = args.get();
        try {
          nlohmann::json j = nlohmann::json::parse(args_str);
          if (j.is_object()) {
            lua_newtable(L);
            for (auto &[key, val] : j.items()) {
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
            return;
          }
        } catch (...) {
        }
        lua_pushstring(L, args_str.c_str());
      };

      // Helper: read return value from Lua stack and fill ret
      auto read_ret = [](lua_State *L, ReturnValues &ret) {
        if (lua_isnil(L, -1)) {
          // no return
        } else if (lua_isstring(L, -1)) {
          ret.set(lua_tostring(L, -1));
        } else if (lua_istable(L, -1)) {
          nlohmann::json result;
          lua_pushnil(L);
          while (lua_next(L, -2)) {
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
              result[key] = lua_tostring(L, -1);
            else if (lua_isinteger(L, -1))
              result[key] = lua_tointeger(L, -1);
            else if (lua_isnumber(L, -1))
              result[key] = lua_tonumber(L, -1);
            else if (lua_isboolean(L, -1))
              result[key] = (bool)lua_toboolean(L, -1);
            else
              result[key] = nullptr;
            lua_pop(L, 1);
          }
          ret.set_json(result);
        }
        lua_pop(L, 1);
      };

      std::string mode = "both";
      if (lua_gettop(L) >= 3 && lua_isstring(L, 3)) {
        mode = lua_tostring(L, 3);
      }
      if (mode == "none") {
        plugin->add_input_event(
            [weak_handler, L]() {
              auto h = weak_handler.lock();
              if (!h)
                return;
              // Set active plugin context
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                VXERROR("LuaInputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
              }
              // Clear active plugin context
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);

      } else if (mode == "args") {
        plugin->add_input_event(
            [weak_handler, L, push_args_table](ArgumentValues &args) {
              auto h = weak_handler.lock();
              if (!h)
                return;
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              push_args_table(L, args);
              if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                VXERROR("LuaInputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
              }
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);

      } else if (mode == "ret") {
        plugin->add_input_event(
            [weak_handler, L, read_ret](ReturnValues &ret) {
              auto h = weak_handler.lock();
              if (!h)
                return;
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
                VXERROR("LuaInputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
                lua_pushnil(L);
                lua_rawset(L, LUA_REGISTRYINDEX);
                return;
              }
              read_ret(L, ret);
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);

      } else {  // "both" (default)
        plugin->add_input_event(
            [weak_handler, L, push_args_table, read_ret](ArgumentValues &args, ReturnValues &ret) {
              auto h = weak_handler.lock();
              if (!h)
                return;
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushlightuserdata(L, (void *)&h->plugin);
              lua_rawset(L, LUA_REGISTRYINDEX);

              lua_rawgeti(L, LUA_REGISTRYINDEX, h->lua_ref);
              push_args_table(L, args);
              if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
                VXERROR("LuaInputEvent", lua_tostring(L, -1));
                lua_pop(L, 1);
                lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
                lua_pushnil(L);
                lua_rawset(L, LUA_REGISTRYINDEX);
                return;
              }
              read_ret(L, ret);
              lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
              lua_pushnil(L);
              lua_rawset(L, LUA_REGISTRYINDEX);
            },
            event_name);
      }

      return 0;
    }

    // TODO add_function
    // TODO execute_function (with support of args and return)

    void register_plugin_api(lua_State *L) {
      VXLUA_REGISTER_AS(L, PluginLog, "Log");
      VXLUA_REGISTER_AS(L, PluginLogWarn, "LogWarn");
      VXLUA_REGISTER_AS(L, PluginLogError, "LogError");
      VXLUA_REGISTER_AS(L, PluginLogFatal, "LogFatal");

      VXLUA_REGISTER_AS(L, PluginSetCreditsFiles, "SetCreditsFiles");

      VXLUA_REGISTER_AS(L, GetPluginPath, "GetPath");
      VXLUA_REGISTER_AS(L, GetCookPath, "CookPath");
      VXLUA_REGISTER_AS(L, PluginSetCreditsFile, "SetCreditsFile");
      VXLUA_REGISTER_AS(L, PluginAddContentBrowserItemHandler, "AddContentBrowserItemHandler");
      VXLUA_REGISTER_AS(L, PluginAddLogo, "AddLogo");

      VXLUA_REGISTER_AS(L, PluginAddOutputEvent, "AddOutputEvent");
      VXLUA_REGISTER_AS(L, PluginCallOutputEvent, "CallOutputEvent");

      VXLUA_REGISTER_AS(L, PluginAddInputEvent, "AddInputEvent");
      VXLUA_REGISTER_AS(L, PluginCallInputEvent, "CallInputEvent");
    }

  }  // namespace script
}  // namespace vxe