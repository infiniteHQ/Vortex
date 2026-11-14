# Vortex Core Events API (Modules)

This page shows the events that the Vortex core itself provides, and how to use them from a C++ module. There are only two of them, and they work in opposite directions:

- `save_all` is an **output event**. The core broadcasts it, and your module listens to it.
- `spawn_content_browser` is an **input event**. The core offers it, and your module calls it.

If you are new to events, here is the short version. An input event is like a door a module (or the core) opens: others can call it by name. An output event is a broadcast: someone shouts, and everyone who registered a handler for it reacts. The full tools are described in the [Module C++ API Reference](#) and in the module logic guide.

## save_all

### What it is

When the user saves everything, for example with the "Save all" button of the content browser, the core broadcasts the output event `save_all` to all the modules and plugins. There is no argument and no return value to care about: it simply means "now is the time to save your work".

Vortex does not save your data for you. If your module has something to save (open files, settings, a project in memory), you must add a handler for `save_all`, otherwise your module will ignore the user's save.

### Handle it

Add the handler in `execute()`, with `add_output_event`:

```cpp
void execute() override {
  add_output_event([this]() {
    log_info("Save all event triggered");
  }, "save_all");
}
```

In a real module, the handler usually goes through your context and asks each part of the module to save itself:

```cpp
// main.cpp
this->add_output_event(ModuleOutputEvent(HelloWorldModule::oe_save_all, "save_all"));
```

```cpp
// module.cpp
void HelloWorldModule::oe_save_all() {
  auto ctx = get_current_context();
  if (!ctx)
    return;

  for (auto &window : ctx->m_text_editor_instances)
    window->save();
}
```

A few good habits. Check that the context still exists before using it. Keep the handler quick, since the user is waiting for the save and every module with a handler runs on the same event. And log what you did, so a failed save leaves a trace in the logs utility.

### Trigger it yourself

The core triggers `save_all` when the user asks for it, so you normally never need to. If your module has its own "Save everything" action and you want the whole Vortex to follow, you can broadcast the event too:

```cpp
vxe::trigger_save_all_event();
```

This broadcasts `save_all` with the origin `"vortex"`. Every handler registered by modules and plugins will run, including yours.

## spawn_content_browser

### What it is

`spawn_content_browser` is an input event of the core. When you call it, Vortex opens a new content browser. It needs no argument.

### Call it

From your module, use `call_input_event` with `"vortex"` as the target and the event name:

```cpp
ArgumentValues args;
ReturnValues ret;
call_input_event("vortex", "spawn_content_browser", args, ret);
```

A nice use is to put it behind a toolbar entry of your module:

```cpp
this->add_toolbar_handler(ToolbarHandlerInterface(
    []() {
      ArgumentValues args;
      ReturnValues ret;
      vxe::call_input_event("vortex", "spawn_content_browser", args, ret, "my_module");
    },
    "New content browser",
    "Open another content browser",
    "My module",
    HelloWorldModule::get_path("resources/icons/browser.png")));
```

Inside a lambda that is not part of your module class, use the global version `vxe::call_input_event`. It has one more parameter, the origin, which is the name of the caller.

### From outside a module class

The same call works anywhere in your code, because the `vxe` namespace is available everywhere:

```cpp
ArgumentValues args;
ReturnValues ret;
vxe::call_input_event("vortex", "spawn_content_browser", args, ret, "my_module");
```

## Troubleshooting

If your `save_all` handler never runs, first check the name: it must be exactly `save_all`, in lowercase with the underscore. Then make sure the handler is added in `execute()` and that your module is really running and not in the failed state. If the handler runs when you trigger the event yourself but not when the user saves, look at the logs to see if another part of Vortex reports an error.

If calling `spawn_content_browser` does nothing, look at the logs. When an input event is not found, Vortex logs an error, and the most common cause is a typo in the target (`"vortex"`) or in the event name.

If your module crashes during a save, the handler is probably using something that no longer exists, usually the context or a window that was closed. Check that the context is not null and that closed windows are removed from your list.