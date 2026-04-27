//
//  interface.cpp
//  Sources for plugins interfaces features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/plugins/interface.hpp"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

VORTEX_API void PluginInterface::add_plugin_function(const PluginFunction &function) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(function);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

VORTEX_API void PluginInterface::add_output_event(const PluginOutputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(const PluginInputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_content_browser_item_handler(const ItemHandlerInterface &handler) {
  item_handlers_.push_back(std::make_shared<ItemHandlerInterface>(handler));
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> PluginInterface::get_content_browser_item_handlers() {
  return item_handlers_;
}

VORTEX_API std::string PluginInterface::cook_path(const std::string &path) {
  return this->path() + "/" + path;
}

VORTEX_API std::string PluginInterface::GetMainScriptPath() {
  return m_mainscript_path;
}

VORTEX_API void
PluginInterface::add_documentation(const std::string &section, const std::string &title, const std::string &path) {
  std::string topic = "plugin:" + this->name_;
  vxe::add_documentation(topic, section, title, path);
}

VORTEX_API void PluginInterface::refresh_main_window() {
  if (!&Cherry::Application::Get()) {
    return;
  }

  // Remove potential old main window
  for (auto &window : Cherry::Application::Get().m_AppWindows) {
    if (main_window_ && window) {
      if (main_window_->m_IdName == window->m_IdName) {
        Cherry::DeleteAppWindow(window);
      }
    }
  }

  // Add/Set the new one
  if (main_window_) {
    Cherry::AddAppWindow(main_window_);
  }
}

VORTEX_API void PluginInterface::set_main_window(const std::shared_ptr<Cherry::AppWindow> &win) {
  main_window_ = win;

  if (&Cherry::Application::Get()) {
    refresh_main_window();
  }
}

VORTEX_API void PluginInterface::add_plugin_render_instance(const std::shared_ptr<PluginRenderInstance> &renderInstance) {
  // Add the shared_ptr to the list of render instances
  this->render_instances_.push_back(renderInstance);
}

VORTEX_API void PluginInterface::add_logo(const uint8_t *hexa, size_t size) {
  // Set the logo data and size
  this->logo_ = hexa;
  this->logo_size_ = size;
}

VORTEX_API void PluginInterface::add_logo(const std::string &relative_path) {
}

VORTEX_API void PluginInterface::add_output_event(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_output_event(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_output_event(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_output_event(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void PluginInterface::add_function(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

VORTEX_API void PluginInterface::add_function(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

VORTEX_API void PluginInterface::add_function(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

VORTEX_API void PluginInterface::add_function(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

VORTEX_API std::shared_ptr<PluginInterface> PluginInterface::get_interface() {
  // Create a shared_ptr to a new PluginInterface object, copying the current
  // instance
  return std::make_shared<PluginInterface>(*this);
}

VORTEX_API std::shared_ptr<PluginInterface> PluginInterface::get_editor_plugin_by_name(const std::string &name) {
  auto ctx = vxe::get_current_context();

  for (auto ep : ctx->IO.ep) {
    if (name == ep->name_) {
      // Return a shared_ptr to the editor plugin if found
      return ep;
    }
  }

  // Return nullptr if editor plugin with the specified name is not found
  return nullptr;
}

VORTEX_API void PluginInterface::execute_function(const std::string &name) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      ArgumentValues empty_args;
      ReturnValues empty_ret;
      foo->m_function(empty_args, empty_ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_function(const std::string &name, ReturnValues &ret) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      ArgumentValues empty_args;
      foo->m_function(empty_args, ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_function(const std::string &name, ArgumentValues &args) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      ReturnValues empty_ret;
      foo->m_function(args, empty_ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_function(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      foo->m_function(args, ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_input_event(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto event : this->input_events_) {
    if (event->name_ == name) {
      event->m_function(args, ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_output_event(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto event : this->output_events_) {
    if (event->name_ == name) {
      event->m_function(args, ret);
      return;  // Exit after executing the output event
    }
  }
}

VORTEX_API void PluginInterface::log_info(const std::string &message) {
  vxe::log_info(this->name_, message);
}

VORTEX_API void PluginInterface::log_warning(const std::string &message) {
  vxe::log_warn(this->name_, message);
}

VORTEX_API void PluginInterface::log_error(const std::string &message) {
  vxe::log_error(this->name_, message);
}

VORTEX_API void PluginInterface::log_fatal(const std::string &message) {
  vxe::log_fatal(this->name_, message);
}

/**
 * @brief Checks the dependencies of the plugin interface.
 *
 * This function checks if the dependencies specified for the plugin interface
 * are satisfied based on the current context.
 */
VORTEX_API void PluginInterface::check_dependencies() {
  // Get the current Vortex context
  auto ctx = vxe::get_current_context();

  // Iterate through each dependency
  for (auto dependency : this->dependencies_) {
    // By default, dependency is not satisfied
    dependency->satisfied = false;

    // Check dependency type
    if (dependency->type == "ep") {
      // Dependency is an entity plugin
      for (auto ctx_ep : ctx->IO.ep) {
        // Check if context entity plugin matches the dependency name
        if (ctx_ep->name() == dependency->name) {
          // Check if the context entity plugin version is among the supported
          // versions
          for (auto version : dependency->supported_versions) {
            if (ctx_ep->version() == version) {
              // Dependency is satisfied
              dependency->satisfied = true;
              break;  // No need to check further versions
            }
          }
        }
      }
    }
    // Futures deps types... (plugins, etc...)
  }
}

/**
 * @brief Starts the plugin interface.
 *
 * Checks dependencies and executes the plugin's main function.
 * If any dependency is not satisfied, logs a fatal error, lists the failed
 * dependencies, and sets the state to "failed". If all dependencies are
 * satisfied, executes the main function and sets the state to "running".
 */
VORTEX_API void PluginInterface::start() {
  // Get the current Vortex context
  auto ctx = vxe::get_current_context();

  // Update dependencies
  this->check_dependencies();

  // Get current vortex version
  std::string version = VORTEX_VERSION;

  std::size_t first_dot = version.find('.');
  std::size_t second_dot = version.find('.', first_dot + 1);

  std::string major = version.substr(0, second_dot);

  if (!major.empty() && major.back() == '.') {
    major.pop_back();
  }

  // Check if all dependencies are satisfied
  bool allDependenciesSatisfied = true;
  bool isVersionCompatible = false;

  // Vector of unsatisfied dependencies
  std::vector<std::pair<std::string, std::string>> unsatisfiedDependencies;

  for (auto dependency : this->dependencies_) {
    if (!dependency->satisfied) {
      allDependenciesSatisfied = false;
      std::string versions;
      for (auto version : dependency->supported_versions) {
        versions += "[" + version + "]";
      }
      unsatisfiedDependencies.push_back({ dependency->name, versions });
    }
  }

  for (auto version : this->supported_versions_) {
    if (version == major) {
      std::cout << version << major << std::endl;
      isVersionCompatible = true;
    }
  }

  // Vector of needed dependencies (to run)
  std::vector<std::pair<std::string, std::string>> needToRunDependencies;
  for (auto dep : this->dependencies_) {
    for (auto ep : ctx->IO.ep) {
      if (ep->name() == dep->name) {
        std::string versions;
        for (auto version : dep->supported_versions) {
          versions += "[" + version + "]";
        }

        if (ep->state() != "running") {
          needToRunDependencies.push_back({ dep->name, versions });
        }
      }
    }
  }

  // If any dependency is not satisfied, log and set state to "failed"
  if (!allDependenciesSatisfied) {
    this->log_error("Dependencies not satisfied!");
    for (auto unsatisfiedDep : unsatisfiedDependencies) {
      this->log_error("Failed to retrieve: " + unsatisfiedDep.first + " with version(s) " + unsatisfiedDep.second);
    }
    this->state_ = "failed";
    return;
  }

  if (!isVersionCompatible) {
    this->log_error(
        "This plugin isn't compatible with this version of Vortex (\"" + major + "\") ! Is incompatible with \"" +
        this->name_ + "\" supported version(s).");

    this->state_ = "failed";
    return;
  }

  if (!needToRunDependencies.empty()) {
    this->log_error("Dependencies of this plugins are not running !");

    for (auto notRunningDeps : needToRunDependencies) {
      this->log_error("Please run : " + notRunningDeps.first + " with version(s) " + notRunningDeps.second);
    }
    this->state_ = "failed";
    return;
  }

  auto &engine = vxe::script::get_scripting_engine();
  engine.load_file_for_plugin(this->m_mainscript_path, this->shared_from_this());

  this->state_ = "running";
}

/**
 * @brief Stops the plugin interface.
 *
 * Calls the destroy function to clean up resources and sets the state to
 * "stopped".
 */
VORTEX_API void PluginInterface::stop() {
  // Get the current Vortex context
  auto ctx = vxe::get_current_context();

  bool authorized = true;
  std::vector<std::pair<std::string, std::string>> deps;

  for (auto ep : ctx->IO.ep) {
    if (ep->state_ == "running") {
      for (auto dependency : ep->dependencies()) {
        if (dependency->name == this->name_) {
          for (auto version : dependency->supported_versions) {
            if (this->version_ == version) {
              authorized = false;

              std::string versions;
              for (auto version : dependency->supported_versions) {
                versions += "[" + version + "]";
              }
              deps.push_back({ dependency->name, versions });
            }
          }
        }
      }
    }
  }

  if (!authorized) {
    this->log_error("Cannot stop \"" + this->name_ + "\" ! ");
    for (auto dep : deps) {
      this->log_error("This plugin is used by \"" + dep.first + "\" with needed versions : " + dep.second + " ! ");
    }
  } else {
    item_handlers_.clear();
    m_lua_handlers.clear();
    this->destroy();
    this->state_ = "stopped";
  }
}

VORTEX_API void PluginInterface::ResetPlugin() {
  item_handlers_.clear();
}

VORTEX_API void PluginInterface::call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret) {
  vxe::call_output_event(event_name, args, ret, this->name_);
}

VORTEX_API void PluginInterface::call_input_event(
    const std::string &plugin_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret) {
  vxe::call_input_event(plugin_name, event_name, args, ret, this->name_);
}

VORTEX_API void PluginInterface::check_version() {
}

std::vector<std::shared_ptr<PluginRenderInstance>> PluginInterface::get_plugin_render_instances() {
  return this->render_instances_;
};

VORTEX_API void PluginInterface::add_content_browser_item_identifier(const ItemIdentifierInterface &item) {
  item_identifiers_.push_back(std::make_shared<ItemIdentifierInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &PluginInterface::get_content_browser_item_identifiers() {
  return item_identifiers_;
}

VORTEX_API void PluginInterface::add_content_browser_item_creator(const ItemCreatorInterface &item) {
  item_creators_.push_back(std::make_shared<ItemCreatorInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &PluginInterface::get_content_browser_item_creators() {
  return item_creators_;
}

VORTEX_API void PluginInterface::set_credits_file(const std::string &file_path) {
  vxe::set_credits_file(this->proper_name_ + "(" + this->name_ + ")", file_path);
}

const std::string &PluginInterface::state() const noexcept {
  return state_;
}

const std::string &PluginInterface::datapath() const noexcept {
  return datapath_;
}

const std::string &PluginInterface::type() const noexcept {
  return type_;
}

const std::string &PluginInterface::proper_name() const noexcept {
  return proper_name_;
}

const std::string &PluginInterface::name() const noexcept {
  return name_;
}

const std::string &PluginInterface::version() const noexcept {
  return version_;
}

const std::string &PluginInterface::path() const noexcept {
  return path_;
}

const std::string &PluginInterface::author() const noexcept {
  return author_;
}

const std::string &PluginInterface::group() const noexcept {
  return group_;
}

const std::string &PluginInterface::picture() const noexcept {
  return picture_;
}

const std::string &PluginInterface::logo_path() const noexcept {
  return logo_path_;
}

const std::string &PluginInterface::description() const noexcept {
  return description_;
}

const std::vector<std::shared_ptr<PluginFunction>> &PluginInterface::functions() const noexcept {
  return functions_;
}

const std::vector<std::shared_ptr<PluginOutputEvent>> &PluginInterface::output_events() const noexcept {
  return output_events_;
}

const std::vector<std::shared_ptr<PluginInputEvent>> &PluginInterface::input_events() const noexcept {
  return input_events_;
}

const std::vector<std::shared_ptr<ItemHandlerInterface>> &PluginInterface::item_handlers() const noexcept {
  return item_handlers_;
}

const std::vector<std::shared_ptr<ItemIdentifierInterface>> &PluginInterface::item_identifiers() const noexcept {
  return item_identifiers_;
}

const std::vector<std::shared_ptr<ItemCreatorInterface>> &PluginInterface::item_creators() const noexcept {
  return item_creators_;
}

const std::vector<std::shared_ptr<PluginDummyFunction>> &PluginInterface::dummy_functions() const noexcept {
  return dummy_functions_;
}

const std::vector<std::shared_ptr<PluginRenderInstance>> &PluginInterface::render_instances() const noexcept {
  return render_instances_;
}

bool PluginInterface::auto_exec() const noexcept {
  return auto_exec_;
}

void PluginInterface::datapath(std::string v) {
  datapath_ = std::move(v);
}

void PluginInterface::type(std::string v) {
  type_ = std::move(v);
}

void PluginInterface::proper_name(std::string v) {
  proper_name_ = std::move(v);
}

void PluginInterface::name(std::string v) {
  name_ = std::move(v);
}

void PluginInterface::version(std::string v) {
  version_ = std::move(v);
}

void PluginInterface::path(std::string v) {
  path_ = std::move(v);
}

void PluginInterface::author(std::string v) {
  author_ = std::move(v);
}

void PluginInterface::group(std::string v) {
  group_ = std::move(v);
}

void PluginInterface::picture(std::string v) {
  picture_ = std::move(v);
}

void PluginInterface::logo_path(std::string v) {
  logo_path_ = std::move(v);
}

void PluginInterface::description(std::string v) {
  description_ = std::move(v);
}

void PluginInterface::auto_exec(bool v) {
  auto_exec_ = v;
}

const std::vector<std::string> &PluginInterface::contributors() const noexcept {
  return contributors_;
}

void PluginInterface::contributors(std::vector<std::string> v) {
  contributors_ = std::move(v);
}

void PluginInterface::add_contributor(std::string v) {
  contributors_.emplace_back(std::move(v));
}

void PluginInterface::clear_contributors() noexcept {
  contributors_.clear();
}

const std::vector<std::shared_ptr<PluginInterfaceDep>> &PluginInterface::dependencies() const noexcept {
  return dependencies_;
}

void PluginInterface::dependencies(std::vector<std::shared_ptr<PluginInterfaceDep>> v) {
  dependencies_ = std::move(v);
}

void PluginInterface::add_dependency(std::shared_ptr<PluginInterfaceDep> v) {
  dependencies_.emplace_back(std::move(v));
}

void PluginInterface::clear_dependencies() noexcept {
  dependencies_.clear();
}

const std::vector<std::string> &PluginInterface::supported_versions() const noexcept {
  return supported_versions_;
}

void PluginInterface::supported_versions(std::vector<std::string> v) {
  supported_versions_ = std::move(v);
}

void PluginInterface::add_supported_version(std::string v) {
  supported_versions_.emplace_back(std::move(v));
}

void PluginInterface::clear_supported_versions() noexcept {
  supported_versions_.clear();
}
