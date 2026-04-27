//
//  interface.hpp
//  Headers, structures and utilities for modules interfaces
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include <vortex_internals.h>

#include "dummy_function.hpp"
#include "event.hpp"
#include "function.hpp"
#include "helpers.hpp"
#include "render_instance.hpp"

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

VORTEX_API class ModuleInterface {
 public:
  virtual ~ModuleInterface() {
  }

  // Main functions
  virtual void execute() { };
  virtual void init_ui() { };
  virtual void init_runtime() { };
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
  VORTEX_API void reset_module();

  // Functions of the modules (gives the Vortex abstraction/features)
  VORTEX_API void add_function(std::function<void()> foo, const std::string &name);
  VORTEX_API void add_function(std::function<void(ArgumentValues &)> foo, const std::string &name);
  VORTEX_API void add_function(std::function<void(ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_function(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  VORTEX_API void execute_function(const std::string &name);
  VORTEX_API void execute_function(const std::string &name, ArgumentValues &args);
  VORTEX_API void execute_function(const std::string &name, ReturnValues &ret);
  VORTEX_API void execute_function(const std::string &name, ArgumentValues &args, ReturnValues &ret);

  // Documentation
  VORTEX_API void add_documentation(const std::string &section, const std::string &title, const std::string &path);

  // Output Events
  VORTEX_API void add_output_event(std::function<void()> foo, const std::string &name);
  VORTEX_API void add_output_event(std::function<void(ArgumentValues &)> foo, const std::string &name);
  VORTEX_API void add_output_event(std::function<void(ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_output_event(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_output_event(const ModuleOutputEvent &event);

  // Input Events
  VORTEX_API void add_input_event(std::function<void()> foo, const std::string &name);
  VORTEX_API void add_input_event(std::function<void(ArgumentValues &)> foo, const std::string &name);
  VORTEX_API void add_input_event(std::function<void(ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_input_event(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
  VORTEX_API void add_input_event(const ModuleInputEvent &event);

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

  VORTEX_API std::shared_ptr<ModuleInterface> get_interface();

  VORTEX_API void execute_output_event(const std::string &name, ArgumentValues &args, ReturnValues &ret);
  VORTEX_API void execute_input_event(const std::string &name, ArgumentValues &args, ReturnValues &ret);

  template<typename T>
  VORTEX_API void add_module_item_param(const void *item, std::pair<std::string, T> parameter);

  VORTEX_API void add_module_render_instance(const std::shared_ptr<ModuleRenderInstance> &event);
  VORTEX_API void add_module_function(const ModuleFunction &event);
  std::vector<std::shared_ptr<ModuleRenderInstance>> get_module_render_instances();

  VORTEX_API void call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret);
  VORTEX_API void
  call_input_event(const std::string &module_name, const std::string &event_name, ArgumentValues &args, ReturnValues &ret);

  VORTEX_API static std::shared_ptr<ModuleInterface> get_editor_module_by_name(const std::string &name);

  // utilities
  VORTEX_API std::string cook_path(const std::string &path);
  VORTEX_API void check_dependencies();
  VORTEX_API void check_version();
  VORTEX_API void add_contributor(std::string v);
  VORTEX_API void clear_contributors() noexcept;
  VORTEX_API void add_dependency(std::shared_ptr<ModuleInterfaceDep> v);
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
  VORTEX_API const std::string &binary_path() const noexcept;
  VORTEX_API const std::string &author() const noexcept;
  VORTEX_API const std::string &group() const noexcept;
  VORTEX_API const std::string &picture() const noexcept;
  VORTEX_API const std::string &logo_path() const noexcept;
  VORTEX_API const std::string &description() const noexcept;
  VORTEX_API bool auto_exec() const noexcept;
  VORTEX_API const std::vector<std::string> &contributors() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ModuleInterfaceDep>> &dependencies() const noexcept;
  VORTEX_API const std::vector<std::string> &supported_versions() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ModuleFunction>> &functions() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ModuleOutputEvent>> &output_events() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ModuleInputEvent>> &input_events() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ItemHandlerInterface>> &item_handlers() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ItemIdentifierInterface>> &item_identifiers() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ItemCreatorInterface>> &item_creators() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ModuleDummyFunction>> &dummy_functions() const noexcept;
  VORTEX_API const std::vector<std::shared_ptr<ModuleRenderInstance>> &render_instances() const noexcept;

  // setters
  VORTEX_API void state(std::string v);
  VORTEX_API void datapath(std::string v);
  VORTEX_API void type(std::string v);
  VORTEX_API void proper_name(std::string v);
  VORTEX_API void name(std::string v);
  VORTEX_API void version(std::string v);
  VORTEX_API void path(std::string v);
  VORTEX_API void binary_path(std::string v);
  VORTEX_API void author(std::string v);
  VORTEX_API void group(std::string v);
  VORTEX_API void picture(std::string v);
  VORTEX_API void logo_path(std::string v);
  VORTEX_API void description(std::string v);
  VORTEX_API void auto_exec(bool v);
  VORTEX_API void contributors(std::vector<std::string> v);
  VORTEX_API void dependencies(std::vector<std::shared_ptr<ModuleInterfaceDep>> v);
  VORTEX_API void supported_versions(std::vector<std::string> v);

 private:
  // main informations and datas
  std::string datapath_;
  std::string type_;
  std::string proper_name_;
  std::string name_;
  std::string version_;
  std::string path_;
  std::string binary_path_;
  std::string author_;
  std::string group_;
  std::string picture_;
  std::string logo_path_;
  std::string description_;
  bool auto_exec_;
  std::vector<std::string> contributors_;
  std::vector<std::shared_ptr<ModuleInterfaceDep>> dependencies_;
  std::vector<std::string> supported_versions_;
  const uint8_t *logo_;
  size_t logo_size_;

  // states
  std::string state_ = "unknow";

  std::shared_ptr<Cherry::AppWindow> main_window_;

  // IO, functions, events
  std::vector<std::shared_ptr<ModuleFunction>> functions_;
  std::vector<std::shared_ptr<ModuleOutputEvent>> output_events_;
  std::vector<std::shared_ptr<ModuleInputEvent>> input_events_;
  std::vector<std::shared_ptr<ItemHandlerInterface>> item_handlers_;
  std::vector<std::shared_ptr<ItemIdentifierInterface>> item_identifiers_;
  std::vector<std::shared_ptr<ItemCreatorInterface>> item_creators_;
  std::vector<std::shared_ptr<ModuleDummyFunction>> dummy_functions_;
  std::vector<std::shared_ptr<ModuleRenderInstance>> render_instances_;
};

#endif  // MODULE_INTERFACE_H