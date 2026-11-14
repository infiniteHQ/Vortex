# Vortex Core Events API (Plugins)

This page shows the events that the Vortex core itself provides, and how to use them from a Lua plugin. There are only two of them, and they work in opposite directions:

- `save_all` is an **output event**. The core broadcasts it, and your plugin listens to it.
- `spawn_content_browser` is an **input event**. The core offers it, and your plugin calls it.

If you are new to events, here is the short version. An input event is like a door a plugin, a module or the core opens: others can call it by name. An output event is a broadcast: someone shouts, and everyone who registered a handler for it reacts. The basics of plugin logic are described in the Lua plugin API reference.

## save_all

### What it is

When the user saves everything, for example with the "Save all" button of the content browser, the core broadcasts the output event `save_all` to all the modules and plugins. It carries nothing useful: it simply means "now is the time to save your work".

Vortex does not save your data for you. If your plugin has something to save (a file, some settings, a state kept in a table), you must add a handler for `save_all`, otherwise your plugin will ignore the user's save.

### Handle it

Add the handler with `Vortex.AddOutputEvent`:

```lua
Vortex.AddOutputEvent("save_all", function(args)
    Vortex.Log("Save all event triggered!")
end, "none")
```

The function is executed every time the event is triggered. In a real plugin, this is where you write your data:

```lua
local settings = { last_file = "notes.txt" }

Vortex.AddOutputEvent("save_all", function(args)
    save_settings(settings)
    Vortex.Log("Settings saved")
end, "none")
```

Keep the handler quick, because the user is waiting for the save and every plugin and module with a handler runs on the same event. And log what you did, so a failed save leaves a trace in the logs utility.

### Trigger it yourself

The core triggers `save_all` when the user asks for it, so you normally never need to. If your plugin has its own "Save everything" action and you want the whole Vortex to follow, you can broadcast the event too:

```lua
Vortex.CallOutputEvent("save_all")
```

Every handler registered by modules and plugins will run, including yours.

## spawn_content_browser

### What it is

`spawn_content_browser` is an input event of the core. When you call it, Vortex opens a new content browser. It needs no argument.

### Call it

Use `Vortex.CallInputEvent` with `"vortex"` as the target in the first parameter and the event name in the second:

```lua
Vortex.CallInputEvent("vortex", "spawn_content_browser")
```

You can call it from any code of your plugin. A simple way to offer it to others is to wrap it in an input event of your own:

```lua
Vortex.AddInputEvent("open_browser", function(args)
    Vortex.CallInputEvent("vortex", "spawn_content_browser")
    return { ok = true }
end, "none")
```

Remember that an input event declared by a plugin is specific to this plugin: others call it by using the name of your plugin as the target.

## Troubleshooting

If your `save_all` handler never runs, first check the name: it must be exactly `save_all`, in lowercase with the underscore. Then make sure the handler is added when the plugin loads, at the top level of your script and not inside a function that is never called. If the handler runs when you trigger the event yourself but not when the user saves, look at the logs to see if another part of Vortex reports an error.

If calling `spawn_content_browser` does nothing, look at the logs. When an input event is not found, Vortex logs an error, and the most common cause is a typo in the target (`"vortex"`) or in the event name.

If a script error appears in the logs during a save, the handler is probably using a variable or a function that is not defined yet. Keep the data your handler needs in variables declared before `Vortex.AddOutputEvent`, and test the handler by triggering the event yourself.