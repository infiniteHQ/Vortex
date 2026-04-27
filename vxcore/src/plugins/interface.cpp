//
//  interface.cpp
//  Sources for plugins interfaces features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../include/plugins/interface.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

VORTEX_API void PluginInterface::add_plugin_function(const PluginFunction &function) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(function);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

VORTEX_API void PluginInterface::add_output_event(const PluginOutputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(const PluginInputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_content_browser_item_handler(const ItemHandlerInterface &handler) {
  m_item_handlers.push_back(std::make_shared<ItemHandlerInterface>(handler));
}

VORTEX_API std::vector<std::shared_ptr<ItemHandlerInterface>> PluginInterface::get_content_browser_item_handlers() {
  return m_item_handlers;
}

VORTEX_API std::string PluginInterface::cook_path(const std::string &path) {
  return this->get_path() + "/" + path;
}

VORTEX_API std::string PluginInterface::get_path() {
  return m_path;
}
VORTEX_API std::string PluginInterface::GetMainScriptPath() {
  return m_mainscript_path;
}

VORTEX_API void
PluginInterface::add_documentation(const std::string &section, const std::string &title, const std::string &path) {
  std::string topic = "plugin:" + this->m_name;
  vxe::add_documentation(topic, section, title, path);
}

VORTEX_API void PluginInterface::refresh_main_window() {
  if (!&Cherry::Application::Get()) {
    return;
  }

  // Remove potential old main window
  for (auto &window : Cherry::Application::Get().m_AppWindows) {
    if (m_main_window && window) {
      if (m_main_window->m_IdName == window->m_IdName) {
        Cherry::DeleteAppWindow(window);
      }
    }
  }

  // Add/Set the new one
  if (m_main_window) {
    Cherry::AddAppWindow(m_main_window);
  }
}

VORTEX_API void PluginInterface::set_main_window(const std::shared_ptr<Cherry::AppWindow> &win) {
  m_main_window = win;

  if (&Cherry::Application::Get()) {
    refresh_main_window();
  }
}

VORTEX_API void PluginInterface::add_plugin_render_instance(const std::shared_ptr<PluginRenderInstance> &renderInstance) {
  // Add the shared_ptr to the list of render instances
  this->m_render_instances.push_back(renderInstance);
}

VORTEX_API void PluginInterface::add_logo(const uint8_t *hexa, size_t size) {
  // Set the logo data and size
  this->m_logo = hexa;
  this->m_logo_size = size;
}

VORTEX_API void PluginInterface::add_logo(const std::string &relative_path) {
}

VORTEX_API void PluginInterface::add_output_event(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_output_event(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_output_event(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_output_event(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_input_event(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(foo, name);

  // Add the shared_ptr to the list of output events
  this->m_input_events.push_back(p_event);
}

VORTEX_API void PluginInterface::add_function(std::function<void()> foo, const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

VORTEX_API void PluginInterface::add_function(std::function<void(ArgumentValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

VORTEX_API void PluginInterface::add_function(std::function<void(ReturnValues &)> foo, const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

VORTEX_API void PluginInterface::add_function(
    std::function<void(ArgumentValues &, ReturnValues &)> foo,
    const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(foo, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

VORTEX_API std::shared_ptr<PluginInterface> PluginInterface::get_interface() {
  // Create a shared_ptr to a new PluginInterface object, copying the current
  // instance
  return std::make_shared<PluginInterface>(*this);
}

VORTEX_API std::shared_ptr<PluginInterface> PluginInterface::get_editor_plugin_by_name(const std::string &name) {
  auto ctx = vxe::get_current_context();

  for (auto ep : ctx->IO.ep) {
    if (name == ep->m_name) {
      // Return a shared_ptr to the editor plugin if found
      return ep;
    }
  }

  // Return nullptr if editor plugin with the specified name is not found
  return nullptr;
}

VORTEX_API void PluginInterface::execute_function(const std::string &name) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      ArgumentValues empty_args;
      ReturnValues empty_ret;
      foo->m_function(empty_args, empty_ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_function(const std::string &name, ReturnValues &ret) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      ArgumentValues empty_args;
      foo->m_function(empty_args, ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_function(const std::string &name, ArgumentValues &args) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      ReturnValues empty_ret;
      foo->m_function(args, empty_ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_function(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      foo->m_function(args, ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_input_event(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto event : this->m_input_events) {
    if (event->m_name == name) {
      event->m_function(args, ret);
      return;
    }
  }
}

VORTEX_API void PluginInterface::execute_output_event(const std::string &name, ArgumentValues &args, ReturnValues &ret) {
  for (auto event : this->m_output_events) {
    if (event->m_name == name) {
      event->m_function(args, ret);
      return;  // Exit after executing the output event
    }
  }
}

VORTEX_API void PluginInterface::log_info(const std::string &message) {
  vxe::log_info(this->m_name, message);
}

VORTEX_API void PluginInterface::log_warning(const std::string &message) {
  vxe::log_warn(this->m_name, message);
}

VORTEX_API void PluginInterface::log_error(const std::string &message) {
  vxe::log_error(this->m_name, message);
}

VORTEX_API void PluginInterface::log_fatal(const std::string &message) {
  vxe::log_fatal(this->m_name, message);
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
  for (auto dependency : this->m_dependencies) {
    // By default, dependency is not satisfied
    dependency->satisfied = false;

    // Check dependency type
    if (dependency->type == "em") {
      // Dependency is an entity plugin
      for (auto ctx_em : ctx->IO.em) {
        // Check if context entity plugin matches the dependency name
        if (ctx_em->m_name == dependency->name) {
          // Check if the context entity plugin version is among the supported
          // versions
          for (auto version : dependency->supported_versions) {
            if (ctx_em->m_version == version) {
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

  for (auto dependency : this->m_dependencies) {
    if (!dependency->satisfied) {
      allDependenciesSatisfied = false;
      std::string versions;
      for (auto version : dependency->supported_versions) {
        versions += "[" + version + "]";
      }
      unsatisfiedDependencies.push_back({ dependency->name, versions });
    }
  }

  for (auto version : this->m_supported_versions) {
    if (version == major) {
      std::cout << version << major << std::endl;
      isVersionCompatible = true;
    }
  }

  // Vector of needed dependencies (to run)
  std::vector<std::pair<std::string, std::string>> needToRunDependencies;
  for (auto dep : this->m_dependencies) {
    for (auto em : ctx->IO.em) {
      if (em->m_name == dep->name) {
        std::string versions;
        for (auto version : dep->supported_versions) {
          versions += "[" + version + "]";
        }

        if (em->m_state != "running") {
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
    this->m_state = "failed";
    return;
  }

  if (!isVersionCompatible) {
    this->log_error(
        "This plugin isn't compatible with this version of Vortex (\"" + major + "\") ! Is incompatible with \"" +
        this->m_name + "\" supported version(s).");

    this->m_state = "failed";
    return;
  }

  if (!needToRunDependencies.empty()) {
    this->log_error("Dependencies of this plugins are not running !");

    for (auto notRunningDeps : needToRunDependencies) {
      this->log_error("Please run : " + notRunningDeps.first + " with version(s) " + notRunningDeps.second);
    }
    this->m_state = "failed";
    return;
  }

  auto &engine = vxe::script::get_scripting_engine();
  engine.load_file_for_plugin(this->m_mainscript_path, this->shared_from_this());

  this->m_state = "running";
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

  for (auto em : ctx->IO.em) {
    if (em->m_state == "running") {
      for (auto dependency : em->m_dependencies) {
        if (dependency->name == this->m_name) {
          for (auto version : dependency->supported_versions) {
            if (this->m_version == version) {
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
    this->log_error("Cannot stop \"" + this->m_name + "\" ! ");
    for (auto dep : deps) {
      this->log_error("This plugin is used by \"" + dep.first + "\" with needed versions : " + dep.second + " ! ");
    }
  } else {
    m_item_handlers.clear();
    m_lua_handlers.clear();
    this->destroy();
    this->m_state = "stopped";
  }
}

VORTEX_API void PluginInterface::ResetPlugin() {
  m_item_handlers.clear();
}

VORTEX_API void PluginInterface::call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret) {
  vxe::call_output_event(event_name, args, ret, this->m_name);
}

VORTEX_API void PluginInterface::call_input_event(
    const std::string &plugin_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret) {
  vxe::call_input_event(plugin_name, event_name, args, ret, this->m_name);
}

VORTEX_API void PluginInterface::check_version() {
}

std::vector<std::shared_ptr<PluginRenderInstance>> PluginInterface::get_plugin_render_instances() {
  return this->m_render_instances;
};

VORTEX_API void PluginInterface::add_content_browser_item_identifier(const ItemIdentifierInterface &item) {
  m_item_identifiers.push_back(std::make_shared<ItemIdentifierInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemIdentifierInterface>> &PluginInterface::get_content_browser_item_identifiers() {
  return m_item_identifiers;
}

VORTEX_API void PluginInterface::add_content_browser_item_creator(const ItemCreatorInterface &item) {
  m_item_creators.push_back(std::make_shared<ItemCreatorInterface>(item));
}

VORTEX_API std::vector<std::shared_ptr<ItemCreatorInterface>> &PluginInterface::get_content_browser_item_creators() {
  return m_item_creators;
}

VORTEX_API void PluginInterface::set_credits_file(const std::string &file_path) {
  vxe::set_credits_file(this->m_proper_name + "(" + this->m_name + ")", file_path);
}
