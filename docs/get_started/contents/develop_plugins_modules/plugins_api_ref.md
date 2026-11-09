# Plugin Lua API Reference

This page lists every function available in the `Vortex` table when programming a Vortex Editor plugin in Lua.

All these functions must be called from a plugin context. Calling most of them outside of it will raise an error (or do nothing for utilities and logs).

## Summary

| Category | Functions |
|---|---|
| Utilities | `GetPath`, `CookPath` |
| Logs | `Log`, `LogWarn`, `LogError`, `LogFatal` |
| Plugin customization | `AddLogo`, `SetCreditsFile` |
| Events | `AddInputEvent`, `CallInputEvent`, `AddOutputEvent`, `CallOutputEvent` |
| Content browser | `AddContentBrowserItemHandler` |
| Windows | `Window` |

---

## Utilities

### Vortex.GetPath

Returns the path of the plugin, optionally followed by a relative path.

```lua
string Vortex.GetPath([relative_path])
```

| Parameter | Type | Description |
|---|---|---|
| `relative_path` | string (optional) | Path appended to the plugin root. |

**Returns:** the full path as a string.

```lua
-- return plugin_path/
local root_path = Vortex.GetPath()

-- return plugin_path/assets
local assets_path = Vortex.GetPath("assets")
```

---

### Vortex.CookPath

Returns the cook path of the plugin, optionally followed by a relative path.

```lua
string Vortex.CookPath([relative_path])
```

| Parameter | Type | Description |
|---|---|---|
| `relative_path` | string (optional) | Path appended to the cook path. |

**Returns:** the full path as a string.

```lua
local cook_root = Vortex.CookPath()
local cooked_assets = Vortex.CookPath("assets")
```

---

## Logs

You can log from your plugin and display the logs in the logs utility. The plugin name is used as the log source.

### Vortex.Log

Logs an information message.

```lua
Vortex.Log(message)
```

| Parameter | Type | Description |
|---|---|---|
| `message` | string | The message to log. |

```lua
Vortex.Log("Cool info")
```

---

### Vortex.LogWarn

Logs a warning message.

```lua
Vortex.LogWarn(message)
```

| Parameter | Type | Description |
|---|---|---|
| `message` | string | The message to log. |

```lua
Vortex.LogWarn("Be careful")
```

---

### Vortex.LogError

Logs an error message.

```lua
Vortex.LogError(message)
```

| Parameter | Type | Description |
|---|---|---|
| `message` | string | The message to log. |

```lua
Vortex.LogError("Unable to do this or that")
```

---

### Vortex.LogFatal

Logs a fatal error message.

```lua
Vortex.LogFatal(message)
```

| Parameter | Type | Description |
|---|---|---|
| `message` | string | The message to log. |

```lua
Vortex.LogFatal("File unrecoverable")
```

---

## Plugin customization

### Vortex.AddLogo

Sets the logo of the plugin.

```lua
Vortex.AddLogo(path)
```

| Parameter | Type | Description |
|---|---|---|
| `path` | string | Path of the logo image, relative to the plugin. |

```lua
Vortex.AddLogo("assets/logo.png")
```

---

### Vortex.SetCreditsFile

Sets the credits file of the plugin.

```lua
Vortex.SetCreditsFile(path)
```

| Parameter | Type | Description |
|---|---|---|
| `path` | string | Path of the credits file, relative to the plugin. |

```lua
Vortex.SetCreditsFile("assets/CREDITS")
```

> `Vortex.SetCreditsFiles` is also available as an alias of `Vortex.SetCreditsFile`.

---

## Events

One of the main usages of a plugin is to call events from other modules, plugins or from the Vortex core.

- An **input event** is like an event handler declared by a module or a plugin, that can be called from outside.
- An **output event** is a broadcast from one module, plugin or the core to all listeners.

### Arguments and return values

Events exchange data as a single value, which can be a **string** or a **table**.

- **Tables** are flat key/value tables. Values can be strings, numbers or booleans. Any other value type (nested table, function...) is converted to `nil`.
- When a handler receives or returns a JSON object, it is converted to a Lua table. Nested values inside it are given as JSON strings.
- Anything that is not a JSON object is given as a string.

---

### Vortex.CallInputEvent

Calls a specific input event of a module, a plugin or the core.

```lua
any Vortex.CallInputEvent(target, event_name, [args])
```

| Parameter | Type | Description |
|---|---|---|
| `target` | string | Name of the module, plugin or core that owns the input event. |
| `event_name` | string | Name of the input event to call. |
| `args` | string or table (optional) | Arguments sent to the event. |

**Returns:** the value returned by the handler (table or string), or `nil` if there is none.

```lua
-- We call the content browser spawn of vortex (the core)
Vortex.CallInputEvent("vortex", "spawn_content_browser")

-- With arguments
local result = Vortex.CallInputEvent("my_plugin", "set_theme", { name = "dark" })
```

---

### Vortex.AddInputEvent

Adds an input event handler. The event is exclusive to the plugin: others need to call it with the plugin name as target.

```lua
Vortex.AddInputEvent(event_name, callback, [mode])
```

| Parameter | Type | Description |
|---|---|---|
| `event_name` | string | Name of the input event. |
| `callback` | function | Function executed when the event is called. Its signature depends on `mode`. |
| `mode` | string (optional) | What the callback receives and returns. Default is `"both"`. |

**Modes**

| Mode | Callback signature | Description |
|---|---|---|
| `"both"` | `function(args) return value end` | Receives the arguments and can return a value. |
| `"args"` | `function(args) end` | Receives the arguments, returns nothing. |
| `"ret"` | `function() return value end` | Receives nothing, can return a value. |
| `"none"` | `function() end` | Receives nothing, returns nothing. |

```lua
Vortex.AddInputEvent("set_theme", function(args)
  Vortex.Log("Theme : " .. args.name)
  return { ok = true }
end)
```

---

### Vortex.CallOutputEvent

Broadcasts an output event to every module, plugin or core having a handler for it.

```lua
any Vortex.CallOutputEvent(event_name, [args])
```

| Parameter | Type | Description |
|---|---|---|
| `event_name` | string | Name of the output event to broadcast. |
| `args` | string or table (optional) | Arguments sent to the listeners. |

**Returns:** the value returned by the handlers (table or string), or `nil` if there is none.

```lua
-- This will trigger every module/plugin or the core that handles this event
Vortex.CallOutputEvent("just_save", "path")
```

---

### Vortex.AddOutputEvent

Adds a handler to an output event. If Vortex or another module/plugin triggers this event, your handler is executed.

```lua
Vortex.AddOutputEvent(event_name, callback, [mode])
```

| Parameter | Type | Description |
|---|---|---|
| `event_name` | string | Name of the output event to listen to. |
| `callback` | function | Function executed when the event is triggered. Its signature depends on `mode`. |
| `mode` | string (optional) | What the callback receives and returns. Default is `"both"`. |

**Modes**

| Mode | Callback signature | Description |
|---|---|---|
| `"both"` | `function(args) return value end` | Receives the arguments and can return a value. |
| `"args"` | `function(args) end` | Receives the arguments, returns nothing. |
| `"ret"` | `function() return value end` | Receives nothing, can return a value. |
| `"none"` | `function() end` | Receives nothing, returns nothing. |

```lua
Vortex.AddOutputEvent("save_all", function()
  Vortex.Log("Save all event triggered !")
end, "none")
```

In this example, we add a handler for the event `save_all`. This event can be triggered by the "Save all" button of the content browser, or by any other module/plugin.

---

## Content browser

### Vortex.AddContentBrowserItemHandler

Adds a custom handler for a specific type of item in the content browser. You can handle generic items by extension (like `png`, `json`), or special module items.

```lua
Vortex.AddContentBrowserItemHandler(type, callback, label, description, [icon])
```

| Parameter | Type | Description |
|---|---|---|
| `type` | string | Item type to handle: a file extension (`png`) or a module item type (`module:item_type`). |
| `callback` | function | Function executed when the handler is used. Receives the path of the item as a string. |
| `label` | string | Name of the action shown in the content browser. |
| `description` | string | Description of the action. |
| `icon` | string (optional) | Path of the icon, relative to the plugin. |

```lua
-- You can handle generic items
Vortex.AddContentBrowserItemHandler(
  "png",
  function(path) Vortex.Log("Opening " .. path) end,
  "Open picture",
  "Open picture in photo viewer",
  "icons/image.png"
)

-- Or even specific items declared by other modules
Vortex.AddContentBrowserItemHandler(
  "efusion:firmware_nodegraph",
  function(path) Vortex.Log("Opening " .. path) end,
  "See properties",
  "Open nodegraph properties viewer",
  "icons/image.png"
)
```

---

## Windows

### Vortex.Window

Creates a custom window for the plugin and adds it to the application.

```lua
Vortex.Window(name, source)
```

| Parameter | Type | Description |
|---|---|---|
| `name` | string | Name of the window. |
| `source` | string or function | Either the path of a Lua file (relative to the plugin) that renders the window, or a render function called to draw the window. |

```lua
-- From a Lua file
Vortex.Window("My window", "ui/my_window.lua")

-- From a render function
Vortex.Window("My other window", function()
  -- Window rendering code
end)
```
