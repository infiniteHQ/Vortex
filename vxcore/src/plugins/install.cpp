//
//  install.cpp
//  Sources for plugins installation features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex_internals.h>

#include <plugins/install.hpp>
#include <plugins/load.hpp>
#include <plugins/runtime.hpp>

VORTEX_API void vxe::install_plugin(const std::string &plugin_name, const std::string &version, bool &restart_plugins) {
  // Get reference to the Vortex context
  auto ctx = vxe::get_current_context();

  // Get the home directory
  std::string homeDir = vxe::get_home_directory();

  // Plugin path on the system
  std::string path = homeDir + "/.vx/plugins";

  // Search plugins registered
  auto plugin_files = vxe::search_files(path, "plugin.json");

  // Finded state
  bool finded = false;

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = vxe::dump_json(file);

      std::string plugin_path = file.substr(0, file.find_last_of("/"));

      std::string name = json_data["name"].get<std::string>();
      std::string version = json_data["version"].get<std::string>();

      if (name == plugin_name && version == version) {
        finded = true;

        // Move the plugin into the project
        {
          std::string cmd = "cp -r " + plugin_path + " " + ctx->projectPath.string() + "/.vx/plugins/";
          system(cmd.c_str());
        }

        // If restart_plugins == true, restart all plugins
        if (restart_plugins) {
          // Stop all plugins
          for (auto ep : ctx->IO.ep) {
            ep->stop();
          }

          // Clear closed plugins
          ctx->IO.ep.clear();

          // Load plugins installed in the current project
          vxe::load_editor_plugins(ctx->projectPath.string(), ctx->IO.ep_handles, ctx->IO.ep);

          // Finally, start all loaded plugins.
          vxe::start_all_plugins();
        }

        return;
      }
    } catch (std::exception e) {
      vxe::log_error("Core", e.what());
    }
  }

  if (!finded) {
    vxe::log_error(
        "Core", "Failed to find the plugin " + plugin_name + " with version " + version + ", this plugin is installed ?");
  }

  // Search plugin registered by plugin_name in env
  // Get main informations
  // Create the folder name
  // Copy distant plugin content into the plugin into the projet
  // Restart plugins
}

VORTEX_API std::shared_ptr<PluginInterface> vxe::find_plugin_in_directory(const std::string &directory) {
  // Search plugins registered
  auto plugin_files = vxe::search_files(directory, "plugin.json");

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = vxe::dump_json(file);

      std::string plugin_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<PluginInterface> plugin_interface = std::make_shared<PluginInterface>();

      plugin_interface->name_ = json_data["name"].get<std::string>();
      plugin_interface->version_ = json_data["version"].get<std::string>();

      return plugin_interface;
    } catch (std::exception e) {
      vxe::log_error("Core", e.what());
    }
  }

  vxe::log_error("Core", "Failed to find the plugin at " + directory + ", no plugins detected.");
  return nullptr;
}

VORTEX_API std::vector<std::shared_ptr<PluginInterface>> vxe::find_plugins_in_directory(const std::string &directory) {
  // Search plugins registered
  auto plugin_files = vxe::search_files(directory, "plugin.json", 3);

  std::vector<std::shared_ptr<PluginInterface>> interfaces;

  // Iterate through each found plugin file
  for (const auto &file : plugin_files) {
    try {
      // Load JSON data from the plugin configuration file
      auto json_data = vxe::dump_json(file);

      std::string plugin_path = file.substr(0, file.find_last_of("/"));

      std::shared_ptr<PluginInterface> plugin_interface = std::make_shared<PluginInterface>();
      plugin_interface->name_ = json_data["name"].get<std::string>();
      plugin_interface->auto_exec_ = json_data["auto_exec"].get<bool>();
      plugin_interface->proper_name_ = json_data["proper_name"].get<std::string>();
      plugin_interface->type_ = json_data["type"].get<std::string>();
      plugin_interface->version_ = json_data["version"].get<std::string>();
      plugin_interface->description_ = json_data["description"].get<std::string>();
      plugin_interface->picture_ = json_data["picture"].get<std::string>();
      plugin_interface->logo_path_ = plugin_path + "/" + plugin_interface->picture_;
      plugin_interface->path_ = plugin_path + "/";
      plugin_interface->author_ = json_data["author"].get<std::string>();
      plugin_interface->group_ = json_data["group"].get<std::string>();
      plugin_interface->contributors_ = json_data["contributors"].get<std::vector<std::string>>();

      for (auto dep : json_data["deps"]) {
        std::shared_ptr<PluginInterfaceDep> dependence = std::make_shared<PluginInterfaceDep>();
        dependence->name = dep["name"].get<std::string>();
        dependence->type = dep["type"].get<std::string>();
        for (auto version : dep["versions"]) {
          dependence->supported_versions.push_back(version);
        }
        plugin_interface->dependencies_.push_back(dependence);
      }

      plugin_interface->supported_versions_ = json_data["compatibility"].get<std::vector<std::string>>();

      interfaces.push_back(plugin_interface);
    } catch (std::exception e) {
      vxe::log_error("Core", e.what());
    }
  }

  return interfaces;
}

VORTEX_API void vxe::install_plugin_to_system(const std::string &path) {
  std::string plugins_path = "~/.vx/plugins";
  std::string json_file = path + "/plugin.json";

  try {
    auto json_data = vxe::dump_json(json_file);
    std::string name = json_data["name"].get<std::string>();
    std::string proper_name = json_data["proper_name"].get<std::string>();
    std::string type = json_data["type"].get<std::string>();
    std::string version = json_data["version"].get<std::string>();
    std::string description = json_data["description"].get<std::string>();
    std::string author = json_data["author"].get<std::string>();

    // std::string origin_path = path.substr(0, path.find_last_of("/"));
    plugins_path += "/" + name + "." + version;

    vxe::log_info("Core", "Installing the plugin " + name + "...");

    {
      std::string cmd = "mkdir " + plugins_path;
      system(cmd.c_str());
    }

    {
      std::string cmd = "cp -r " + path + "/* " + plugins_path;
      system(cmd.c_str());
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

VORTEX_API void vxe::add_plugin_to_project(const std::string &plugin_name) {
  // TODO
}

LuaItemHandler::LuaItemHandler(int ref, lua_State *state, std::shared_ptr<PluginInterface> p)
    : lua_ref(ref),
      L(state),
      plugin(std::move(p)) {
}

LuaItemHandler::LuaItemHandler(LuaItemHandler &&other) noexcept
    : lua_ref(other.lua_ref),
      L(other.L),
      plugin(std::move(other.plugin)) {
  other.lua_ref = LUA_NOREF;
  other.L = nullptr;
}
void LuaItemHandler::Call(const std::string &path) {
  if (!L)
    return;

  lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
  lua_pushlightuserdata(L, (void *)&plugin);
  lua_rawset(L, LUA_REGISTRYINDEX);

  int type = lua_rawgeti(L, LUA_REGISTRYINDEX, lua_ref);
  if (type != LUA_TFUNCTION) {
    vxe::log_error(
        "LuaHandler",
        "Corrupted register : ID " + std::to_string(lua_ref) + " is type " + std::string(lua_typename(L, type)));
    lua_pop(L, 1);

    lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
    lua_pushnil(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
    return;
  }

  lua_pushstring(L, path.c_str());

  if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
    std::string error = lua_tostring(L, -1);
    vxe::log_error("LuaHandler", "Error while executing Lua : " + error);
    lua_pop(L, 1);
  }

  lua_pushlightuserdata(L, (void *)&ACTIVE_PLUGIN_KEY);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);
}

LuaItemHandler::~LuaItemHandler() {
  if (L && lua_ref != LUA_NOREF && lua_ref != LUA_REFNIL) {
    luaL_unref(L, LUA_REGISTRYINDEX, lua_ref);
  }
}