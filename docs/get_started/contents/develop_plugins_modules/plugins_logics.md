This section shows you basic stuffs on logical programming of Vortex Editor plugins in Lua

### Utilities

#### Get relative path of plugin
You can get the relative path of plugin with the GetPath function.

```lua
-- return plugin_path/
local root_path = Vortex.GetPath()

-- return plugin_path/assets
local assets_path = Vortex.GetPath("assets")
```

### Call events
One of the main usage of a plugin is to call events from other modules/plugins or vortex core. You can call input or output events.

(A input event is like a event handler decalred by a module or a plugin that can be called from outside)

(A output event is a broadcast from one module/plugin/core to all listeners.)

#### Call input event

You can call a specific input event, like the content browser spawner in the vortex code like the following example.

You need to specify the target in the first parameter and the event name on the second parameter.

```lua
-- We call the content browser spawn of vortex (the core)
Vortex.CallInputEvent("vortex", "spawn_content_browser")
```

#### Call output event

You can broadcast an output event to everyone having a handler for this specific event.

You only need to specify the event name and optionnally arguments.

```lua
-- This is an example that will trigger every modules/plugin or the core that handle this event
Vortex.CallOutputEvent("just_save", "path")
```

### Add events handler

#### Output event handler
You can add a handler to a output event, and then, if Vortex or another module/plugin will trigger this event you will be able to handle it.


```lua
Vortex.AddOutputEvent("save_all", function(args)
    -- Code executed at event trigger.
    Vortex.Log("Save all event triggered !")
end, "none")
```

In this example, we add a handler for the event "save_all", this event can be triggered by the "Save all" button of the content browser, or any other modules/plugins


#### Input event handler
You can also add input event handler to handle specific targeting from other modules/plugins/core


```lua
-- This input event is exclusive to the plugin, others needs to call it + the plugin name, it's specific
Vortex.AddInputEvent("set_theme", function(args)
  Vortex.Log("Theme : " .. args.name)
  return { ok = true }
end, "none")
```

### Content browser

The plugins allows you to add handlers and customize the content browser.

#### Add item handler
You can add a custom handler for a specific type of item. You can handle generic items by extensions (like png, json), or also handle special modules items.

```lua
-- You can handle generic items
Vortex.AddContentBrowserItemHandler(
  "png",
  function(path) Vortex.Log("Opening " .. path) end,
  "Open picture",
  "Open picture in photo viewer",
  "icons/image.png"
)

-- Or event specific items declared by other modules
Vortex.AddContentBrowserItemHandler(
  "efusion:firmware_nodegraph",
  function(path) Vortex.Log("Opening " .. path) end,
  "See properties",
  "Open nodegraph properties viewer",
  "icons/image.png"
)
```

### Customize plugin

By it's code, you can define certains plugin properties, such as credit files, logo etc.

```lua
-- Set the plugin logo
Vortex.AddLogo("assets/logo.png")


-- Set the credits file
Vortex.SetCreditsFile("assets/CREDITS")
```

### Logs

You can log from this plugin and display logs to the logs utility

```lua
Vortex.Log("Cool info")
Vortex.LogWarn("Be careful")
Vortex.LogError("Unable to do this or that")
Vortex.LogFatal("File unrecoverable")
```