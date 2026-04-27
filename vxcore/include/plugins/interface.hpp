
//
//  interface.hpp
//  Headers, helpers and structures for plugin interfaces
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex_internals.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#if defined(__linux__)
#include <cxxabi.h>
#elif defined(_WIN32) || defined(_WIN64)
//
#elif defined(__APPLE__)
//
#else
//
#endif

#include "../vortex/scripting/lua_helpers.hpp"
#include "../vortex/scripting/scripting.hpp"
#include "dummy_function.hpp"
#include "event.hpp"
#include "function.hpp"
#include "render_instance.hpp"

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

// Forward declarations
struct ItemHandlerInterface;
struct ItemIdentifierInterface;
struct ItemCreatorInterface;
class PluginInterface;

VORTEX_API struct LuaItemHandler {
  int lua_ref;
  lua_State *L;
  std::shared_ptr<PluginInterface> plugin;

  LuaItemHandler(int ref, lua_State *state, std::shared_ptr<PluginInterface> p);

  LuaItemHandler(const LuaItemHandler &) = delete;
  LuaItemHandler &operator=(const LuaItemHandler &) = delete;

  LuaItemHandler(LuaItemHandler &&other) noexcept;

  void Call(const std::string &path);

  ~LuaItemHandler();
};
VORTEX_API struct PluginInterfaceDep {
  std::string type;  // em, plugin, etc..
  std::string name;
  std::vector<std::string> supported_versions;
  bool satisfied;
};

VORTEX_API class PluginInterface : public std::enable_shared_from_this<PluginInterface> {
 public:
  virtual ~PluginInterface() {
  }

  // Main functions
  virtual void execute() { };
  virtual void destroy() { };
  virtual void render() { };

  VORTEX_API void start();
  VORTEX_API void stop();

  // Hooks
  VORTEX_API void on_input_event();
  VORTEX_API void on_output_event();

  // Misc
  VORTEX_API void add_logo(const uint8_t *hexa, size_t size);
  VORTEX_API void add_logo(const std::string &relative_path);
  VORTEX_API void reset_plugin();

  // Functions of the plugins (gives the Vortex abstraction/features)
  VORTEX_API void add_function(std::function<void()> foo, const std::string &name);
  VORTEX_API void add_function(std::function<void(ArgumentValues &)> foo, const std::string &name);
  VORTEX_API void add_function(std::function<void(ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_function(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  VORTEX_API void execute_function(const std::string &name);
  VORTEX_API void execute_function(const std::string &name, ArgumentValues &args);
  VORTEX_API void execute_function(const std::string &name, ReturnValues &ret);
  VORTEX_API void execute_function(const std::string &name, ArgumentValues &args, ReturnValues &ret);

  VORTEX_API std::string get_main_script_path();

  // Documentation
  VORTEX_API void add_documentation(const std::string &section, const std::string &title, const std::string &path);
  // Output Events
  // A output event is triggered via vxe::execute_output_event() by the
  // current component to all concerned extern components with the output event
  // added.
  VORTEX_API void add_output_event(std::function<void()> foo, const std::string &name);
  VORTEX_API void add_output_event(std::function<void(ArgumentValues &)> foo, const std::string &name);
  VORTEX_API void add_output_event(std::function<void(ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_output_event(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_output_event(const PluginOutputEvent &event);

  // Input Events
  // A input event is triggered via vxe::execute_input_event() by a extern
  // component to the current component with the input component added.
  VORTEX_API void add_input_event(std::function<void()> foo, const std::string &name);
  VORTEX_API void add_input_event(std::function<void(ArgumentValues &)> foo, const std::string &name);
  VORTEX_API void add_input_event(std::function<void(ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_input_event(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_input_event(const PluginInputEvent &event);

  // GUI stuffs
  VORTEX_API void refresh_main_window();
  VORTEX_API void set_main_window(const std::shared_ptr<Cherry::AppWindow> &win);
  VORTEX_API void add_window(const std::shared_ptr<Cherry::AppWindow> &win);

  // Content Browser item handler
  VORTEX_API void add_content_browser_item_handler(const ItemHandlerInterface &handler);
  VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> get_content_browser_item_handlers();

  // Content Browser item identifier
  VORTEX_API void add_content_browser_item_identifier(const ItemIdentifierInterface &item);
  VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &get_content_browser_item_identifiers();

  // Content Browser item creators
  VORTEX_API void add_content_browser_item_creator(const ItemCreatorInterface &item);
  VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &get_content_browser_item_creators();

  // Logs & Metrics
  VORTEX_API void log_info(const std::string &message);
  VORTEX_API void log_warning(const std::string &message);
  VORTEX_API void log_error(const std::string &message);
  VORTEX_API void log_fatal(const std::string &message);

  // Credits
  VORTEX_API void set_credits_file(const std::string &file_path);

  // TODO : AddParameterToFunction
  // TODO : AddParameterToInputEvent
  // TODO : AddParameterToOutputEvent
  // TODO : AddReturnToFunction
  // TODO : AddReturnToInputEvent
  // TODO : AddReturnToOutputEvent
  // TODO : AddCommonUtility

  // void add_function(void (*item)(), const std::string& name, Parameters
  // params);
  VORTEX_API std::shared_ptr<PluginInterface> get_interface();

  VORTEX_API void execute_output_event(const std::string &name, ArgumentValues &args, ReturnValues &ret);
  VORTEX_API void execute_input_event(const std::string &name, ArgumentValues &args, ReturnValues &ret);

  // Trigger all output events for every modules/plugins
  // VORTEX_API void DeployOutputEvent(const std::string &name, std::shared_ptr<hArgs> args);
  // VORTEX_API void ExecOutputEvent(const std::string &name, std::shared_ptr<hArgs> args);

  template<typename T>
  VORTEX_API void add_plugin_item_param(const void *item, std::pair<std::string, T> parameter);

  VORTEX_API void add_plugin_render_instance(const std::shared_ptr<PluginRenderInstance> &event);
  VORTEX_API void add_plugin_function(const PluginFunction &event);
  std::vector<std::shared_ptr<PluginRenderInstance>> get_plugin_render_instances();

  VORTEX_API void call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret);
  VORTEX_API void call_input_event(
      const std::string &plugin_name,  // or module
      const std::string &event_name,
      ArgumentValues &args,
      ReturnValues &ret);

  VORTEX_API static std::shared_ptr<PluginInterface> get_editor_plugin_by_name(const std::string &name);

  VORTEX_API void add_lua_handler(const std::shared_ptr<LuaItemHandler> &handler) {
    lua_handlers_.push_back(handler);
  }

  // utilities
  VORTEX_API std::string cook_path(const std::string &path);
  VORTEX_API void check_dependencies();
  VORTEX_API void check_version();
  VORTEX_API void add_contributor(std::string v);
  VORTEX_API void clear_contributors() noexcept;
  VORTEX_API void add_dependency(std::shared_ptr<PluginInterfaceDep> v);
  VORTEX_API void clear_dependencies() noexcept;
  VORTEX_API void add_supported_version(std::string v);
  VORTEX_API void clear_supported_versions() noexcept;

  // getters
  VORTEX_API const std::string &state() const noexcept;
  VORTEX_API const std::string &datapath() const noexcept;
  VORTEX_API const std::string &type() const noexcept;
  VORTEX_API const std::string &proper_name() const noexcept;
  VORTEX_API const std::string &name() const noexcept;
  VORTEX_API const std::string &version() const noexcept;
  VORTEX_API const std::string &path() const noexcept;
  VORTEX_API const std::string &author() const noexcept;
  VORTEX_API const std::string &group() const noexcept;
  VORTEX_API const std::string &picture() const noexcept;
  VORTEX_API const std::string &logo_path() const noexcept;
  VORTEX_API const std::string &description() const noexcept;
  VORTEX_API const std::string &mainscript_path() const noexcept;
  VORTEX_API bool auto_exec() const noexcept;
  VORTEX_API const std::vector<std::string> &contributors() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<PluginInterfaceDep>> &dependencies() const noexcept;
  VORTEX_API const std::vector<std::string> &supported_versions() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<PluginFunction>> &functions() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<PluginOutputEvent>> &output_events() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<PluginInputEvent>> &input_events() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ItemHandlerInterface>> &item_handlers() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ItemIdentifierInterface>> &item_identifiers() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ItemCreatorInterface>> &item_creators() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<PluginDummyFunction>> &dummy_functions() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<PluginRenderInstance>> &render_instances() const noexcept;

  // setters
  VORTEX_API void state(std::string v);
  VORTEX_API void datapath(std::string v);
  VORTEX_API void type(std::string v);
  VORTEX_API void proper_name(std::string v);
  VORTEX_API void name(std::string v);
  VORTEX_API void version(std::string v);
  VORTEX_API void path(std::string v);
  VORTEX_API void author(std::string v);
  VORTEX_API void group(std::string v);
  VORTEX_API void picture(std::string v);
  VORTEX_API void logo_path(std::string v);
  VORTEX_API void description(std::string v);
  VORTEX_API void mainscript_path(std::string v);
  VORTEX_API void auto_exec(bool v);
  VORTEX_API void contributors(std::vector<std::string> v);
  VORTEX_API void dependencies(std::vector<std::shared_ptr<PluginInterfaceDep>> v);
  VORTEX_API void supported_versions(std::vector<std::string> v);

 private:
  std::string datapath_;
  std::string type_;
  std::string proper_name_;
  std::string name_;
  std::string version_;
  std::string path_;
  std::string mainscript_path_;
  std::string author_;
  std::string group_;
  std::string picture_;
  std::string logo_path_;
  std::string description_;
  bool auto_exec_;
  std::vector<std::string> contributors_;
  std::vector<std::shared_ptr<PluginInterfaceDep>> dependencies_;
  std::vector<std::string> supported_versions_;
  const uint8_t *logo_;
  size_t logo_size_;

  std::string state_ = "unknow";
  std::shared_ptr<Cherry::AppWindow> main_window_;

  std::vector<std::shared_ptr<PluginFunction>> functions_;
  std::vector<std::shared_ptr<PluginOutputEvent>> output_events_;
  std::vector<std::shared_ptr<PluginInputEvent>> input_events_;
  std::vector<std::shared_ptr<ItemHandlerInterface>> item_handlers_;
  std::vector<std::shared_ptr<ItemIdentifierInterface>> item_identifiers_;
  std::vector<std::shared_ptr<ItemCreatorInterface>> item_creators_;

  std::vector<std::shared_ptr<LuaItemHandler>> lua_handlers_;

  std::vector<std::shared_ptr<PluginDummyFunction>> dummy_functions_;
  std::vector<std::shared_ptr<PluginRenderInstance>> render_instances_;
};

#endif  // PLUGIN_INTERFACE_H