# Module C++ API Reference

This page lists every function available in the `ModuleInterface` class, used to program Vortex Editor modules in C++.

All these functions are members of `ModuleInterface`. You can use them in two ways:

- **From your module**, inside the overridden functions (`execute`, `init_ui`...), by calling them directly.
- **From an instance of `ModuleInterface`**, for example the one stored in your module context, or one retrieved with `ModuleInterface::get_editor_module_by_name`.

```cpp
class MyModule : public ModuleInterface {
 public:
  void execute() override {
    log_info("Module started");
  }
};
```

```cpp
namespace MyModule {
struct Context {
  std::shared_ptr<ModuleInterface> m_interface;
};
}

std::string MyModule::get_path(const std::string &path) {
  return get_current_context()->m_interface->cook_path(path);
}
```

```cpp
auto other = ModuleInterface::get_editor_module_by_name("other_module");
if (other)
  other->log_info("Called from outside");
```

> The examples of this page are written as if they were called from inside the module. From an instance, call them on the instance (`m_interface->log_info(...)`).

## Summary

| Category | Functions |
|---|---|
| Main functions | `execute`, `init_ui`, `destroy` |
| Lifecycle | `start`, `stop`, `reset_module` |
| Logs | `log_info`, `log_warning`, `log_error`, `log_fatal` |
| Arguments & return values | `ArgumentValues`, `ReturnValues` |
| Module functions | `add_function`, `add_module_function`, `execute_function` |
| Events | `add_input_event`, `add_output_event`, `execute_input_event`, `execute_output_event`, `call_input_event`, `call_output_event` |
| Event classes | `ModuleInputEvent`, `ModuleOutputEvent`, `trigger_happening` |
| Content browser | `add_content_browser_item_handler`, `add_content_browser_item_identifier`, `add_content_browser_item_creator` |
| Toolbar | `add_toolbar_handler`, `toolbar_main_title`, `set_toolbar_main_logo_path`, `add_toolbar_topic_logo` |
| Windows | `set_main_window`, `refresh_main_window` |
| Documentation | `add_documentation` |
| Customization | `add_logo`, `set_credits_file` |
| Dependencies & versions | `add_dependency`, `add_supported_version`, `add_contributor`, `check_dependencies` |
| Utilities | `cook_path`, `get_editor_module_by_name` |
| Getters & setters | module properties (`name`, `version`, `path`...) |

---

## Main functions

These functions are virtual. Override them in your module to define its behavior.

### execute

Main entry point of the module. Called by `start()` once the dependencies and the version are validated.

```cpp
virtual void execute();
```

```cpp
void execute() override {
  log_info("Hello from my module");
}
```

---

### init_ui

Called when the UI part of the module starts. Override it to initialize the user interface of the module.

```cpp
virtual void init_ui();
```

```cpp
void init_ui() override {
  add_toolbar_topic_logo("my_topic", cook_path("assets/topic.png"));
}
```

---

### destroy

Called by `stop()` to clean up the resources of the module.

```cpp
virtual void destroy();
```

```cpp
void destroy() override {
  log_info("Module stopped");
}
```

---

## Lifecycle

### start

Starts the module. It checks the dependencies, the compatibility with the current Vortex version and that the dependencies are running.

If everything is valid, `execute()` is called and the state becomes `"running"`. Otherwise, errors are logged and the state becomes `"failed"`.

Modules with `auto_exec` enabled are started automatically when Vortex bootstraps the modules.

```cpp
void start();
```

```cpp
auto module = ModuleInterface::get_editor_module_by_name("my_module");
if (module)
  module->start();
```

---

### stop

Stops the module. `destroy()` is called and the state becomes `"stopped"`.

If another running module depends on this one, the module is not stopped and an error is logged.

```cpp
void stop();
```

```cpp
auto module = ModuleInterface::get_editor_module_by_name("my_module");
if (module)
  module->stop();
```

---

### reset_module

Clears the content browser item handlers and item identifiers of the module.

```cpp
void reset_module();
```

```cpp
reset_module();
```

---

## Logs

The module name is used as the log source.

### log_info

Logs an information message.

```cpp
void log_info(const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `message` | `const std::string &` | The message to log. |

```cpp
log_info("Cool info");
```

---

### log_warning

Logs a warning message.

```cpp
void log_warning(const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `message` | `const std::string &` | The message to log. |

```cpp
log_warning("Be careful");
```

---

### log_error

Logs an error message.

```cpp
void log_error(const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `message` | `const std::string &` | The message to log. |

```cpp
log_error("Unable to do this or that");
```

---

### log_fatal

Logs a fatal error message.

```cpp
void log_fatal(const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `message` | `const std::string &` | The message to log. |

```cpp
log_fatal("File unrecoverable");
```

---

## Arguments and return values

Functions and events exchange data with two objects: `ArgumentValues` for the arguments received, and `ReturnValues` for the value returned.

Both work the same way. The data is stored as a string, and is oriented around JSON to easily transfer complex types or large amounts of data. The default value is `"null"`.

| Function | Description |
|---|---|
| `std::string get()` | Returns the value as a string. |
| `nlohmann::json get_json()` | Parses the value and returns it as JSON. |
| `T get_json<T>(const std::string &val)` | Parses the string `val` and converts it to the type `T`. |
| `void set(const std::string &val)` | Sets the value from a string. |
| `void set_json(const nlohmann::json &val)` | Sets the value from a JSON object. |

```cpp
ArgumentValues args;
args.set_json({ { "name", "dark" } });

std::string name = args.get_json()["name"];

ReturnValues ret;
ret.set("ok");
```

---

## Module functions

Module functions give the Vortex abstraction and features of the module. They can be executed by name from the module.

### add_function

Adds a function to the module. Four signatures are available depending on whether the function uses arguments and a return value.

```cpp
void add_function(std::function<void()> foo, const std::string &name);
void add_function(std::function<void(ArgumentValues &)> foo, const std::string &name);
void add_function(std::function<void(ReturnValues &)> foo, const std::string &name);
void add_function(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `foo` | `std::function` | Function to add. Can receive the arguments, the return value, both or none. |
| `name` | `const std::string &` | Name used to execute the function. |

```cpp
add_function([](ArgumentValues &args, ReturnValues &ret) {
  ret.set("Hello " + args.get());
}, "greet");
```

---

### add_module_function

Adds a ready-to-use `ModuleFunction` to the module.

```cpp
void add_module_function(const ModuleFunction &function);
```

| Parameter | Type | Description |
|---|---|---|
| `function` | `const ModuleFunction &` | The function to add. |

```cpp
std::function<void()> ping = [this]() { log_info("ping"); };
add_module_function(ModuleFunction(ping, "ping"));
```

---

### execute_function

Executes a function of the module by its name. Nothing happens if the function does not exist.

```cpp
void execute_function(const std::string &name);
void execute_function(const std::string &name, ArgumentValues &args);
void execute_function(const std::string &name, ReturnValues &ret);
void execute_function(const std::string &name, ArgumentValues &args, ReturnValues &ret);
```

| Parameter | Type | Description |
|---|---|---|
| `name` | `const std::string &` | Name of the function to execute. |
| `args` | `ArgumentValues &` (optional) | Arguments sent to the function. |
| `ret` | `ReturnValues &` (optional) | Receives the value returned by the function. |

```cpp
ArgumentValues args;
args.set("World");

ReturnValues ret;
execute_function("greet", args, ret);

log_info(ret.get());
```

---

## Events

- An **input event** is like an event handler declared by a module or a plugin, that can be called from outside.
- An **output event** is a broadcast from one module, plugin or the core to all listeners.

### add_input_event

Adds an input event handler to the module. Four signatures are available depending on whether the handler uses arguments and a return value. You can also give a [`ModuleInputEvent`](#moduleinputevent) directly.

If the handler only receives the arguments, the returned value is always `null`.

```cpp
void add_input_event(std::function<void()> foo, const std::string &name);
void add_input_event(std::function<void(ArgumentValues &)> foo, const std::string &name);
void add_input_event(std::function<void(ReturnValues &)> foo, const std::string &name);
void add_input_event(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
void add_input_event(const ModuleInputEvent &event);
```

| Parameter | Type | Description |
|---|---|---|
| `foo` | `std::function` | Handler executed when the event is called. Can receive the arguments, the return value, both or none. |
| `name` | `const std::string &` | Name of the input event. |
| `event` | `const ModuleInputEvent &` | A ready-to-use input event. |

```cpp
add_input_event([this](ArgumentValues &args, ReturnValues &ret) {
  log_info("Theme : " + args.get_json()["name"].get<std::string>());
  ret.set_json({ { "ok", true } });
}, "set_theme");
```

---

### add_output_event

Adds a handler to an output event. If Vortex or another module/plugin triggers this event, the handler is executed. Four signatures are available depending on whether the handler uses arguments and a return value. You can also give a [`ModuleOutputEvent`](#moduleoutputevent) directly.

If the handler only receives the arguments, the returned value is always `null`.

```cpp
void add_output_event(std::function<void()> foo, const std::string &name);
void add_output_event(std::function<void(ArgumentValues &)> foo, const std::string &name);
void add_output_event(std::function<void(ReturnValues &)> foo, const std::string &name);
void add_output_event(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
void add_output_event(const ModuleOutputEvent &event);
```

| Parameter | Type | Description |
|---|---|---|
| `foo` | `std::function` | Handler executed when the event is triggered. Can receive the arguments, the return value, both or none. |
| `name` | `const std::string &` | Name of the output event to listen to. |
| `event` | `const ModuleOutputEvent &` | A ready-to-use output event. |

```cpp
add_output_event([this]() {
  log_info("Save all event triggered !");
}, "save_all");
```

---

### call_input_event

Calls a specific input event of a module, a plugin or the core.

```cpp
void call_input_event(
    const std::string &module_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret);
```

| Parameter | Type | Description |
|---|---|---|
| `module_name` | `const std::string &` | Name of the module, plugin or core that owns the input event. |
| `event_name` | `const std::string &` | Name of the input event to call. |
| `args` | `ArgumentValues &` | Arguments sent to the event. |
| `ret` | `ReturnValues &` | Receives the value returned by the handler. |

```cpp
ArgumentValues args;
ReturnValues ret;
call_input_event("vortex", "spawn_content_browser", args, ret);
```

---

### call_output_event

Broadcasts an output event to everyone having a handler for it.

```cpp
void call_output_event(const std::string &event_name, ArgumentValues &args, ReturnValues &ret);
```

| Parameter | Type | Description |
|---|---|---|
| `event_name` | `const std::string &` | Name of the output event to broadcast. |
| `args` | `ArgumentValues &` | Arguments sent to the listeners. |
| `ret` | `ReturnValues &` | Receives the value returned by the handlers. |

```cpp
ArgumentValues args;
args.set("path");

ReturnValues ret;
call_output_event("just_save", args, ret);
```

---

### execute_input_event

Executes an input event handler declared by this module, directly by its name. Nothing happens if the event does not exist.

```cpp
void execute_input_event(const std::string &name, ArgumentValues &args, ReturnValues &ret);
```

| Parameter | Type | Description |
|---|---|---|
| `name` | `const std::string &` | Name of the input event of this module. |
| `args` | `ArgumentValues &` | Arguments sent to the handler. |
| `ret` | `ReturnValues &` | Receives the value returned by the handler. |

```cpp
ArgumentValues args;
args.set_json({ { "name", "dark" } });

ReturnValues ret;
execute_input_event("set_theme", args, ret);
```

---

### execute_output_event

Executes an output event handler declared by this module, directly by its name. Nothing happens if the event does not exist.

```cpp
void execute_output_event(const std::string &name, ArgumentValues &args, ReturnValues &ret);
```

| Parameter | Type | Description |
|---|---|---|
| `name` | `const std::string &` | Name of the output event of this module. |
| `args` | `ArgumentValues &` | Arguments sent to the handler. |
| `ret` | `ReturnValues &` | Receives the value returned by the handler. |

```cpp
ArgumentValues args;
ReturnValues ret;
execute_output_event("save_all", args, ret);
```

---

## Event classes

`ModuleInputEvent` and `ModuleOutputEvent` represent the events registered in a module. You can build them yourself and give them to `add_input_event` and `add_output_event`, or get the registered ones with `input_events()` and `output_events()`.

### ModuleInputEvent

An input event, that can be called from outside.

```cpp
ModuleInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
ModuleInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
ModuleInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
ModuleInputEvent(std::function<void()> foo, const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `foo` | `std::function` | Handler executed when the event is called. Can receive the arguments, the return value, both or none. |
| `name` | `const std::string &` | Name of the input event. |

**Members**

| Member | Type | Description |
|---|---|---|
| `name_` | `std::string` | Name of the event. |
| `description_` | `std::string` | Description of the event. |
| `function_` | `std::function<void(ArgumentValues &, ReturnValues &)>` | The handler, whatever the signature given in the constructor. |
| `happenings_` | `std::vector<std::shared_ptr<ModuleInputEventHappening>>` | The happenings recorded with `trigger_happening`. |

```cpp
add_input_event(ModuleInputEvent(
    [this](ArgumentValues &args, ReturnValues &ret) { ret.set("ok"); },
    "set_theme"));
```

---

### ModuleOutputEvent

An output event, emitted with custom parameters to the listeners.

```cpp
ModuleOutputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
ModuleOutputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
ModuleOutputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
ModuleOutputEvent(std::function<void()> foo, const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `foo` | `std::function` | Handler executed when the event is triggered. Can receive the arguments, the return value, both or none. |
| `name` | `const std::string &` | Name of the output event. |

**Members**

| Member | Type | Description |
|---|---|---|
| `name_` | `std::string` | Name of the event. |
| `function_` | `std::function<void(ArgumentValues &, ReturnValues &)>` | The handler, whatever the signature given in the constructor. |
| `happenings_` | `std::vector<std::shared_ptr<ModuleOutputEventHappening>>` | The happenings recorded with `trigger_happening`. |

```cpp
add_output_event(ModuleOutputEvent(
    [this]() { log_info("Save all event triggered !"); },
    "save_all"));
```

---

### trigger_happening

Available on `ModuleInputEvent` and `ModuleOutputEvent`. Records a happening on the event (something that happened while the event was running) and logs it in the logs utility under the source `Modules Events`.

```cpp
void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);
```

| Parameter | Type | Description |
|---|---|---|
| `trigger_name` | `const std::string &` | Name of what triggered the happening. |
| `state` | `HappeningState` | Level of the happening. It also defines the level of the log. |
| `log` | `const std::string &` | Message of the happening. |

**HappeningState**

| Value | Log level |
|---|---|
| `HappeningState::INFO` | Information |
| `HappeningState::WARNING` | Warning |
| `HappeningState::ERR` | Error |
| `HappeningState::FATAL` | Fatal error |

Each recorded happening (`ModuleInputEventHappening` or `ModuleOutputEventHappening`) contains the `trigger_name`, the `state`, the `log` and a `timestamp`.

```cpp
for (auto &event : input_events()) {
  if (event->name_ == "set_theme")
    event->trigger_happening("my_module", HappeningState::WARNING, "Theme not found");
}
```

---

## Content browser

The module allows you to add handlers, identifiers and creators to customize the content browser.

### add_content_browser_item_handler

Adds a custom handler for a specific type of item. You can handle generic items by extension (like `png`, `json`), or special module items.

```cpp
void add_content_browser_item_handler(const ItemHandlerInterface &handler);
std::vector<std::shared_ptr<ItemHandlerInterface>> &get_content_browser_item_handlers();
```

| Parameter | Type | Description |
|---|---|---|
| `handler` | `const ItemHandlerInterface &` | The handler to add. |

**ItemHandlerInterface**

```cpp
ItemHandlerInterface(
    const std::string &type,
    std::function<void(const std::string &)> handler,
    const std::string &title,
    const std::string &description = "",
    const std::string &logo = "");
```

| Parameter | Type | Description |
|---|---|---|
| `type` | `const std::string &` | Item type to handle: a file extension (`png`) or a module item type (`module:item_type`). |
| `handler` | `std::function<void(const std::string &)>` | Function executed when the handler is used. Receives the path of the item. |
| `title` | `const std::string &` | Name of the action shown in the content browser. |
| `description` | `const std::string &` (optional) | Description of the action. |
| `logo` | `const std::string &` (optional) | Path of the icon. |

```cpp
add_content_browser_item_handler(ItemHandlerInterface(
    "png",
    [this](const std::string &path) { log_info("Opening " + path); },
    "Open picture",
    "Open picture in photo viewer",
    cook_path("icons/image.png")));
```

---

### add_content_browser_item_identifier

Adds an identifier, used to recognize a special type of item (like a folder with a specific structure) in the content browser.

```cpp
void add_content_browser_item_identifier(const ItemIdentifierInterface &item);
std::vector<std::shared_ptr<ItemIdentifierInterface>> &get_content_browser_item_identifiers();
```

| Parameter | Type | Description |
|---|---|---|
| `item` | `const ItemIdentifierInterface &` | The identifier to add. |

**ItemIdentifierInterface**

```cpp
ItemIdentifierInterface(
    bool (*detect_function)(const std::string &path),
    const std::string &name,
    const std::string &description,
    const std::string &line_color,
    const std::string &logo_path = "",
    const std::string &bg_image_path = "");
```

| Parameter | Type | Description |
|---|---|---|
| `detect_function` | `bool (*)(const std::string &path)` | Function receiving the path of an item. Returns `true` if the item is recognized. It must be a plain function (or a lambda without captures). |
| `name` | `const std::string &` | Name of the item type. |
| `description` | `const std::string &` | Description of the item type. |
| `line_color` | `const std::string &` | Color of the item line, as a hex string (`"#343434"`). |
| `logo_path` | `const std::string &` (optional) | Path of the logo. |
| `bg_image_path` | `const std::string &` (optional) | Path of the background image. |

```cpp
static bool is_my_item(const std::string &path) {
  return fs::exists(path + "/config.txt");
}

add_content_browser_item_identifier(ItemIdentifierInterface(
    is_my_item,
    "My item",
    "A folder containing a config file",
    "#4a90e2",
    cook_path("icons/item.png")));
```

---

### add_content_browser_item_creator

Adds a creator, used to create new items from the content browser.

```cpp
void add_content_browser_item_creator(const ItemCreatorInterface &item);
std::vector<std::shared_ptr<ItemCreatorInterface>> &get_content_browser_item_creators();
```

| Parameter | Type | Description |
|---|---|---|
| `item` | `const ItemCreatorInterface &` | The creator to add. |

**ItemCreatorInterface**

```cpp
ItemCreatorInterface(
    std::function<void(const std::string &path)> function,
    const std::string &name,
    const std::string &description,
    const std::string &line_color = "#343434",
    const std::string &logo_path = "");
```

| Parameter | Type | Description |
|---|---|---|
| `function` | `std::function<void(const std::string &path)>` | Function executed to create the item. Receives the path where the item is created. |
| `name` | `const std::string &` | Name of the item to create. |
| `description` | `const std::string &` | Description of the item. |
| `line_color` | `const std::string &` (optional) | Color of the item line, as a hex string. Default is `"#343434"`. |
| `logo_path` | `const std::string &` (optional) | Path of the logo. |

```cpp
add_content_browser_item_creator(ItemCreatorInterface(
    [this](const std::string &path) { log_info("Create item in " + path); },
    "My item",
    "Create a new item",
    "#4a90e2",
    cook_path("icons/item.png")));
```

---

## Toolbar

### add_toolbar_handler

Adds a handler to the modules section of the main toolbar. This also enables the modules section on the toolbar.

```cpp
void add_toolbar_handler(const ToolbarHandlerInterface &handler, const std::vector<std::string> &topic = {});
std::vector<std::shared_ptr<ToolbarHandlerInterface>> &get_toolbar_handlers();
```

| Parameter | Type | Description |
|---|---|---|
| `handler` | `const ToolbarHandlerInterface &` | The handler to add. |
| `topic` | `const std::vector<std::string> &` (optional) | Currently unused. Use the `topic` of the handler instead. |

**ToolbarHandlerInterface**

```cpp
ToolbarHandlerInterface(
    std::function<void()> handler,
    const std::string &title,
    const std::string &description = "",
    const std::string &topic = "",
    const std::string &logo = "");
```

| Parameter | Type | Description |
|---|---|---|
| `handler` | `std::function<void()>` | Function executed when the entry is clicked. |
| `title` | `const std::string &` | Title of the entry. |
| `description` | `const std::string &` (optional) | Description of the entry. |
| `topic` | `const std::string &` (optional) | Topic where the entry is displayed. |
| `logo` | `const std::string &` (optional) | Path of the logo. |

```cpp
add_toolbar_handler(ToolbarHandlerInterface(
    [this]() { log_info("Clicked"); },
    "Open my tool",
    "Open the tool of my module",
    "my_topic",
    cook_path("icons/tool.png")));
```

---

### toolbar_main_title

Gets or sets the main title of the module in the toolbar.

```cpp
const std::string &toolbar_main_title() const noexcept;
void toolbar_main_title(std::string v);
```

| Parameter | Type | Description |
|---|---|---|
| `v` | `std::string` | The new title. |

```cpp
toolbar_main_title("My module");
```

---

### set_toolbar_main_logo_path

Sets the main logo of the module in the toolbar. Use `get_toolbar_main_logo_path` to read it.

```cpp
void set_toolbar_main_logo_path(std::string v);
const std::string &get_toolbar_main_logo_path() const noexcept;
```

| Parameter | Type | Description |
|---|---|---|
| `v` | `std::string` | Path of the logo. |

```cpp
set_toolbar_main_logo_path(cook_path("assets/logo.png"));
```

---

### add_toolbar_topic_logo

Sets the logo of a toolbar topic. Use `get_toolbar_topic_logo` to read it (returns an empty string if the topic has no logo).

```cpp
void add_toolbar_topic_logo(const std::string &id, const std::string &v);
const std::string &get_toolbar_topic_logo(const std::string &id) const noexcept;
```

| Parameter | Type | Description |
|---|---|---|
| `id` | `const std::string &` | Identifier of the topic. |
| `v` | `const std::string &` | Path of the logo. |

```cpp
add_toolbar_topic_logo("my_topic", cook_path("assets/topic.png"));
```

---

## Windows

### set_main_window

Sets the main window of the module. If the application is running, the window is refreshed immediately.

```cpp
void set_main_window(const std::shared_ptr<Cherry::AppWindow> &win);
```

| Parameter | Type | Description |
|---|---|---|
| `win` | `const std::shared_ptr<Cherry::AppWindow> &` | The window to use as main window. |

```cpp
set_main_window(my_window->get_app_window());
```

---

### refresh_main_window

Removes the old main window from the application and adds the current one again.

```cpp
void refresh_main_window();
```

```cpp
refresh_main_window();
```

---

## Documentation

### add_documentation

Adds a documentation page to the module. It is registered under the topic `module:<module_name>`.

```cpp
void add_documentation(const std::string &section, const std::string &title, const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `section` | `const std::string &` | Section where the page is displayed. |
| `title` | `const std::string &` | Title of the page. |
| `path` | `const std::string &` | Path of the documentation file. |

```cpp
add_documentation("Getting started", "Introduction", cook_path("docs/intro.md"));
```

---

## Customization

### add_logo

Sets the logo of the module, from raw data.

```cpp
void add_logo(const uint8_t *hexa, size_t size);
```

| Parameter | Type | Description |
|---|---|---|
| `hexa` | `const uint8_t *` | Pointer to the logo data. |
| `size` | `size_t` | Size of the logo data in bytes. |

```cpp
add_logo(logo_data, sizeof(logo_data));
```

---

### set_credits_file

Sets the credits file of the module. It is registered under `proper_name(name)`.

```cpp
void set_credits_file(const std::string &file_path);
```

| Parameter | Type | Description |
|---|---|---|
| `file_path` | `const std::string &` | Path of the credits file. |

```cpp
set_credits_file(cook_path("assets/CREDITS"));
```

---

## Dependencies & versions

### add_dependency

Adds a dependency to the module. The module can't start until all its dependencies are satisfied and running.

```cpp
void add_dependency(std::shared_ptr<ModuleInterfaceDep> v);
void clear_dependencies() noexcept;
```

| Parameter | Type | Description |
|---|---|---|
| `v` | `std::shared_ptr<ModuleInterfaceDep>` | The dependency to add. |

**ModuleInterfaceDep**

| Field | Type | Description |
|---|---|---|
| `type` | `std::string` | Type of the dependency. Only `"em"` (editor module) is currently checked. |
| `name` | `std::string` | Name of the module needed. |
| `supported_versions` | `std::vector<std::string>` | Versions of the module that are accepted. |
| `satisfied` | `bool` | Updated by `check_dependencies()`. `true` if a matching module is loaded. |

```cpp
auto dep = std::make_shared<ModuleInterfaceDep>();
dep->type = "em";
dep->name = "other_module";
dep->supported_versions = { "1.0.0" };
add_dependency(dep);
```

---

### add_supported_version

Adds a version of Vortex supported by the module. The version of Vortex is compared with its first two numbers (`X.Y`). For the current version of Vortex (`1.0`), the supported version to declare is `"1.0"`.

```cpp
void add_supported_version(std::string v);
void clear_supported_versions() noexcept;
```

| Parameter | Type | Description |
|---|---|---|
| `v` | `std::string` | Supported version of Vortex (`X.Y`). |

```cpp
add_supported_version("1.0");
```

---

### add_contributor

Adds a contributor to the module.

```cpp
void add_contributor(std::string v);
void clear_contributors() noexcept;
```

| Parameter | Type | Description |
|---|---|---|
| `v` | `std::string` | Name of the contributor. |

```cpp
add_contributor("Jane Doe");
```

---

### check_dependencies

Updates the `satisfied` state of each dependency, based on the modules currently loaded. It is called automatically by `start()`.

```cpp
void check_dependencies();
```

```cpp
check_dependencies();
```

---

## Utilities

### cook_path

Returns the path of the module followed by a relative path.

```cpp
std::string cook_path(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path appended to the module path. |

**Returns:** `module_path/path`.

```cpp
std::string assets = cook_path("assets");
```

---

### get_editor_module_by_name

Static function that searches for an editor module by its name.

```cpp
static std::shared_ptr<ModuleInterface> get_editor_module_by_name(const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `name` | `const std::string &` | Name of the module to find. |

**Returns:** the module, or `nullptr` if it is not found.

```cpp
auto module = ModuleInterface::get_editor_module_by_name("other_module");
if (module)
  module->log_info("Found");
```

---

## Getters & setters

Each property of the module can be read with its getter and changed with its setter. The setter has the same name as the getter, with the value as parameter.

| Property | Type | Description |
|---|---|---|
| `state` | `std::string` | Current state of the module (`"running"`, `"stopped"`, `"failed"`, `"unknow"` by default). |
| `datapath` | `std::string` | Data folder of the module (`<project>/.vx/data/<module_name>`). Set automatically when Vortex bootstraps the modules, and created if it does not exist. |
| `type` | `std::string` | Type of the module. |
| `proper_name` | `std::string` | Display name of the module. |
| `name` | `std::string` | Unique name of the module. |
| `version` | `std::string` | Version of the module. |
| `path` | `std::string` | Path of the module. |
| `binary_path` | `std::string` | Path of the binary of the module. |
| `author` | `std::string` | Author of the module. |
| `group` | `std::string` | Group of the module. |
| `picture` | `std::string` | Path of the picture of the module. |
| `logo_path` | `std::string` | Path of the logo of the module. |
| `description` | `std::string` | Description of the module. |
| `auto_exec` | `bool` | If `true`, the module is started automatically when Vortex bootstraps the modules. |
| `contributors` | `std::vector<std::string>` | Contributors of the module. |
| `dependencies` | `std::vector<std::shared_ptr<ModuleInterfaceDep>>` | Dependencies of the module. |
| `supported_versions` | `std::vector<std::string>` | Vortex versions supported by the module. |

```cpp
name("my_module");
proper_name("My module");
version("1.0.0");
auto_exec(true);

log_info(name() + " " + version());
```

The following properties are read-only. They return the elements registered in the module.

| Getter | Returns |
|---|---|
| `functions()` | Functions added with `add_function`. |
| `output_events()` | `ModuleOutputEvent` added with `add_output_event`. |
| `input_events()` | `ModuleInputEvent` added with `add_input_event`. |
| `item_handlers()` | Content browser item handlers. |
| `item_identifiers()` | Content browser item identifiers. |
| `item_creators()` | Content browser item creators. |

```cpp
for (auto &event : input_events())
  log_info("Input event : " + event->name_);
```
