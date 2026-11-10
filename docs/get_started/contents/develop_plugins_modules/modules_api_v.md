# Vortex API Reference

This page lists the functions of the Vortex API. They are in the `vxe` namespace and are available from anywhere: the core, but also a module or a plugin.

```cpp
#include <vortex.h>
#include <vortex_internals.h>
```

| Macro | Value | Description |
|---|---|---|
| `VORTEX_VERSION` | `"1.0"` | Version of Vortex. |
| `VORTEX_VERSION_NUM` | `1000` | Version of Vortex as an integer (`XYYZZ`), usable in `#if` conditions. |

## Summary

| Category | Content |
|---|---|
| [Context](#context) | `create_context`, `destroy_context`, `get_current_context`, `set_current_context`, `initialize` |
| [VxContext](#vxcontext) | Members of the context and structures |
| [VortexNet](#vortexnet) | `CheckNet`, `GET`, `POST` |
| [Arguments and return values](#arguments-and-return-values) | `ArgumentValues`, `ReturnValues` |
| [Logs](#logs) | `log_info`, `log_warn`, `log_error`, `log_fatal`, loggers |
| [Events](#events) | `call_input_event`, `call_output_event`, core events |
| [Environment](#environment) | Environment, platform, net permission |
| [Project](#project) | Project initialization, creation and update |
| [Session](#session) | Sessions of the user |
| [Content browser](#content-browser) | Items, pools, custom folders, clipboard, files |
| [Themes](#themes) | Themes of the interface |
| [Customizations](#customizations) | Edit menu, custom menus, toolbar |
| [Credits](#credits) | Credits of the topics |
| [Documentation](#documentation) | Embedded documentation |
| [Scripting](#scripting) | Lua scripting engine |
| [Files and JSON](#files-and-json) | Search, copy, creation, JSON |
| [Web and installation](#web-and-installation) | Download, checksum, install of releases |
| [Utilities](#utilities) | Timestamp, base64, strings, commands |

---

# Context

The context is a `VxContext`. It holds the state of Vortex (the loaded modules and plugins, the current project, the logs...).

```cpp
auto ctx = vxe::get_current_context();
if (ctx)
  vxe::log_info("My module", ctx->projectPath.string());
```

### vxe::create_context

Creates a new context and initializes it with `initialize()`.

If no context was set before, the new context becomes the current context. If a context was already set, it stays the current context.

```cpp
std::shared_ptr<VxContext> vxe::create_context();
```

**Returns:** the new context.

```cpp
auto ctx = vxe::create_context();
```

---

### vxe::destroy_context

Removes a context from the current context. The context is only released when nothing else holds it.

If the context is the current one, there is no current context anymore. If it is another one, the current context stays unchanged.

```cpp
void vxe::destroy_context(std::shared_ptr<VxContext> ctx = nullptr);
```

| Parameter | Type | Description |
|---|---|---|
| `ctx` | `std::shared_ptr<VxContext>` (optional) | The context to destroy. If not given, the current context is used. |

```cpp
vxe::destroy_context();
```

---

### vxe::get_current_context

Returns the current context.

```cpp
std::shared_ptr<VxContext> vxe::get_current_context();
```

**Returns:** the current context, or `nullptr` if there is none.

```cpp
auto ctx = vxe::get_current_context();
```

---

### vxe::set_current_context

Sets the current context. Only a weak reference is kept, so the context must be held elsewhere (for example by the `shared_ptr` returned by `create_context`).

```cpp
void vxe::set_current_context(std::shared_ptr<VxContext> ctx);
```

| Parameter | Type | Description |
|---|---|---|
| `ctx` | `std::shared_ptr<VxContext>` | The context to use. `nullptr` removes the current context. |

```cpp
vxe::set_current_context(ctx);
```

---

### vxe::initialize

Marks the current context as initialized. It requires a current context. It is called by `create_context`.

```cpp
void vxe::initialize();
```

```cpp
vxe::initialize();
```

---

## VxContext

The members of the context, accessed with `ctx->`.

```cpp
auto ctx = vxe::get_current_context();
```

### Project

The informations of the current project. They are read from the `project` section of the `vortex.config` file of the project.

| Member | Type | Description |
|---|---|---|
| `name` | `std::string` | Name of the project. |
| `label` | `std::string` | Label of the project. Same as `name`. |
| `author` | `std::string` | Author of the project. |
| `description` | `std::string` | Description of the project. |
| `type` | `std::string` | Type of the project. |
| `version` | `std::string` | Version of the project, read when the project is initialized. |
| `project_version` | `std::string` | Version of the project, read when the project informations are refreshed. |
| `compatibleWith` | `std::string` | Version of Vortex the project is compatible with. |
| `tags` | `std::string` | Tags of the project. |
| `website` | `std::string` | Website of the project. |
| `support_contact` | `std::string` | Support contact of the project. |
| `logo_path` | `std::string` | Path of the logo (`logoPath` in `vortex.config`). |
| `banner_path` | `std::string` | Path of the banner (`bannerPath` in `vortex.config`). |
| `startup_script` | `std::string` | Path of the Lua script executed at startup. |
| `copyright_file` | `std::string` | Path of the copyright file. |
| `license_file` | `std::string` | Path of the license file. |
| `readme_file` | `std::string` | Path of the readme file. |
| `requirements_file` | `std::string` | Path of the requirements file. |
| `code_of_conduct_file` | `std::string` | Path of the code of conduct file. |
| `security_file` | `std::string` | Path of the security file. |
| `root_content_path` | `std::string` | Root folder of the contents, relative to the project path. |
| `include_system_templates` | `bool` | If `true`, the templates installed on the system are available in the project. |
| `projectPath` | `fs::path` | Path of the project. It is the current working directory when the project is initialized. |
| `projectDataPath` | `fs::path` | Path of the root content folder. It is the project path if `root_content_path` is not set or does not exist. |

```cpp
vxe::log_info("My module", "Project : " + ctx->name + " (" + ctx->version + ")");
```

---

### Platform

| Member | Type | Description |
|---|---|---|
| `platform_vendor` | `PlatformVendor` | Operating system, as an enum (`Linux`, `Windows`, `Macos`). |
| `platform` | `std::string` | Operating system, as a string (`"windows"`, `"macos"`, `"linux"`, `"freebsd"` or `"unknown"`). |
| `arch` | `std::string` | Architecture (`"x86_64"`, `"x86"`, `"arm64"`, `"arm"`, `"riscv"`, `"ppc64"` or `"unknown"`). |

```cpp
if (ctx->platform_vendor == PlatformVendor::Linux)
  vxe::log_info("My module", "Running on Linux (" + ctx->arch + ")");
```

---

### Logs

| Member | Type | Description |
|---|---|---|
| `logger` | `bool` | Enables the logs. When `false`, the log functions do nothing. |
| `logger_registering` | `bool` | If `true` (default), the logs are also stored in `registered_logs`. |
| `registered_logs` | `std::vector<std::shared_ptr<VxSystemLog>>` | The logs stored since the start of Vortex. |
| `global_logger` | `std::shared_ptr<spdlog::logger>` | Logger of the global log file of the session. |
| `console_logger` | `std::shared_ptr<spdlog::logger>` | Logger of the console. |
| `pool_loggers` | `std::vector<std::pair<std::string, std::shared_ptr<spdlog::logger>>>` | Loggers of the log pools, with the name of the pool. |

```cpp
for (auto &log : ctx->registered_logs)
  std::cout << log->timestamp << " [" << log->filter << "] " << log->message << std::endl;
```

---

### Events

| Member | Type | Description |
|---|---|---|
| `input_events` | `std::vector<std::shared_ptr<CoreInputEvent>>` | The input events of the core. They are called with the target `"vortex"`. |

```cpp
for (auto &event : ctx->input_events)
  vxe::log_info("My module", event->name_);
```

---

### Session

| Member | Type | Description |
|---|---|---|
| `state` | `SessionState` | State of the current session. |

**SessionState**

| Member | Type | Description |
|---|---|---|
| `session_id` | `std::string` | Identifier of the session. |
| `last_used_module` | `std::shared_ptr<ModuleInterface>` | Last module used. |

```cpp
std::string id = ctx->state.session_id;
```

---

### Credits and documentation

| Member | Type | Description |
|---|---|---|
| `credits` | `std::map<std::string, TopicCredits>` | The credits, by topic. |
| `documentations` | `std::map<std::string, DocumentationTopic>` | The embedded documentation, by topic. |

```cpp
bool has_credits = ctx->credits.count("my_topic") > 0;
```

---

### Menus

| Member | Type | Description |
|---|---|---|
| `editMenuItems` | `std::vector<EditMenuItem>` | The items added to the edit menu. |
| `customMenus` | `std::vector<CustomMenu>` | The custom menus added to the menu bar. |
| `modules_section_on_toolbar` | `bool` | If `true`, the modules section is displayed on the toolbar. |

```cpp
bool modules_section = ctx->modules_section_on_toolbar;
```

---

### Net

| Member | Type | Description |
|---|---|---|
| `net` | `VortexNet` | The net features of Vortex. See [VortexNet](#vortexnet). |

```cpp
std::string response = ctx->net.GET("https://example.com");
```

---

### IO

`ctx->IO` is a `VxIO`. It holds the modules, plugins, projects, themes and content browser data.

**Modules, plugins and projects**

| Member | Type | Description |
|---|---|---|
| `em` | `std::vector<std::shared_ptr<ModuleInterface>>` | The modules installed in the current project. |
| `sys_em` | `std::vector<std::shared_ptr<ModuleInterface>>` | The modules installed on the system. They are initialized but not loaded in the project. |
| `ep` | `std::vector<std::shared_ptr<PluginInterface>>` | The plugins installed in the current project. |
| `sys_ep` | `std::vector<std::shared_ptr<PluginInterface>>` | The plugins installed on the system. They are initialized but not loaded in the project. |
| `sys_templates` | `std::vector<std::shared_ptr<TemplateInterface>>` | The templates installed on the system. Loaded if `include_system_templates` is `true`. |
| `sys_projects` | `std::vector<std::shared_ptr<EnvProject>>` | The projects known by the system. |
| `sys_vortex_versions_pools` | `std::vector<std::string>` | The Vortex versions pools of the system. |

```cpp
for (auto &module : ctx->IO.em)
  vxe::log_info("My module", module->name() + " : " + module->state());
```

**Net**

| Member | Type | Description |
|---|---|---|
| `allow_net` | `bool` | `true` if the user allows Vortex to access the internet. |

```cpp
if (ctx->IO.allow_net)
  std::string response = ctx->net.GET("https://example.com");
```

**Themes**

| Member | Type | Description |
|---|---|---|
| `themes` | `std::vector<std::shared_ptr<Theme>>` | The themes of the project. |
| `used_theme` | `std::string` | Label of the theme in use. |
| `override_themes` | `std::vector<std::string>` | The override themes. |
| `theme_changed` | `bool` | Set to `true` when the theme changed, so the interface is rebuilt with the new values. |

```cpp
vxe::log_info("My module", "Theme : " + ctx->IO.used_theme);
```

**Content browser**

| Member | Type | Description |
|---|---|---|
| `contentbrowser_items` | `std::vector<std::shared_ptr<ContenBrowserItem>>` | The items of the content browser. |
| `contentbrowser_customfolders` | `std::vector<std::shared_ptr<ContentBrowserCustomFolder>>` | The folders with a custom color or favorite status. |
| `contentbrowser_mainpool` | `std::string` | The main pool of the content browser, relative to the project path. |
| `contentbrowser_absolute_mainpool` | `std::string` | The absolute path of the main pool. |
| `contentbrowser_pools` | `std::vector<std::pair<std::string, std::string>>` | The other pools, as pairs of absolute path and name. |
| `copy_selection` | `std::vector<std::string>` | The paths selected with copy. |
| `cut_selection` | `std::vector<std::string>` | The paths selected with cut. |

```cpp
for (auto &pool : ctx->IO.contentbrowser_pools)
  vxe::log_info("My module", pool.second + " : " + pool.first);
```

---

## Structures

### EnvProject

A project known by the system.

| Member | Type | Description |
|---|---|---|
| `name` | `std::string` | Name of the project. |
| `path` | `std::string` | Path of the project. |
| `version` | `std::string` | Version of the project. |
| `compatibleWith` | `std::string` | Version of Vortex the project is compatible with. |
| `description` | `std::string` | Description of the project. |
| `logoPath` | `std::string` | Path of the logo. |
| `author` | `std::string` | Author of the project. |
| `lastOpened` | `std::string` | Date of the last opening. |

---

### Theme

A theme of the interface.

| Member | Type | Description |
|---|---|---|
| `label` | `std::string` | Label of the theme. |
| `name` | `std::string` | Name of the theme. |
| `description` | `std::string` | Description of the theme. |
| `authors` | `std::string` | Authors of the theme. |
| `theme` | `std::unordered_map<std::string, std::string>` | The properties of the theme, as pairs of property name and value. |

---

### VxSystemLog

A log stored in `registered_logs`.

| Member | Type | Description |
|---|---|---|
| `level` | `spdlog::level::level_enum` | Level of the log. |
| `filter` | `std::string` | Scope of the log. |
| `message` | `std::string` | Message of the log. |
| `timestamp` | `std::string` | Date of the log (`%Y-%m-%d %H:%M:%S`). |

---

### ContentBrowserCustomFolder

A folder customized in the content browser.

| Member | Type | Description |
|---|---|---|
| `path` | `std::string` | Path of the folder. |
| `color` | `std::string` | Color of the folder, as a hex string. |
| `is_fav` | `bool` | `true` if the folder is a favorite. |

---

### EditMenuItem

An item of the edit menu.

| Member | Type | Description |
|---|---|---|
| `title` | `std::string` | Title of the item. |
| `action` | `std::function<void()>` | Function executed when the item is clicked. |
| `logo` | `std::string` | Path of the logo. |
| `section` | `std::string` | Section of the menu where the item is displayed. |

---

### CustomMenu

A custom menu of the menu bar.

| Member | Type | Description |
|---|---|---|
| `title` | `std::string` | Title of the menu. |
| `render` | `std::function<void()>` | Function rendering the content of the menu. |

---

### PlatformVendor

Enum of the operating systems.

| Value | Description |
|---|---|
| `PlatformVendor::Linux` | Linux. |
| `PlatformVendor::Windows` | Windows. |
| `PlatformVendor::Macos` | macOS. |

---

# VortexNet

Net features of Vortex. The instance of the context is accessed with `ctx->net`.

`VortexNet` does not check the `allow_net` setting by itself. Check `ctx->IO.allow_net` before sending a request.

### CheckNet

Checks if the internet is reachable.

On Windows, it pings `1.1.1.1`, then `8.8.8.8` if the first one fails. On the other systems, it resolves `infinite.si`.

```cpp
bool CheckNet();
```

**Returns:** `true` if the internet is reachable.

```cpp
if (ctx->net.CheckNet())
  vxe::log_info("My module", "Online");
```

---

### GET

Sends a GET request and waits for the response.

```cpp
std::string GET(const std::string &url);
```

| Parameter | Type | Description |
|---|---|---|
| `url` | `const std::string &` | URL of the request. |

**Returns:** the body of the response, or an empty string if the request failed or the response is empty.

```cpp
std::string response = ctx->net.GET("https://example.com/api/data");
```

---

### POST

Sends a POST request and waits for the response.

```cpp
std::string POST(const std::string &url, const std::string &body, const std::string &contentType = "application/json");
```

| Parameter | Type | Description |
|---|---|---|
| `url` | `const std::string &` | URL of the request. |
| `body` | `const std::string &` | Body of the request. |
| `contentType` | `const std::string &` (optional) | Content type of the body. Default is `"application/json"`. |

**Returns:** the body of the response, or an empty string if the request failed or the response is empty.

```cpp
nlohmann::json payload = { { "uuid", "my_uuid" } };
std::string response = ctx->net.POST("https://example.com/api/send", payload.dump());
```

---

# Arguments and return values

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

# Logs

### vxe::log_info

Logs an information message. A second version logs in a log pool.

```cpp
void vxe::log_info(const std::string &scope, const std::string &message);
void vxe::log_info(const std::string &pool_name, const std::string &scope, const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `pool_name` | `const std::string &` | Name of the log pool. |
| `scope` | `const std::string &` | Scope of the log (the part of Vortex or the module that logs). |
| `message` | `const std::string &` | The message to log. |

```cpp
vxe::log_info("My module", "Cool info");
vxe::log_info("my_pool", "My module", "Cool info");
```

---

### vxe::log_warn

Logs a warning message.

```cpp
void vxe::log_warn(const std::string &scope, const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `scope` | `const std::string &` | Scope of the log. |
| `message` | `const std::string &` | The message to log. |

```cpp
vxe::log_warn("My module", "Be careful");
```

---

### vxe::log_error

Logs an error message.

```cpp
void vxe::log_error(const std::string &scope, const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `scope` | `const std::string &` | Scope of the log. |
| `message` | `const std::string &` | The message to log. |

```cpp
vxe::log_error("My module", "Unable to do this or that");
```

---

### vxe::log_fatal

Logs a fatal error message.

```cpp
void vxe::log_fatal(const std::string &scope, const std::string &message);
```

| Parameter | Type | Description |
|---|---|---|
| `scope` | `const std::string &` | Scope of the log. |
| `message` | `const std::string &` | The message to log. |

```cpp
vxe::log_fatal("My module", "File unrecoverable");
```

---

### vxe::create_log_pool

Creates a log pool, with its own log file, and returns its logger.

```cpp
std::shared_ptr<spdlog::logger> vxe::create_log_pool(const std::string &pool_name);
```

| Parameter | Type | Description |
|---|---|---|
| `pool_name` | `const std::string &` | Name of the pool. It is also the name of the log file. |

**Returns:** the logger of the pool.

```cpp
auto logger = vxe::create_log_pool("my_pool");
```

---

### vxe::create_global_logger

Creates the global logger of the session, writing in the `global.log` file of the session.

```cpp
std::shared_ptr<spdlog::logger> vxe::create_global_logger();
```

**Returns:** the global logger. It is also stored in `ctx->global_logger`.

```cpp
auto logger = vxe::create_global_logger();
```

---

### vxe::create_console_logger

Creates the logger of the console.

```cpp
std::shared_ptr<spdlog::logger> vxe::create_console_logger();
```

**Returns:** the console logger. It is also stored in `ctx->console_logger`.

```cpp
auto logger = vxe::create_console_logger();
```

---

### vxe::drop_loggers

Drops all the loggers.

```cpp
void vxe::drop_loggers();
```

```cpp
vxe::drop_loggers();
```

---

### vxe::log_level_to_string

Converts a log level to a string.

```cpp
std::string vxe::log_level_to_string(const spdlog::level::level_enum &level);
```

| Parameter | Type | Description |
|---|---|---|
| `level` | `const spdlog::level::level_enum &` | The log level. |

**Returns:** `"error"`, `"warning"`, `"info"`, `"trace"`, `"fatal"` or `"unknown"`.

```cpp
std::string level = vxe::log_level_to_string(log->level);
```

---

# Events

### vxe::call_output_event

Broadcasts an output event to all the modules and plugins having a handler for it.

```cpp
void vxe::call_output_event(
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret,
    const std::string &origin);
```

| Parameter | Type | Description |
|---|---|---|
| `event_name` | `const std::string &` | Name of the output event. |
| `args` | `ArgumentValues &` | Arguments sent to the handlers. |
| `ret` | `ReturnValues &` | Receives the value returned by the handlers. |
| `origin` | `const std::string &` | Name of the caller. |

```cpp
ArgumentValues args;
args.set("path");

ReturnValues ret;
vxe::call_output_event("just_save", args, ret, "my_module");
```

---

### vxe::call_input_event

Calls an input event of the core, a module or a plugin. An error is logged if the event is not found.

```cpp
void vxe::call_input_event(
    const std::string &module_name,
    const std::string &event_name,
    ArgumentValues &args,
    ReturnValues &ret,
    const std::string &origin);
```

| Parameter | Type | Description |
|---|---|---|
| `module_name` | `const std::string &` | Name of the module or plugin that owns the event, or `"vortex"` for the core. |
| `event_name` | `const std::string &` | Name of the input event. |
| `args` | `ArgumentValues &` | Arguments sent to the event. |
| `ret` | `ReturnValues &` | Receives the value returned by the handler. |
| `origin` | `const std::string &` | Name of the caller. |

```cpp
ArgumentValues args;
ReturnValues ret;
vxe::call_input_event("vortex", "spawn_content_browser", args, ret, "my_module");
```

---

### vxe::add_core_input_event

Adds an input event to the core. It can then be called with the target `"vortex"`.

```cpp
void vxe::add_core_input_event(const CoreInputEvent &event);
```

| Parameter | Type | Description |
|---|---|---|
| `event` | `const CoreInputEvent &` | The input event to add. |

```cpp
vxe::add_core_input_event(CoreInputEvent([]() { vxe::log_info("Core", "Hello"); }, "say_hello"));
```

---

### vxe::trigger_save_all_event

Broadcasts the output event `save_all`, with the origin `"vortex"`.

```cpp
void vxe::trigger_save_all_event();
```

```cpp
vxe::trigger_save_all_event();
```

---

### Core input events

| Event | Description |
|---|---|
| `spawn_content_browser` | Spawns a content browser. |

---

### CoreInputEvent

An input event of the core.

```cpp
CoreInputEvent(std::function<void(ArgumentValues &, ReturnValues &)> foo, const std::string &name);
CoreInputEvent(std::function<void(ArgumentValues &)> foo, const std::string &name);
CoreInputEvent(std::function<void(ReturnValues &)> foo, const std::string &name);
CoreInputEvent(std::function<void()> foo, const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `foo` | `std::function` | Handler executed when the event is called. Can receive the arguments, the return value, both or none. |
| `name` | `const std::string &` | Name of the event. |

| Member | Type | Description |
|---|---|---|
| `name_` | `std::string` | Name of the event. |
| `function_` | `std::function<void(ArgumentValues &, ReturnValues &)>` | The handler, whatever the signature given in the constructor. |

#### trigger_happening

Records a happening on the event and logs it in the logs under the source `Modules Events`.

```cpp
void trigger_happening(const std::string &trigger_name, HappeningState state, const std::string &log);
```

| Parameter | Type | Description |
|---|---|---|
| `trigger_name` | `const std::string &` | Name of what triggered the happening. |
| `state` | `HappeningState` | Level of the happening: `INFO`, `WARNING`, `ERR` or `FATAL`. It also defines the level of the log. |
| `log` | `const std::string &` | Message of the happening. |

```cpp
event->trigger_happening("my_module", HappeningState::WARNING, "Something happened");
```

---

# Environment

### vxe::init_environment

Initializes the environment of Vortex. It creates the folders and files needed in the `.vx` folder of the user (data, modules, templates, `projects.json`) and initializes the net configuration.

```cpp
void vxe::init_environment();
```

```cpp
vxe::init_environment();
```

---

### vxe::refresh_environment_projects

Reloads the projects known by the system, from the `projects.json` file, into `ctx->IO.sys_projects`.

```cpp
void vxe::refresh_environment_projects();
```

```cpp
vxe::refresh_environment_projects();
```

---

### vxe::refresh_environment_vortex_versions_pools

Reloads the Vortex versions pools of the system, from the `vortex_versions_pools.json` file, into `ctx->IO.sys_vortex_versions_pools`.

```cpp
void vxe::refresh_environment_vortex_versions_pools();
```

```cpp
vxe::refresh_environment_vortex_versions_pools();
```

---

### vxe::update_environment_project

Updates the projects known by the system (the `projects.json` file). Three versions are available.

```cpp
void vxe::update_environment_project(
    const std::string &name,
    const std::string &author,
    const std::string &version,
    const std::string &compatibleWith,
    const std::string &description,
    const std::string &path,
    const std::string &logo_path,
    const std::string &template_name);

void vxe::update_environment_project();

void vxe::update_environment_project(const std::string &oldname);
```

| Parameter | Type | Description |
|---|---|---|
| `name` | `const std::string &` | Name of the project. |
| `author` | `const std::string &` | Author of the project. |
| `version` | `const std::string &` | Version of the project. |
| `compatibleWith` | `const std::string &` | Version of Vortex the project is compatible with. |
| `description` | `const std::string &` | Description of the project. |
| `path` | `const std::string &` | Path of the project. |
| `logo_path` | `const std::string &` | Path of the logo. |
| `template_name` | `const std::string &` | Name of the template of the project. |
| `oldname` | `const std::string &` | Name of the project to update, with the informations of the current project. |

- With all the parameters: registers a new project. Nothing is done if a project with this name already exists.
- Without parameter: updates the entry of the current project, or adds it if it does not exist.
- With `oldname`: updates the entry named `oldname` with the informations of the current project, or adds it if it does not exist.

```cpp
vxe::update_environment_project();
```

---

### Platform

Functions to detect and check the platform.

| Function | Description |
|---|---|
| `void vxe::initialize_platform_vendor()` | Sets `ctx->platform_vendor`. |
| `void vxe::detect_platform()` | Sets `ctx->platform`. |
| `void vxe::detect_arch()` | Sets `ctx->arch`. |
| `bool vxe::is_linux()` | `true` on Linux. |
| `bool vxe::is_not_linux()` | `true` if not on Linux. |
| `bool vxe::is_windows()` | `true` on Windows. |
| `bool vxe::is_not_windows()` | `true` if not on Windows. |
| `bool vxe::is_macos()` | `true` on macOS. |
| `bool vxe::is_not_macos()` | `true` if not on macOS. |

The `is_*` functions use `ctx->platform_vendor`, set by `initialize_platform_vendor`.

```cpp
vxe::initialize_platform_vendor();

if (vxe::is_linux())
  vxe::log_info("My module", "Running on Linux");
```

---

### vxe::net_config_path

Returns the path of the `net.json` file, the net configuration of the user.

```cpp
std::string vxe::net_config_path();
```

**Returns:** the path of `net.json`.

```cpp
std::string path = vxe::net_config_path();
```

---

### vxe::ensure_net_config

Makes sure the `net.json` file exists and is valid, and returns its content. The file is created or repaired with `{"allow_net": false}` if needed.

```cpp
nlohmann::json vxe::ensure_net_config(std::function<void(const std::string &)> on_error);
```

| Parameter | Type | Description |
|---|---|---|
| `on_error` | `std::function<void(const std::string &)>` | Function called with a message when the file had to be repaired. |

**Returns:** the content of `net.json`.

```cpp
auto config = vxe::ensure_net_config([](const std::string &err) { vxe::log_error("My module", err); });
```

---

### vxe::refresh_net_permission_state

Reloads `allow_net` from the `net.json` file into `ctx->IO.allow_net`.

```cpp
void vxe::refresh_net_permission_state();
```

```cpp
vxe::refresh_net_permission_state();
```

---

### vxe::toggle_vortex_net_permission

Toggles the permission of Vortex to access the internet, saves it in `net.json` and refreshes `ctx->IO.allow_net`.

```cpp
bool vxe::toggle_vortex_net_permission(std::function<void(const std::string &)> on_error);
```

| Parameter | Type | Description |
|---|---|---|
| `on_error` | `std::function<void(const std::string &)>` | Function called with a message if the file cannot be read or written. |

**Returns:** the new value of `allow_net`.

```cpp
bool allowed = vxe::toggle_vortex_net_permission([](const std::string &err) { vxe::log_error("My module", err); });
```

---

# Project

### vxe::init_project

Initializes the current project from the content of its `vortex.config` file.

It reads the project informations into the context, sets the project path to the current working directory, initializes the environment, loads the modules, plugins and templates, loads the themes, registers the core input events, executes the startup script and starts the modules and plugins.

An error is logged if the configuration has no `project` section.

```cpp
void vxe::init_project(const nlohmann::json &main_configs);
```

| Parameter | Type | Description |
|---|---|---|
| `main_configs` | `const nlohmann::json &` | Content of the `vortex.config` file. |

```cpp
vxe::init_project(vxe::dump_json("vortex.config"));
```

---

### vxe::create_project

Creates a new project in `path` from a template, writes its informations in its `vortex.config` file and registers it in the projects known by the system.

Nothing is done if a project with the same name already exists.

```cpp
void vxe::create_project(
    const std::string &name,
    const std::string &author,
    const std::string &version,
    const std::string &description,
    const std::string &path,
    const std::string &logo_path,
    const std::string &template_name);
```

| Parameter | Type | Description |
|---|---|---|
| `name` | `const std::string &` | Name of the project. |
| `author` | `const std::string &` | Author of the project. |
| `version` | `const std::string &` | Version of the project. |
| `description` | `const std::string &` | Description of the project. |
| `path` | `const std::string &` | Path where the project is created. |
| `logo_path` | `const std::string &` | Path of the logo. |
| `template_name` | `const std::string &` | Name of the template used to create the project. |

```cpp
vxe::create_project("My project", "Jane Doe", "1.0.0", "My first project", "/path/to/project", "", "my_template");
```

---

### vxe::refresh_project_informations

Reloads the informations of the current project from its `vortex.config` file into the context.

```cpp
void vxe::refresh_project_informations();
```

```cpp
vxe::refresh_project_informations();
```

---

### vxe::find_project_in_folder

Searches the projects (`vortex.config` files) in a folder.

```cpp
std::vector<std::shared_ptr<EnvProject>> vxe::find_project_in_folder(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The folder to search in. |

**Returns:** the projects found.

```cpp
auto projects = vxe::find_project_in_folder("/path/to/folder");
```

---

### vxe::bootstrapp_all_modules

Prepares and starts the modules of the project. For each module, the data folder (`datapath`) is set, and the module is started if its `auto_exec` is `true`.

```cpp
void vxe::bootstrapp_all_modules();
```

```cpp
vxe::bootstrapp_all_modules();
```

---

### vxe::execute_start_script

Executes the startup script of the project (`startup_script`). Nothing is done if it is not defined. A warning is logged if the script is invalid.

```cpp
void vxe::execute_start_script();
```

```cpp
vxe::execute_start_script();
```

---

### Update project fields

These functions update a field of the `project` section of the `vortex.config` file of the current project. They all have the same signature. An error is logged if the file cannot be read or written, or has no `project` section.

```cpp
void vxe::update_project_xxx(const std::string &value);
```

| Parameter | Type | Description |
|---|---|---|
| `value` | `const std::string &` | The new value of the field. |

| Function | Field |
|---|---|
| `update_project_name` | `name` |
| `update_project_author` | `author` |
| `update_project_description` | `description` |
| `update_project_type` | `type` |
| `update_project_version` | `version` |
| `update_project_tags` | `tags` |
| `update_project_website` | `website` |
| `update_project_support_contact` | `support_contact` |
| `update_project_logo_path` | `logoPath` |
| `update_project_banner_path` | `bannerPath` |
| `update_project_root_content_path` | `root_content_path` |
| `update_project_startup_script` | `startup_script` |
| `update_project_copyright_flle` | `copyright_file` |
| `update_project_license_flle` | `license_file` |
| `update_project_readme_flle` | `readme_file` |
| `update_project_requirements_flle` | `requirements_file` |
| `update_project_code_of_conduct_flle` | `code_of_conduct_file` |
| `update_project_security_flle` | `security_file` |

```cpp
vxe::update_project_name("My project");
vxe::update_project_version("1.0.1");
```

---

# Session

The sessions of the user are stored in the `.vx/sessions` folder of the user, by topic.

### vxe::create_session_topic

Creates the folder of a session topic, with its `crash` and `logs` folders and its files (`core_dump.txt`, `log_dump.txt` and `vortex.json`).

```cpp
void vxe::create_session_topic(const std::string &post_topic);
```

| Parameter | Type | Description |
|---|---|---|
| `post_topic` | `const std::string &` | Name of the session topic. |

```cpp
vxe::create_session_topic("my_session");
```

---

### vxe::delete_session_topic

Deletes the folder of a session topic.

```cpp
void vxe::delete_session_topic(const std::string &post_topic);
```

| Parameter | Type | Description |
|---|---|---|
| `post_topic` | `const std::string &` | Name of the session topic. |

```cpp
vxe::delete_session_topic("my_session");
```

---

### vxe::get_last_module_of_last_session

Returns the content of the `last_module.json` file of a session topic.

```cpp
nlohmann::json vxe::get_last_module_of_last_session(const std::string &post_topic);
```

| Parameter | Type | Description |
|---|---|---|
| `post_topic` | `const std::string &` | Name of the session topic. |

**Returns:** the JSON content of the file.

```cpp
auto last_module = vxe::get_last_module_of_last_session("my_session");
```

---

### vxe::get_last_session

Returns the content of the `master_session.json` file of a session topic.

```cpp
nlohmann::json vxe::get_last_session(const std::string &post_topic);
```

| Parameter | Type | Description |
|---|---|---|
| `post_topic` | `const std::string &` | Name of the session topic. |

**Returns:** the JSON content of the file.

```cpp
auto last_session = vxe::get_last_session("my_session");
```

---

### vxe::post_session_state

Saves the state of the session for a session topic (the version of Vortex, and the last used module if it changed).

```cpp
void vxe::post_session_state(const std::string &post_topic);
```

| Parameter | Type | Description |
|---|---|---|
| `post_topic` | `const std::string &` | Name of the session topic. |

```cpp
vxe::post_session_state("my_session");
```

---

# Content browser

### Items

#### vxe::add_content_browser_item

Adds an item to the content browser.

```cpp
void vxe::add_content_browser_item(const std::shared_ptr<ContenBrowserItem> &item);
```

| Parameter | Type | Description |
|---|---|---|
| `item` | `const std::shared_ptr<ContenBrowserItem> &` | The item to add. |

```cpp
vxe::add_content_browser_item(item);
```

---

#### vxe::get_all_item_handlers_for

Returns the item handlers of a type, declared by the running modules and plugins.

```cpp
std::vector<std::shared_ptr<ItemHandlerInterface>> vxe::get_all_item_handlers_for(const std::string &type);
```

| Parameter | Type | Description |
|---|---|---|
| `type` | `const std::string &` | Type of item (a file extension, or a module item type). |

**Returns:** the handlers of this type.

```cpp
auto handlers = vxe::get_all_item_handlers_for("png");
```

---

### Create files and folders

The names are numbered (`New file 2.txt`, `New folder 2`...) if the name already exists.

#### vxe::create_file

Creates an empty text file named `New file.txt` in a folder.

```cpp
std::string vxe::create_file(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The folder where the file is created. |

**Returns:** the absolute path of the new file.

```cpp
std::string file = vxe::create_file("/path/to/folder");
```

---

#### vxe::create_folder

Creates a folder named `New folder` in a folder.

```cpp
std::string vxe::create_folder(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The folder where the new folder is created. |

**Returns:** the absolute path of the new folder.

```cpp
std::string folder = vxe::create_folder("/path/to/folder");
```

---

#### vxe::create_item

Creates a folder named `New item` in a folder.

```cpp
std::string vxe::create_item(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The folder where the item is created. |

**Returns:** the absolute path of the new item.

```cpp
std::string item = vxe::create_item("/path/to/folder");
```

---

### Rename and delete

#### vxe::submit_rename

Renames a file or a folder. If the new name already exists, a number is added to it (`name (1)`).

```cpp
void vxe::submit_rename(const std::string &oldPathStr, const std::string &newName);
```

| Parameter | Type | Description |
|---|---|---|
| `oldPathStr` | `const std::string &` | Path of the file or folder. |
| `newName` | `const std::string &` | The new name. |

```cpp
vxe::submit_rename("/path/to/file.txt", "renamed.txt");
```

---

#### vxe::rename_folder

Renames a folder. The custom folder rules of the folder are transferred to the new path. An exception is thrown if the folder does not exist or if the operation fails.

```cpp
void vxe::rename_folder(const std::string &target_path, const std::string &new_name);
```

| Parameter | Type | Description |
|---|---|---|
| `target_path` | `const std::string &` | Path of the folder. |
| `new_name` | `const std::string &` | The new name. |

```cpp
vxe::rename_folder("/path/to/folder", "renamed");
```

---

#### vxe::rename_file

Renames a file. An exception is thrown if the file does not exist or if the operation fails.

```cpp
void vxe::rename_file(const std::string &target_path, const std::string &new_name);
```

| Parameter | Type | Description |
|---|---|---|
| `target_path` | `const std::string &` | Path of the file. |
| `new_name` | `const std::string &` | The new name. |

```cpp
vxe::rename_file("/path/to/file.txt", "renamed.txt");
```

---

#### vxe::delete_file

Deletes a file.

```cpp
void vxe::delete_file(const std::string &target_path);
```

| Parameter | Type | Description |
|---|---|---|
| `target_path` | `const std::string &` | Path of the file. |

```cpp
vxe::delete_file("/path/to/file.txt");
```

---

#### vxe::delete_folder

Deletes a folder and its content.

```cpp
void vxe::delete_folder(const std::string &target_path);
```

| Parameter | Type | Description |
|---|---|---|
| `target_path` | `const std::string &` | Path of the folder. |

```cpp
vxe::delete_folder("/path/to/folder");
```

---

#### vxe::delete_path

Deletes a file or a folder.

```cpp
void vxe::delete_path(const std::string &target_path);
```

| Parameter | Type | Description |
|---|---|---|
| `target_path` | `const std::string &` | Path of the file or folder. |

```cpp
vxe::delete_path("/path/to/target");
```

---

### Clipboard

#### vxe::copy

Selects paths to copy, in `ctx->IO.copy_selection`.

```cpp
void vxe::copy(std::vector<std::string> selection, bool in_addition);
```

| Parameter | Type | Description |
|---|---|---|
| `selection` | `std::vector<std::string>` | The paths to copy. |
| `in_addition` | `bool` | If `true`, the paths are added to the current selection. If `false`, the current selection is cleared first. |

```cpp
vxe::copy({ "/path/to/file.txt" }, false);
```

---

#### vxe::cut

Selects paths to cut, in `ctx->IO.cut_selection`.

```cpp
void vxe::cut(std::vector<std::string> selection, bool in_addition);
```

| Parameter | Type | Description |
|---|---|---|
| `selection` | `std::vector<std::string>` | The paths to cut. |
| `in_addition` | `bool` | If `true`, the paths are added to the current selection. If `false`, the current selection is cleared first. |

```cpp
vxe::cut({ "/path/to/file.txt" }, false);
```

---

#### vxe::clear_copy_selection

Clears the paths selected with copy.

```cpp
void vxe::clear_copy_selection();
```

```cpp
vxe::clear_copy_selection();
```

---

#### vxe::clear_cut_selection

Clears the paths selected with cut.

```cpp
void vxe::clear_cut_selection();
```

```cpp
vxe::clear_cut_selection();
```

---

#### vxe::paste_all_selections

Pastes the copied paths (copied with a new name if the name already exists in the target) and moves the cut paths into a folder.

```cpp
void vxe::paste_all_selections(const std::string &target_path_str);
```

| Parameter | Type | Description |
|---|---|---|
| `target_path_str` | `const std::string &` | The folder where the paths are pasted. |

```cpp
vxe::paste_all_selections("/path/to/folder");
```

---

### Pools

#### vxe::publish_pool

Adds a pool to the content browser and saves the pools. Nothing is done if the pool already exists.

```cpp
void vxe::publish_pool(const std::string &absolute_pool_path, const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `absolute_pool_path` | `const std::string &` | Absolute path of the pool. |
| `name` | `const std::string &` | Name of the pool. |

```cpp
vxe::publish_pool("/path/to/pool", "My pool");
```

---

#### vxe::remove_pool

Removes a pool from the content browser and saves the pools.

```cpp
void vxe::remove_pool(const std::string &absolute_pool_path, const std::string &name);
```

| Parameter | Type | Description |
|---|---|---|
| `absolute_pool_path` | `const std::string &` | Absolute path of the pool. |
| `name` | `const std::string &` | Name of the pool. |

```cpp
vxe::remove_pool("/path/to/pool", "My pool");
```

---

#### vxe::fetch_pools

Loads the pools of the content browser from the configuration of the project into the context.

```cpp
void vxe::fetch_pools();
```

```cpp
vxe::fetch_pools();
```

---

#### vxe::post_pools_to_json

Saves the pools of the content browser in the configuration of the project.

```cpp
void vxe::post_pools_to_json();
```

```cpp
vxe::post_pools_to_json();
```

---

### Custom folders

#### vxe::publish_content_browser_custom_folder

Sets the color and the favorite status of a folder, and saves the custom folders.

```cpp
void vxe::publish_content_browser_custom_folder(const std::string &path, const std::string &hex_color, const bool &isFav);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |
| `hex_color` | `const std::string &` | Color of the folder, as a hex string. |
| `isFav` | `const bool &` | `true` to set the folder as a favorite. |

```cpp
vxe::publish_content_browser_custom_folder("/path/to/folder", "#4a90e2", true);
```

---

#### vxe::publish_content_browser_custom_folder_color

Sets the color of a folder, and saves the custom folders.

```cpp
void vxe::publish_content_browser_custom_folder_color(const std::string &path, const std::string &hex_color);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |
| `hex_color` | `const std::string &` | Color of the folder, as a hex string. |

```cpp
vxe::publish_content_browser_custom_folder_color("/path/to/folder", "#4a90e2");
```

---

#### vxe::publish_content_browser_custom_folder_is_fav

Sets the favorite status of a folder, and saves the custom folders.

```cpp
void vxe::publish_content_browser_custom_folder_is_fav(const std::string &path, const bool &isFav);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |
| `isFav` | `const bool &` | `true` to set the folder as a favorite. |

```cpp
vxe::publish_content_browser_custom_folder_is_fav("/path/to/folder", true);
```

---

#### vxe::content_browser_folder_is_fav

Checks if a folder is a favorite.

```cpp
bool vxe::content_browser_folder_is_fav(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |

**Returns:** `true` if the folder is a favorite.

```cpp
bool fav = vxe::content_browser_folder_is_fav("/path/to/folder");
```

---

#### vxe::get_content_browser_folder_color

Gets the custom color of a folder.

```cpp
bool vxe::get_content_browser_folder_color(const std::string &path, ImU32 *color);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |
| `color` | `ImU32 *` | Receives the color of the folder. |

**Returns:** `true` if the folder has a custom color.

```cpp
ImU32 color;
if (vxe::get_content_browser_folder_color("/path/to/folder", &color)) {
  // The folder has a custom color
}
```

---

#### vxe::transfer_custom_folder_rules

Moves the custom rules (color, favorite) of a folder, and of its subfolders, to a new path, and saves the custom folders.

```cpp
void vxe::transfer_custom_folder_rules(const std::string &old_path, const std::string &new_path);
```

| Parameter | Type | Description |
|---|---|---|
| `old_path` | `const std::string &` | The old path of the folder. |
| `new_path` | `const std::string &` | The new path of the folder. |

```cpp
vxe::transfer_custom_folder_rules("/path/to/old", "/path/to/new");
```

---

#### vxe::fetch_custom_folders

Loads the custom folders from the configuration of the project into the context.

```cpp
void vxe::fetch_custom_folders();
```

```cpp
vxe::fetch_custom_folders();
```

---

#### vxe::post_custom_folder_to_json

Saves the custom folders in the configuration of the project.

```cpp
void vxe::post_custom_folder_to_json();
```

```cpp
vxe::post_custom_folder_to_json();
```

---

# Themes

### vxe::refresh_project_themes

Loads the themes of the project, and the theme configuration (`used_theme` and `override_themes`), into the context. The themes are the `.json` files of the themes folder of the project.

```cpp
void vxe::refresh_project_themes();
```

```cpp
vxe::refresh_project_themes();
```

---

### vxe::verify_and_populate_themes

Checks that the default themes (`dark` and `clear`) exist and are valid in the project, and recreates them if needed.

```cpp
void vxe::verify_and_populate_themes();
```

```cpp
vxe::verify_and_populate_themes();
```

---

### vxe::get_theme

Gets a theme by its label.

```cpp
std::shared_ptr<Theme> vxe::get_theme(const std::string &label);
```

| Parameter | Type | Description |
|---|---|---|
| `label` | `const std::string &` | Label of the theme. |

**Returns:** the theme, or `nullptr` if it is not found.

```cpp
auto theme = vxe::get_theme("dark");
```

---

### vxe::get_selected_theme

Gets the theme in use (`ctx->IO.used_theme`). An error is logged if no theme matches.

```cpp
std::shared_ptr<Theme> vxe::get_selected_theme();
```

**Returns:** the theme in use, or `nullptr` if it is not found.

```cpp
auto theme = vxe::get_selected_theme();
```

---

### vxe::create_new_theme

Creates a new theme file from a base theme. The label of the new theme is the title, in lowercase and with underscores instead of spaces.

```cpp
void vxe::create_new_theme(const std::shared_ptr<Theme> &base_theme, const std::string &title);
```

| Parameter | Type | Description |
|---|---|---|
| `base_theme` | `const std::shared_ptr<Theme> &` | The theme to copy. |
| `title` | `const std::string &` | Name of the new theme. |

```cpp
vxe::create_new_theme(vxe::get_theme("dark"), "My theme");
```

---

### vxe::update_project_themes_config

Saves the theme configuration (`used_theme` and `override_themes`) of the context.

```cpp
void vxe::update_project_themes_config();
```

```cpp
ctx->IO.used_theme = "clear";
vxe::update_project_themes_config();
```

---

### vxe::rebuild_theme

Asks for the interface to rebuild its theme (`ctx->IO.theme_changed` becomes `true`).

```cpp
void vxe::rebuild_theme();
```

```cpp
vxe::rebuild_theme();
```

---

### vxe::theme_rebuilded

Marks the theme as rebuilt (`ctx->IO.theme_changed` becomes `false`).

```cpp
void vxe::theme_rebuilded();
```

```cpp
vxe::theme_rebuilded();
```

---

### vxe::is_theme_needs_rebuild

Checks if the interface needs to rebuild its theme.

```cpp
bool vxe::is_theme_needs_rebuild();
```

**Returns:** `true` if the theme needs to be rebuilt.

```cpp
if (vxe::is_theme_needs_rebuild()) {
  vxe::theme_rebuilded();
}
```

---

# Customizations

### vxe::push_edit_menu_item

Adds an item to the edit menu.

```cpp
void vxe::push_edit_menu_item(
    const std::string &title,
    const std::function<void()> &action,
    const std::string &logo,
    const std::string &section);
```

| Parameter | Type | Description |
|---|---|---|
| `title` | `const std::string &` | Title of the item. |
| `action` | `const std::function<void()> &` | Function executed when the item is clicked. |
| `logo` | `const std::string &` | Path of the logo. |
| `section` | `const std::string &` | Section of the menu where the item is displayed. |

```cpp
vxe::push_edit_menu_item("My action", []() { vxe::log_info("My module", "Clicked"); }, "icons/action.png", "My section");
```

---

### vxe::pop_edit_menu_item

Removes the last items added to the edit menu.

```cpp
void vxe::pop_edit_menu_item(const int &count);
```

| Parameter | Type | Description |
|---|---|---|
| `count` | `const int &` | Number of items to remove. |

```cpp
vxe::pop_edit_menu_item(1);
```

---

### vxe::clear_edit_menu_item

Removes all the items of the edit menu.

```cpp
void vxe::clear_edit_menu_item();
```

```cpp
vxe::clear_edit_menu_item();
```

---

### vxe::push_custom_menu

Adds a custom menu.

```cpp
void vxe::push_custom_menu(const std::string &title, const std::function<void()> &render);
```

| Parameter | Type | Description |
|---|---|---|
| `title` | `const std::string &` | Title of the menu. |
| `render` | `const std::function<void()> &` | Function rendering the content of the menu. |

```cpp
vxe::push_custom_menu("My menu", []() { /* Render the menu */ });
```

---

### vxe::pop_custom_menu

Removes the last custom menus added.

```cpp
void vxe::pop_custom_menu(const int &count);
```

| Parameter | Type | Description |
|---|---|---|
| `count` | `const int &` | Number of menus to remove. |

```cpp
vxe::pop_custom_menu(1);
```

---

### vxe::clear_custom_menus

Removes all the custom menus.

```cpp
void vxe::clear_custom_menus();
```

```cpp
vxe::clear_custom_menus();
```

---

### vxe::enable_modules_section_on_toolbar

Displays the modules section on the toolbar.

```cpp
void vxe::enable_modules_section_on_toolbar();
```

```cpp
vxe::enable_modules_section_on_toolbar();
```

---

### vxe::disable_modules_section_on_toolbar

Hides the modules section on the toolbar.

```cpp
void vxe::disable_modules_section_on_toolbar();
```

```cpp
vxe::disable_modules_section_on_toolbar();
```

---

# Credits

The credits are organized by topic, with sections.

A credits file contains a block between `-----BEGIN CREDITS-----` and `-----END CREDITS-----`. A line starting with `#` defines the title of a section. The other lines are the entries: a name, followed by an optional contact in `<>` or `()`.

```
-----BEGIN CREDITS-----
# Authors
Jane Doe <jane@example.com>
John Doe (website.com)

# Contributors
Alex
-----END CREDITS-----
```

### vxe::set_credits_file

Sets the credits of a topic from a credits file. The existing credits of the topic are replaced.

```cpp
void vxe::set_credits_file(const std::string &topic, const std::string &credit_file);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the credits. |
| `credit_file` | `const std::string &` | Path of the credits file. |

```cpp
vxe::set_credits_file("my_topic", "assets/CREDITS");
```

---

### vxe::add_credits

Adds the credits of a credits file to a topic.

```cpp
void vxe::add_credits(const std::string &topic, const std::string &credit_file);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the credits. |
| `credit_file` | `const std::string &` | Path of the credits file. |

```cpp
vxe::add_credits("my_topic", "assets/CREDITS");
```

---

### vxe::get_titles_from_topic

Gets the titles of the sections of the credits of a topic, in order.

```cpp
std::vector<std::string> vxe::get_titles_from_topic(const std::string &topic);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the credits. |

**Returns:** the titles of the sections.

```cpp
auto titles = vxe::get_titles_from_topic("my_topic");
```

---

### vxe::get_names_from_topic_and_title

Gets the names of a section of the credits of a topic. The contact is added after the name, as `<contact>` if it is an email, or `(contact)` otherwise.

```cpp
std::vector<std::string> vxe::get_names_from_topic_and_title(const std::string &topic, const std::string &title);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the credits. |
| `title` | `const std::string &` | Title of the section. |

**Returns:** the names of the section.

```cpp
auto names = vxe::get_names_from_topic_and_title("my_topic", "Authors");
```

---

# Documentation

The embedded documentation is organized by topic, with sections and chapters. Each chapter is a markdown file.

### vxe::add_documentation

Adds a chapter to the documentation.

```cpp
void vxe::add_documentation(
    const std::string &topic,
    const std::string &section,
    const std::string &title,
    const std::string &md_file_path);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the documentation. |
| `section` | `const std::string &` | The section of the chapter. |
| `title` | `const std::string &` | Title of the chapter. |
| `md_file_path` | `const std::string &` | Path of the markdown file of the chapter. |

```cpp
vxe::add_documentation("my_topic", "Getting started", "Introduction", "docs/intro.md");
```

---

### vxe::get_sections

Gets the sections of a topic.

```cpp
std::vector<std::string> vxe::get_sections(const std::string &topic);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the documentation. |

**Returns:** the names of the sections.

```cpp
auto sections = vxe::get_sections("my_topic");
```

---

### vxe::get_chapters

Gets the chapters of a section.

```cpp
std::vector<std::string> vxe::get_chapters(const std::string &topic, const std::string &section);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the documentation. |
| `section` | `const std::string &` | The section. |

**Returns:** the titles of the chapters.

```cpp
auto chapters = vxe::get_chapters("my_topic", "Getting started");
```

---

### vxe::get_chapter_file_path

Gets the path of the markdown file of a chapter.

```cpp
std::string vxe::get_chapter_file_path(const std::string &topic, const std::string &section, const std::string &title);
```

| Parameter | Type | Description |
|---|---|---|
| `topic` | `const std::string &` | The topic of the documentation. |
| `section` | `const std::string &` | The section. |
| `title` | `const std::string &` | Title of the chapter. |

**Returns:** the path of the file, or an empty string if the chapter is not found.

```cpp
std::string path = vxe::get_chapter_file_path("my_topic", "Getting started", "Introduction");
```

---

# Scripting

The scripting engine runs the Lua scripts of Vortex. Its functions are in the `vxe::script` namespace.

### vxe::script::get_scripting_engine

Returns the scripting engine of Vortex.

```cpp
ScriptingEngine &vxe::script::get_scripting_engine();
```

**Returns:** the scripting engine.

```cpp
auto &engine = vxe::script::get_scripting_engine();
```

---

### execute

Executes Lua code.

```cpp
bool execute(const std::string &code);
```

| Parameter | Type | Description |
|---|---|---|
| `code` | `const std::string &` | The Lua code to execute. |

**Returns:** `true` if the code was executed without error. Errors are logged.

```cpp
engine.execute("Vortex.Log('Hello')");
```

---

### load_file

Loads and runs a Lua file.

```cpp
int load_file(const std::string &path, int nargs);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the Lua file. |
| `nargs` | `int` | Number of arguments given to the script. |

**Returns:** the number of values returned by the script, or `0` if an error happened.

```cpp
engine.load_file("scripts/my_script.lua", 0);
```

---

### reload_file

Reloads a Lua file.

```cpp
void reload_file(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the Lua file. |

```cpp
engine.reload_file("scripts/my_script.lua");
```

---

### enable_hot_reload

Enables the hot reload of the scripts.

```cpp
void enable_hot_reload(const std::string &scriptDir);
```

| Parameter | Type | Description |
|---|---|---|
| `scriptDir` | `const std::string &` | The folder of the scripts. |

```cpp
engine.enable_hot_reload("scripts");
```

---

### check_for_changes

Reloads the Lua files that changed since they were loaded. It does nothing if the hot reload is not enabled.

```cpp
void check_for_changes();
```

```cpp
engine.check_for_changes();
```

---

### get_last_output

Returns the last output of the scripts.

```cpp
std::string get_last_output() const;
```

**Returns:** the last output, or an empty string if there is none.

```cpp
std::string output = engine.get_last_output();
```

---

### get_last_error

Returns the last error of the scripts.

```cpp
std::string get_last_error() const;
```

**Returns:** the last error.

```cpp
std::string error = engine.get_last_error();
```

---

### get_output_history

Returns the history of the outputs of the scripts.

```cpp
const std::vector<std::string> &get_output_history() const;
```

**Returns:** the outputs.

```cpp
for (auto &output : engine.get_output_history())
  std::cout << output << std::endl;
```

---

### clear_output

Clears the history of the outputs of the scripts.

```cpp
void clear_output();
```

```cpp
engine.clear_output();
```

---

### vxe::script::render_lua_script

Runs a Lua script.

```cpp
void vxe::script::render_lua_script(const std::string &lua_file_path);
```

| Parameter | Type | Description |
|---|---|---|
| `lua_file_path` | `const std::string &` | Path of the Lua file. |

```cpp
vxe::script::render_lua_script("scripts/my_script.lua");
```

---

### vxe::script::render_lua_fresh_script

Runs a Lua script, and enables the hot reload for the folder of the script.

```cpp
void vxe::script::render_lua_fresh_script(const std::string &lua_file_path);
```

| Parameter | Type | Description |
|---|---|---|
| `lua_file_path` | `const std::string &` | Path of the Lua file. |

```cpp
vxe::script::render_lua_fresh_script("scripts/my_script.lua");
```

---

# Files and JSON

### vxe::search_files

Searches files in a folder. A relative path is relative to the current working directory. Two versions are available.

```cpp
std::vector<std::string> vxe::search_files(const std::string &path, const std::string &filename);
std::vector<std::string> vxe::search_files(const std::string &path, const std::string &filename, int recursions);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The folder to search in. |
| `filename` | `const std::string &` | Name of the file. |
| `recursions` | `int` | Limit of the number of folder levels searched. |

- Without `recursions`: searches recursively the files named exactly `filename`.
- With `recursions`: searches the files whose name contains `filename`, with a limit on the number of folder levels.

**Returns:** the paths of the files found.

```cpp
auto files = vxe::search_files("modules", "module.json");
```

---

### vxe::search_system_files

Searches recursively the files whose name contains `filename`.

```cpp
std::vector<std::string> vxe::search_system_files(const std::string &path, const std::string &filename);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The folder to search in. |
| `filename` | `const std::string &` | Part of the name of the files. |

**Returns:** the paths of the files found.

```cpp
auto files = vxe::search_system_files("/path/to/folder", "config");
```

---

### vxe::search_files_recursive

Searches recursively the files whose name contains `filename`, and adds them to a list.

```cpp
void vxe::search_files_recursive(const fs::path &path, const std::string &filename, std::vector<std::string> &file);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const fs::path &` | The folder to search in. |
| `filename` | `const std::string &` | Part of the name of the files. |
| `file` | `std::vector<std::string> &` | Receives the paths of the files found. |

```cpp
std::vector<std::string> files;
vxe::search_files_recursive("/path/to/folder", "config", files);
```

---

### vxe::get_home_directory

Returns the home directory of the user.

```cpp
std::string vxe::get_home_directory();
```

**Returns:** the path of the home directory. An exception is thrown if it cannot be determined.

```cpp
std::string home = vxe::get_home_directory();
```

---

### vxe::copy_paste_directories

Copies a file or a folder into a target folder. If the name already exists in the target, the copy is named with `copy`.

```cpp
void vxe::copy_paste_directories(const std::string &origin_path_str, const std::string &target_path_str);
```

| Parameter | Type | Description |
|---|---|---|
| `origin_path_str` | `const std::string &` | Path of the file or folder to copy. |
| `target_path_str` | `const std::string &` | The existing folder where it is copied. |

```cpp
vxe::copy_paste_directories("/path/to/file.txt", "/path/to/folder");
```

---

### vxe::copy_directory_recursively

Copies the content of a folder into another one.

```cpp
void vxe::copy_directory_recursively(const fs::path &src, const fs::path &dest, const fs::path &destRoot);
```

| Parameter | Type | Description |
|---|---|---|
| `src` | `const fs::path &` | The folder to copy. |
| `dest` | `const fs::path &` | The destination folder. |
| `destRoot` | `const fs::path &` | The root of the destination. It is not copied again, to avoid copying a folder into itself. |

```cpp
vxe::copy_directory_recursively("/path/to/src", "/path/to/dest", "/path/to/dest");
```

---

### vxe::is_strict_sub_path

Checks if a path is strictly inside another one.

```cpp
bool vxe::is_strict_sub_path(const fs::path &potentialSub, const fs::path &base);
```

| Parameter | Type | Description |
|---|---|---|
| `potentialSub` | `const fs::path &` | The path to check. |
| `base` | `const fs::path &` | The base path. |

**Returns:** `true` if `potentialSub` is inside `base`.

```cpp
bool inside = vxe::is_strict_sub_path("/path/to/folder/sub", "/path/to/folder");
```

---

### vxe::create_folder_if_not_exists

Creates a folder (and its parents) if it does not exist.

```cpp
void vxe::create_folder_if_not_exists(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |

```cpp
vxe::create_folder_if_not_exists("/path/to/folder");
```

---

### vxe::create_file_if_not_exists

Creates a JSON file with default data if it does not exist.

```cpp
void vxe::create_file_if_not_exists(const std::string &filename, const nlohmann::json &defaultData);
```

| Parameter | Type | Description |
|---|---|---|
| `filename` | `const std::string &` | Path of the file. |
| `defaultData` | `const nlohmann::json &` | The data written in the file. |

```cpp
vxe::create_file_if_not_exists("config.json", { { "enabled", true } });
```

---

### vxe::file_exists

Checks if a regular file exists.

```cpp
bool vxe::file_exists(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the file. |

**Returns:** `true` if the file exists.

```cpp
bool exists = vxe::file_exists("config.json");
```

---

### vxe::dump_json

Reads a JSON file. An error is logged if the file does not exist or cannot be opened.

```cpp
nlohmann::json vxe::dump_json(const std::string &file);
```

| Parameter | Type | Description |
|---|---|---|
| `file` | `const std::string &` | Path of the JSON file. |

**Returns:** the content of the file.

```cpp
auto data = vxe::dump_json("config.json");
```

---

### vxe::populate_json

Writes JSON data in a file. The folder of the file is created if needed.

```cpp
void vxe::populate_json(const nlohmann::json &json_data, const std::string &file);
```

| Parameter | Type | Description |
|---|---|---|
| `json_data` | `const nlohmann::json &` | The data to write. |
| `file` | `const std::string &` | Path of the file. |

```cpp
vxe::populate_json({ { "enabled", true } }, "config.json");
```

---

# Web and installation

### Release format

The install functions use a release, as a JSON object:

| Key | Description |
|---|---|
| `files` | Array of the URLs of the files of the release. The first one is installed. |
| `metadata.sums` | Array of objects, associating the URL of a file to its sha256 sum. |
| `uuid` | Identifier of the release. |

### ModuleInstallProgress

The installation functions report their progress in a `ModuleInstallProgress`.

| Member | Description |
|---|---|
| `state` | State of the installation, an atomic `ModuleInstallState`. |
| `status_message` | Message describing the installation. |
| `install_path` | Folder where the release was installed. |
| `mutex` | Mutex protecting the members. |

| Function | Description |
|---|---|
| `set_status(message)` | Sets the status message. |
| `set_error(message)` | Sets an error. |

**ModuleInstallState**

| Value | Description |
|---|---|
| `CheckingIntegrityInfo` | Verification of the release informations. |
| `CreatingDirs` | Preparation of the installation folder. |
| `Downloading` | Download of the file. |
| `VerifyingChecksum` | Verification of the sha256 sum. |
| `Extracting` | Extraction of the archive. |
| `Done` | Installation finished. |

---

### vxe::install_module_release_async

Installs a release of a module in the modules folder of the project, in the background.

The steps are: verification of the release informations (a sha256 sum is required), preparation of the folders, download, verification of the sha256 sum, extraction, and notification of the download. If a step fails, the error is set in `progress`.

```cpp
void vxe::install_module_release_async(
    const nlohmann::json &release_json,
    const std::string &parent_uuid,
    std::shared_ptr<ModuleInstallProgress> progress);
```

| Parameter | Type | Description |
|---|---|---|
| `release_json` | `const nlohmann::json &` | The release to install. |
| `parent_uuid` | `const std::string &` | Identifier of the module the release belongs to. |
| `progress` | `std::shared_ptr<ModuleInstallProgress>` | Receives the progress of the installation. |

```cpp
void install(const nlohmann::json &release, std::shared_ptr<ModuleInstallProgress> progress) {
  vxe::install_module_release_async(release, "parent_uuid", progress);
}
```

---

### vxe::install_plugin_release_async

Installs a release of a plugin in the plugins folder of the project, in the background. The steps are the same as for a module.

```cpp
void vxe::install_plugin_release_async(
    const nlohmann::json &release_json,
    const std::string &parent_uuid,
    std::shared_ptr<ModuleInstallProgress> progress);
```

| Parameter | Type | Description |
|---|---|---|
| `release_json` | `const nlohmann::json &` | The release to install. |
| `parent_uuid` | `const std::string &` | Identifier of the plugin the release belongs to. |
| `progress` | `std::shared_ptr<ModuleInstallProgress>` | Receives the progress of the installation. |

```cpp
vxe::install_plugin_release_async(release, "parent_uuid", progress);
```

---

### vxe::install_content_release_async

Installs a release of a content in a destination folder, in the background. The steps are the same as for a module.

```cpp
void vxe::install_content_release_async(
    const nlohmann::json &release_json,
    const std::string &parent_uuid,
    const std::string &destination_path,
    std::shared_ptr<ModuleInstallProgress> progress);
```

| Parameter | Type | Description |
|---|---|---|
| `release_json` | `const nlohmann::json &` | The release to install. |
| `parent_uuid` | `const std::string &` | Identifier of the content the release belongs to. |
| `destination_path` | `const std::string &` | The folder where the content is installed. |
| `progress` | `std::shared_ptr<ModuleInstallProgress>` | Receives the progress of the installation. |

```cpp
vxe::install_content_release_async(release, "parent_uuid", "/path/to/destination", progress);
```

---

### vxe::notify_module_download

Notifies the server that a module was downloaded.

```cpp
void vxe::notify_module_download(const std::string &parent_uuid);
```

| Parameter | Type | Description |
|---|---|---|
| `parent_uuid` | `const std::string &` | Identifier of the module. |

```cpp
vxe::notify_module_download("parent_uuid");
```

---

### vxe::notify_plugin_download

Notifies the server that a plugin was downloaded.

```cpp
void vxe::notify_plugin_download(const std::string &parent_uuid);
```

| Parameter | Type | Description |
|---|---|---|
| `parent_uuid` | `const std::string &` | Identifier of the plugin. |

```cpp
vxe::notify_plugin_download("parent_uuid");
```

---

### vxe::notify_content_download

Notifies the server that a content was downloaded.

```cpp
void vxe::notify_content_download(const std::string &parent_uuid);
```

| Parameter | Type | Description |
|---|---|---|
| `parent_uuid` | `const std::string &` | Identifier of the content. |

```cpp
vxe::notify_content_download("parent_uuid");
```

---

### vxe::download_file

Downloads a file with `curl`.

```cpp
bool vxe::download_file(const std::string &url, const std::string &outPath, std::string &errorOut);
```

| Parameter | Type | Description |
|---|---|---|
| `url` | `const std::string &` | URL of the file. |
| `outPath` | `const std::string &` | Path where the file is saved. |
| `errorOut` | `std::string &` | Receives the error message if the download fails. |

**Returns:** `true` if the file was downloaded and is not empty.

```cpp
std::string error;
if (!vxe::download_file("https://example.com/file.tar.gz", "/tmp/file.tar.gz", error))
  vxe::log_error("My module", error);
```

---

### vxe::compute_file_sha256

Computes the sha256 sum of a file.

```cpp
bool vxe::compute_file_sha256(const std::string &filePath, std::string &outSum, std::string &errorOut);
```

| Parameter | Type | Description |
|---|---|---|
| `filePath` | `const std::string &` | Path of the file. |
| `outSum` | `std::string &` | Receives the sum, in lowercase. |
| `errorOut` | `std::string &` | Receives the error message if the sum cannot be computed. |

**Returns:** `true` if the sum was computed.

```cpp
std::string sum, error;
if (vxe::compute_file_sha256("/tmp/file.tar.gz", sum, error))
  vxe::log_info("My module", sum);
```

---

### vxe::compare_sha256

Compares two sha256 sums, ignoring the case and the spaces around.

```cpp
bool vxe::compare_sha256(const std::string &a, const std::string &b);
```

| Parameter | Type | Description |
|---|---|---|
| `a` | `const std::string &` | The first sum. |
| `b` | `const std::string &` | The second sum. |

**Returns:** `true` if the sums are the same.

```cpp
bool same = vxe::compare_sha256(sum, expected_sum);
```

---

### vxe::extract_tar

Extracts a `.tar.gz` archive into a folder. The folder is created if needed.

```cpp
bool vxe::extract_tar(const std::string &archivePath, const std::string &destDir, std::string &errorOut);
```

| Parameter | Type | Description |
|---|---|---|
| `archivePath` | `const std::string &` | Path of the archive. |
| `destDir` | `const std::string &` | The folder where the archive is extracted. |
| `errorOut` | `std::string &` | Receives the error message if the extraction fails. |

**Returns:** `true` if the archive was extracted.

```cpp
std::string error;
vxe::extract_tar("/tmp/file.tar.gz", "/path/to/folder", error);
```

---

### vxe::is_curl_available

Checks if `curl` is available on the system.

```cpp
bool vxe::is_curl_available();
```

**Returns:** `true` if `curl` is available.

```cpp
bool curl = vxe::is_curl_available();
```

---

### vxe::is_tar_available

Checks if `tar` is available on the system.

```cpp
bool vxe::is_tar_available();
```

**Returns:** `true` if `tar` is available.

```cpp
bool tar = vxe::is_tar_available();
```

---

### Install folders

| Function | Returns |
|---|---|
| `std::string vxe::get_modules_directory()` | The modules folder of the project (`.vx/modules`). The folder is created if needed. |
| `std::string vxe::get_plugins_directory()` | The plugins folder of the project (`.vx/plugins`). The folder is created if needed. |
| `std::string vxe::get_module_install_temp_directory()` | The temporary folder used to install the modules. |
| `std::string vxe::get_plugin_install_temp_directory()` | The temporary folder used to install the plugins. |
| `std::string vxe::get_content_install_temp_directory()` | The temporary folder used to install the contents. |

```cpp
std::string modules = vxe::get_modules_directory();
```

---

# Utilities

### vxe::get_current_timestamp

Returns the current date and time.

```cpp
std::string vxe::get_current_timestamp();
```

**Returns:** the date, formatted as `%Y-%m-%d %H:%M:%S`.

```cpp
std::string now = vxe::get_current_timestamp();
```

---

### vxe::gen_random

Generates a random string of letters and digits.

```cpp
std::string vxe::gen_random(int len);
```

| Parameter | Type | Description |
|---|---|---|
| `len` | `int` | Length of the string. |

**Returns:** the random string.

```cpp
std::string id = vxe::gen_random(8);
```

---

### vxe::open_url

Opens a URL with the default application of the system.

```cpp
void vxe::open_url(const std::string &url);
```

| Parameter | Type | Description |
|---|---|---|
| `url` | `const std::string &` | The URL to open. |

```cpp
vxe::open_url("https://infinite.si");
```

---

### vxe::run_command_capture_output

Runs a command and captures its output.

```cpp
bool vxe::run_command_capture_output(const std::string &cmd, std::string &output);
```

| Parameter | Type | Description |
|---|---|---|
| `cmd` | `const std::string &` | The command to run. |
| `output` | `std::string &` | Receives the output of the command. |

**Returns:** `true` if the command was started.

```cpp
std::string output;
vxe::run_command_capture_output("curl --version", output);
```

---

### vxe::replace_placeholders

Replaces placeholders in a string.

```cpp
std::string vxe::replace_placeholders(
    const std::string &command,
    const std::unordered_map<std::string, std::string> &replacements);
```

| Parameter | Type | Description |
|---|---|---|
| `command` | `const std::string &` | The string containing the placeholders. |
| `replacements` | `const std::unordered_map<std::string, std::string> &` | The placeholders, with their values. |

**Returns:** the string with the placeholders replaced.

```cpp
std::string result = vxe::replace_placeholders("hello {name}", { { "{name}", "world" } });
```

---

### vxe::convert_path_to_windows_style

Replaces `/` with `\` in a path.

```cpp
std::string vxe::convert_path_to_windows_style(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | The path to convert. |

**Returns:** the converted path.

```cpp
std::string path = vxe::convert_path_to_windows_style("path/to/file");
```

---

### vxe::filename_from_url

Gets the file name of a URL, without the query. Returns `download.tmp` if there is none.

```cpp
std::string vxe::filename_from_url(const std::string &url);
```

| Parameter | Type | Description |
|---|---|---|
| `url` | `const std::string &` | The URL. |

**Returns:** the file name.

```cpp
std::string name = vxe::filename_from_url("https://example.com/file.tar.gz?token=1");
```

---

### vxe::ensure_directory_exists

Makes sure a folder exists, and creates it if needed.

```cpp
bool vxe::ensure_directory_exists(const std::string &path);
```

| Parameter | Type | Description |
|---|---|---|
| `path` | `const std::string &` | Path of the folder. |

**Returns:** `true` if the folder exists, or was created.

```cpp
bool ok = vxe::ensure_directory_exists("/path/to/folder");
```

---

### vxe::to_lower

Converts a string to lowercase.

```cpp
std::string vxe::to_lower(std::string s);
```

| Parameter | Type | Description |
|---|---|---|
| `s` | `std::string` | The string to convert. |

**Returns:** the string in lowercase.

```cpp
std::string lower = vxe::to_lower("Hello");
```

---

### vxe::trim

Removes the spaces, tabs and line breaks at the start and the end of a string.

```cpp
std::string vxe::trim(const std::string &s);
```

| Parameter | Type | Description |
|---|---|---|
| `s` | `const std::string &` | The string to trim. |

**Returns:** the trimmed string.

```cpp
std::string trimmed = vxe::trim("  hello  ");
```

---

### vxe::base64_encode

Encodes a string in base64.

```cpp
std::string vxe::base64_encode(const std::string &data);
```

| Parameter | Type | Description |
|---|---|---|
| `data` | `const std::string &` | The data to encode. |

**Returns:** the encoded string.

```cpp
std::string encoded = vxe::base64_encode("hello");
```

---

### vxe::base64_decode

Decodes a base64 string.

```cpp
std::string vxe::base64_decode(const std::string &encoded);
```

| Parameter | Type | Description |
|---|---|---|
| `encoded` | `const std::string &` | The base64 string. |

**Returns:** the decoded data.

```cpp
std::string decoded = vxe::base64_decode(encoded);
```

---

### vxe::is_base64

Checks if a character is valid in a base64 string.

```cpp
bool vxe::is_base64(unsigned char c);
```

| Parameter | Type | Description |
|---|---|---|
| `c` | `unsigned char` | The character. |

**Returns:** `true` if the character is a letter, a digit, `+` or `/`.

```cpp
bool valid = vxe::is_base64('a');
```

---

### Flashlinks

A flashlink is a prefix followed by a UUID. These functions check if a string is a flashlink of a given type.

| Function | Format |
|---|---|
| `bool vxe::is_module_flashlink(const std::string &str)` | `mod:<uuid>` |
| `bool vxe::is_plugin_flashlink(const std::string &str)` | `plu:<uuid>` |
| `bool vxe::is_content_flashlink(const std::string &str)` | `con:<uuid>` |
| `bool vxe::is_template_flashlink(const std::string &str)` | `tem:<uuid>` |

```cpp
bool is_module = vxe::is_module_flashlink("mod:123e4567-e89b-12d3-a456-426614174000");
```
