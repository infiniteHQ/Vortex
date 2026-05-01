//
//  interface.cpp
//  Sources for modules interfaces features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/modules/interface.hpp"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Adds a ModuleFunction to the ModuleInterface.
 *
 * This function creates a shared_ptr to the provided ModuleFunction and adds it
 * to the ModuleInterface's list of functions.
 *
 * @param function The ModuleFunction to add.
 */
VORTEX_API void ModuleInterface::add_module_function(const ModuleFunction &function) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(function);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

/**
 * @brief Adds a ModuleOutputEvent to the ModuleInterface.
 *
 * This function creates a shared_ptr to the provided ModuleOutputEvent and adds
 * it to the ModuleInterface's list of output events.
 *
 * @param event The ModuleOutputEvent to add.
 */
VORTEX_API void ModuleInterface::add_output_event(const ModuleOutputEvent &event) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_input_event(const ModuleInputEvent &event) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_content_browser_item_handler(const ItemHandlerInterface &handler) {
  item_handlers_.push_back(std::make_shared<ItemHandlerInterface>(handler));
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> ModuleInterface::get_content_browser_item_handlers() {
  return item_handlers_;
}

VORTEX_API std::string ModuleInterface::cook_path(const std::string &path) {
  return this->path() + "/" + path;
}

VORTEX_API void
ModuleInterface::add_documentation(const std::string &section, const std::string &title, const std::string &path) {
  std::string topic = "module:" + this->name_;
  vxe::add_documentation(topic, section, title, path);
}

VORTEX_API void ModuleInterface::refresh_main_window() {
  if (!&Cherry::Application::Get()) {
    return;
  }

  // Remove potential old main window
  for (auto &window : Cherry::Application::Get().GetAppWindows()) {
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

VORTEX_API void ModuleInterface::set_main_window(const std::shared_ptr<Cherry::AppWindow> &win) {
  main_window_ = win;

  if (&Cherry::Application::Get()) {
    refresh_main_window();
  }
}

/**
 * @brief Adds a ModuleRenderInstance to the ModuleInterface.
 *
 * This function adds the provided shared_ptr to a ModuleRenderInstance to the
 * ModuleInterface's list of render instances.
 *
 * @param renderInstance The shared_ptr to the ModuleRenderInstance to add.
 */
VORTEX_API void ModuleInterface::add_module_render_instance(const std::shared_ptr<ModuleRenderInstance> &renderInstance) {
  // Add the shared_ptr to the list of render instances
  this->render_instances_.push_back(renderInstance);
}

/**
 * @brief Sets the logo for the ModuleInterface.
 *
 * This function sets the logo data and size for the ModuleInterface.
 *
 * @param hexa Pointer to the logo data as an array of uint8_t.
 * @param size Size of the logo data in bytes.
 */
VORTEX_API void ModuleInterface::add_logo(const uint8_t *hexa, size_t size) {
  // Set the logo data and size
  this->logo_ = hexa;
  this->logo_size_ = size;
}

VORTEX_API void ModuleInterface::add_logo(const std::string &relative_path) {
}

VORTEX_API void ModuleInterface::add_output_event(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_output_event(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_output_event(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_output_event(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleOutputEvent> p_event = std::make_shared<ModuleOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->output_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_input_event(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_input_event(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_input_event(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

VORTEX_API void ModuleInterface::add_input_event(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the ModuleOutputEvent
  std::shared_ptr<ModuleInputEvent> p_event = std::make_shared<ModuleInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->input_events_.push_back(p_event);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::add_function(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::add_function(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::add_function(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

/**
 * @brief Adds a function to the ModuleInterface.
 *
 * This function creates a shared_ptr to a ModuleFunction and adds it to the
 * ModuleInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
VORTEX_API void ModuleInterface::add_function(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the ModuleFunction
  std::shared_ptr<ModuleFunction> p_function = std::make_shared<ModuleFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->functions_.push_back(p_function);
}

/**
 * @brief Gets a shared_ptr to a copy of the ModuleInterface.
 *
 * This function creates a shared_ptr to a new ModuleInterface object, copying
 * the contents of the current ModuleInterface instance.
 *
 * @return A shared_ptr to a copy of the ModuleInterface.
 */
VORTEX_API std::shared_ptr<ModuleInterface> ModuleInterface::get_interface() {
  // Create a shared_ptr to a new ModuleInterface object, copying the current
  // instance
  return std::make_shared<ModuleInterface>(*this);
}

/**
 * @brief Gets an editor module by name.
 *
 * This function searches for an editor module with the specified name
 * and returns a shared_ptr to it if found.
 *
 * @param name The name of the editor module to search for.
 * @return A shared_ptr to the editor module if found, otherwise nullptr.
 */
VORTEX_API std::shared_ptr<ModuleInterface> ModuleInterface::get_editor_module_by_name(const std::string &name) {
  auto ctx = vxe::get_current_context();

  for (auto em : ctx->IO.em) {
    if (name == em->name_) {
      // Return a shared_ptr to the editor module if found
      return em;
    }
  }

  // Return nullptr if editor module with the specified name is not found
  return nullptr;
}

/**
 * @brief Executes a function by name.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::execute_function(const std::string &name) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      ArgumentValues empty_args;
      ReturnValues empty_ret;
      foo->function_(empty_args, empty_ret);
      return;  // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::execute_function(const std::string &name, ReturnValues &ret) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      ArgumentValues empty_args;
      foo->function_(empty_args, ret);
      return;  // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::execute_function(const std::string &name, ArgumentValues &args) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      ReturnValues empty_ret;
      foo->function_(args, empty_ret);
      return;  // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a ModuleFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
VORTEX_API void ModuleInterface::execute_function(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto foo : this->functions_) {
    if (foo->name_ == name) {
      foo->function_(args, ret);
      return;  // Exit after executing the function
    }
  }
}

/**
 * @brief Executes an input event by name.
 *
 * This function searches for a ModuleInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 */
VORTEX_API void ModuleInterface::execute_input_event(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto event : this->input_events_) {
    if (event->name_ == name) {
      event->function_(args, ret);
      return;  // Exit after executing the input event
    }
  }
}
/**
 * @brief Executes an input event by name.
 *
 * This function searches for a ModuleInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 */
VORTEX_API void ModuleInterface::execute_output_event(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto event : this->output_events_) {
    if (event->name_ == name) {
      event->function_(args, ret);
      return;  // Exit after executing the output event
    }
  }
}

/**
 * @brief Logs an informational message.
 *
 * Logs an informational message associated with this module interface.
 *
 * @param message The message to log.
 */
VORTEX_API void ModuleInterface::log_info(const std::string &message) {
  vxe::log_info(this->name_, message);
}

/**
 * @brief Logs a warning message.
 *
 * Logs a warning message associated with this module interface.
 *
 * @param message The warning message to log.
 */
VORTEX_API void ModuleInterface::log_warning(const std::string &message) {
  vxe::log_warn(this->name_, message);
}

/**
 * @brief Logs an error message.
 *
 * Logs an error message associated with this module interface.
 *
 * @param message The error message to log.
 */
VORTEX_API void ModuleInterface::log_error(const std::string &message) {
  vxe::log_error(this->name_, message);
}

/**
 * @brief Logs a fatal error message.
 *
 * Logs a fatal error message associated with this module interface.
 *
 * @param message The fatal error message to log.
 */
VORTEX_API void ModuleInterface::log_fatal(const std::string &message) {
  vxe::log_fatal(this->name_, message);
}

/**
 * @brief Checks the dependencies of the module interface.
 *
 * This function checks if the dependencies specified for the module interface
 * are satisfied based on the current context.
 */
VORTEX_API void ModuleInterface::check_dependencies() {
  // Get the current Vortex context
  auto ctx = vxe::get_current_context();

  // Iterate through each dependency
  for (auto dependency : this->dependencies_) {
    // By default, dependency is not satisfied
    dependency->satisfied = false;

    // Check dependency type
    if (dependency->type == "em") {
      // Dependency is an entity module
      for (auto ctx_em : ctx->IO.em) {
        // Check if context entity module matches the dependency name
        if (ctx_em->name_ == dependency->name) {
          // Check if the context entity module version is among the supported
          // versions
          for (auto version : dependency->supported_versions) {
            if (ctx_em->version_ == version) {
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
 * @brief Starts the module interface.
 *
 * Checks dependencies and executes the module's main function.
 * If any dependency is not satisfied, logs a fatal error, lists the failed
 * dependencies, and sets the state to "failed". If all dependencies are
 * satisfied, executes the main function and sets the state to "running".
 */
VORTEX_API void ModuleInterface::start() {
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
    for (auto em : ctx->IO.em) {
      if (em->name_ == dep->name) {
        std::string versions;
        for (auto version : dep->supported_versions) {
          versions += "[" + version + "]";
        }

        if (em->state_ != "running") {
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
        "This module isn't compatible with this version of Vortex (\"" + major + "\") ! Is incompatible with \"" +
        this->name_ + "\" supported version(s).");

    this->state_ = "failed";
    return;
  }

  if (!needToRunDependencies.empty()) {
    this->log_error("Dependencies of this modules are not running !");

    for (auto notRunningDeps : needToRunDependencies) {
      this->log_error("Please run : " + notRunningDeps.first + " with version(s) " + notRunningDeps.second);
    }
    this->state_ = "failed";
    return;
  }

  // Execute the module's main function and set state to "running"
  this->execute();
  this->state_ = "running";
}

/**
 * @brief Stops the module interface.
 *
 * Calls the destroy function to clean up resources and sets the state to
 * "stopped".
 */
VORTEX_API void ModuleInterface::stop() {
  // Get the current Vortex context
  auto ctx = vxe::get_current_context();

  bool authorized = true;
  std::vector<std::pair<std::string, std::string>> deps;

  for (auto em : ctx->IO.em) {
    if (em->state_ == "running") {
      for (auto dependency : em->dependencies_) {
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
      this->log_error("This module is used by \"" + dep.first + "\" with needed versions : " + dep.second + " ! ");
    }
  } else {
    this->destroy();
    this->state_ = "stopped";
  }
}

VORTEX_API void ModuleInterface::reset_module() {
  item_handlers_.clear();
}

VORTEX_API void ModuleInterface::call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret) {
  vxe::call_output_event(event_name, args, ret, this->name_);
}

VORTEX_API void ModuleInterface::call_input_event(
    const std::string &module_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret) {
  vxe::call_input_event(module_name, event_name, args, ret, this->name_);
}

VORTEX_API void ModuleInterface::check_version() {
}

std::vector<std::shared_ptr<ModuleRenderInstance>> ModuleInterface::get_module_render_instances() {
  return this->render_instances_;
};

VORTEX_API void ModuleInterface::add_content_browser_item_identifier(const ItemIdentifierInterface &item) {
  item_identifiers_.push_back(std::make_shared<ItemIdentifierInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &ModuleInterface::get_content_browser_item_identifiers() {
  return item_identifiers_;
}

VORTEX_API void ModuleInterface::add_content_browser_item_creator(const ItemCreatorInterface &item) {
  item_creators_.push_back(std::make_shared<ItemCreatorInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &ModuleInterface::get_content_browser_item_creators() {
  return item_creators_;
}

VORTEX_API void ModuleInterface::set_credits_file(const std::string &file_path) {
  vxe::set_credits_file(this->proper_name_ + "(" + this->name_ + ")", file_path);
}

const std::string &ModuleInterface::state() const noexcept {
  return state_;
}

const std::string &ModuleInterface::datapath() const noexcept {
  return datapath_;
}

const std::string &ModuleInterface::type() const noexcept {
  return type_;
}

const std::string &ModuleInterface::proper_name() const noexcept {
  return proper_name_;
}

const std::string &ModuleInterface::name() const noexcept {
  return name_;
}

const std::string &ModuleInterface::version() const noexcept {
  return version_;
}

const std::string &ModuleInterface::path() const noexcept {
  return path_;
}

const std::string &ModuleInterface::binary_path() const noexcept {
  return binary_path_;
}

const std::string &ModuleInterface::author() const noexcept {
  return author_;
}

const std::string &ModuleInterface::group() const noexcept {
  return group_;
}

const std::string &ModuleInterface::picture() const noexcept {
  return picture_;
}

const std::string &ModuleInterface::logo_path() const noexcept {
  return logo_path_;
}

const std::string &ModuleInterface::description() const noexcept {
  return description_;
}

const std::vector<std::shared_ptr<ModuleFunction>> &ModuleInterface::functions() const noexcept {
  return functions_;
}

const std::vector<std::shared_ptr<ModuleOutputEvent>> &ModuleInterface::output_events() const noexcept {
  return output_events_;
}

const std::vector<std::shared_ptr<ModuleInputEvent>> &ModuleInterface::input_events() const noexcept {
  return input_events_;
}

const std::vector<std::shared_ptr<ItemHandlerInterface>> &ModuleInterface::item_handlers() const noexcept {
  return item_handlers_;
}

const std::vector<std::shared_ptr<ItemIdentifierInterface>> &ModuleInterface::item_identifiers() const noexcept {
  return item_identifiers_;
}

const std::vector<std::shared_ptr<ItemCreatorInterface>> &ModuleInterface::item_creators() const noexcept {
  return item_creators_;
}

const std::vector<std::shared_ptr<ModuleDummyFunction>> &ModuleInterface::dummy_functions() const noexcept {
  return dummy_functions_;
}

const std::vector<std::shared_ptr<ModuleRenderInstance>> &ModuleInterface::render_instances() const noexcept {
  return render_instances_;
}

bool ModuleInterface::auto_exec() const noexcept {
  return auto_exec_;
}

void ModuleInterface::datapath(std::string v) {
  datapath_ = std::move(v);
}

void ModuleInterface::type(std::string v) {
  type_ = std::move(v);
}

void ModuleInterface::proper_name(std::string v) {
  proper_name_ = std::move(v);
}

void ModuleInterface::name(std::string v) {
  name_ = std::move(v);
}

void ModuleInterface::version(std::string v) {
  version_ = std::move(v);
}

void ModuleInterface::path(std::string v) {
  path_ = std::move(v);
}

void ModuleInterface::binary_path(std::string v) {
  binary_path_ = std::move(v);
}

void ModuleInterface::author(std::string v) {
  author_ = std::move(v);
}

void ModuleInterface::group(std::string v) {
  group_ = std::move(v);
}

void ModuleInterface::picture(std::string v) {
  picture_ = std::move(v);
}

void ModuleInterface::logo_path(std::string v) {
  logo_path_ = std::move(v);
}

void ModuleInterface::description(std::string v) {
  description_ = std::move(v);
}

void ModuleInterface::auto_exec(bool v) {
  auto_exec_ = v;
}

const std::vector<std::string> &ModuleInterface::contributors() const noexcept {
  return contributors_;
}

void ModuleInterface::contributors(std::vector<std::string> v) {
  contributors_ = std::move(v);
}

void ModuleInterface::add_contributor(std::string v) {
  contributors_.emplace_back(std::move(v));
}

void ModuleInterface::clear_contributors() noexcept {
  contributors_.clear();
}

const std::vector<std::shared_ptr<ModuleInterfaceDep>> &ModuleInterface::dependencies() const noexcept {
  return dependencies_;
}

void ModuleInterface::dependencies(std::vector<std::shared_ptr<ModuleInterfaceDep>> v) {
  dependencies_ = std::move(v);
}

void ModuleInterface::add_dependency(std::shared_ptr<ModuleInterfaceDep> v) {
  dependencies_.emplace_back(std::move(v));
}

void ModuleInterface::clear_dependencies() noexcept {
  dependencies_.clear();
}

const std::vector<std::string> &ModuleInterface::supported_versions() const noexcept {
  return supported_versions_;
}

void ModuleInterface::supported_versions(std::vector<std::string> v) {
  supported_versions_ = std::move(v);
}

void ModuleInterface::add_supported_version(std::string v) {
  supported_versions_.emplace_back(std::move(v));
}

void ModuleInterface::clear_supported_versions() noexcept {
  supported_versions_.clear();
}
