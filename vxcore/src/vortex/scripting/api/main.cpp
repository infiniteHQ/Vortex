//
//  main.cpp
//  Sources of the main vortex API for scripting (in LUA)
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <plugins/interface.h>
#include <vortex.h>

#include <vortex/scripting/scripting.hpp>

namespace vxe {
  namespace script {

    VXLUA_FUNC(Log) {
      std::string log = vxlua_getstring(L, 1);

      VXINFO("Vortex", log);
      return 0;
    }

    VXLUA_FUNC(log_warn) {
      std::string log = vxlua_getstring(L, 1);

      VXINFO("Vortex", log);
      return 0;
    }

    VXLUA_FUNC(log_error) {
      std::string log = vxlua_getstring(L, 1);

      VXINFO("Vortex", log);
      return 0;
    }

    VXLUA_FUNC(log_fatal) {
      std::string log = vxlua_getstring(L, 1);

      VXINFO("Vortex", log);
      return 0;
    }

    VXLUA_FUNC(call_output_event) {
      std::string eventName = vxlua_getstring(L, 1);

      if (!lua_istable(L, 2))
        return vxlua_error_msg(L, "ArgumentValues must be a table");

      std::string origin = (lua_gettop(L) >= 3) ? vxlua_getstring(L, 3) : "unknow";

      nlohmann::json j = LuaTableToJson(L, 2);
      ArgumentValues args(j.dump());
      ReturnValues ret;

      vxe::call_output_event(eventName, args, ret, origin);

      if (ret.get() != "null") {
        JsonToLuaTable(L, ret.get_json());
        return 1;
      }

      return 0;
    }

    VXLUA_FUNC(call_input_event) {
      std::string moduleName = vxlua_getstring(L, 1);
      std::string eventName = vxlua_getstring(L, 2);

      if (!lua_istable(L, 3))
        return vxlua_error_msg(L, "ArgumentValues must be a table");

      std::string origin = (lua_gettop(L) >= 4) ? vxlua_getstring(L, 4) : "unknow";

      nlohmann::json j = LuaTableToJson(L, 3);
      ArgumentValues args(j.dump());
      ReturnValues ret;

      vxe::call_input_event(moduleName, eventName, args, ret, origin);

      JsonToLuaTable(L, ret.get_json());
      return 1;
    }

    VXLUA_FUNC(is_linux) {
      bool res = vxe::is_linux();

      lua_pushboolean(L, res);
      return 1;
    }

    VXLUA_FUNC(is_not_linux) {
      bool res = vxe::is_not_linux();

      lua_pushboolean(L, res);
      return 1;
    }

    VXLUA_FUNC(is_windows) {
      bool res = vxe::is_windows();

      lua_pushboolean(L, res);
      return 1;
    }

    VXLUA_FUNC(is_not_windows) {
      bool res = vxe::is_not_windows();

      lua_pushboolean(L, res);
      return 1;
    }

    VXLUA_FUNC(IsMacOS) {
      bool res = vxe::is_macos();

      lua_pushboolean(L, res);
      return 1;
    }

    VXLUA_FUNC(is_not_macos) {
      bool res = vxe::is_not_macos();

      lua_pushboolean(L, res);
      return 1;
    }

    VXLUA_FUNC(GetBuildDate) {
      auto res = vxe::GetBuildDate();

      lua_pushstring(L, res.c_str());
      return 1;
    }

    VXLUA_FUNC(GetGitCommit) {
      auto res = vxe::GetGitCommit();

      lua_pushstring(L, res.c_str());
      return 1;
    }

    VXLUA_FUNC(GetVortexEditorDist) {
      auto res = vxe::GetVortexEditorDist();

      lua_pushstring(L, res.c_str());
      return 1;
    }

    VXLUA_FUNC(GetVortexBuildID) {
      auto res = vxe::GetVortexBuildID();

      lua_pushstring(L, res.c_str());
      return 1;
    }

    VXLUA_FUNC(GetVortexBuildName) {
      auto res = vxe::GetVortexBuildName();

      lua_pushstring(L, res.c_str());
      return 1;
    }

    VXLUA_FUNC(GetVortexEditorHash) {
      auto res = vxe::GetVortexEditorHash();

      lua_pushstring(L, res.c_str());
      return 1;
    }

    void register_main_api(lua_State *L) {
      // Logs & Debug
      VXLUA_REGISTER(L, -1, Log);
      VXLUA_REGISTER(L, -1, log_warn);
      VXLUA_REGISTER(L, -1, log_error);
      VXLUA_REGISTER(L, -1, log_fatal);

      // Modules/Plugins events
      VXLUA_REGISTER(L, -1, call_output_event);
      VXLUA_REGISTER(L, -1, call_input_event);

      // Cross platform functions
      VXLUA_REGISTER(L, -1, is_linux);
      VXLUA_REGISTER(L, -1, is_not_linux);
      VXLUA_REGISTER(L, -1, is_windows);
      VXLUA_REGISTER(L, -1, is_not_windows);
      VXLUA_REGISTER(L, -1, IsMacOS);
      VXLUA_REGISTER(L, -1, is_not_macos);

      // Information functions
      VXLUA_REGISTER(L, -1, GetBuildDate);
      VXLUA_REGISTER(L, -1, GetGitCommit);
      VXLUA_REGISTER(L, -1, GetVortexEditorDist);
      VXLUA_REGISTER(L, -1, GetVortexBuildID);
      VXLUA_REGISTER(L, -1, GetVortexBuildName);
      VXLUA_REGISTER(L, -1, GetVortexEditorHash);
    }

  }  // namespace script
}  // namespace vxe