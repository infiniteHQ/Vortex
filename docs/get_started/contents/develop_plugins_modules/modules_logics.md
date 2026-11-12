# Develop module logics

This guide walks you through the logic part of a Vortex module: how a module is built, how it talks to Vortex, and how to add your own features to the content browser and the toolbar.

It is not a reference. For the full list of functions, see the [Module C++ API Reference](#) and the [Vortex API Reference](#). Here we go step by step, using a small `HelloWorld` module as the example.

A module is a C++ library that Vortex loads. Inside it, you can do anything C++ can do, including linking **any external library** you like (a text editor widget, an image library, a network client, a parser...). Vortex does not limit you: if it compiles and links, you can use it.

## Anatomy of a module

A typical module looks like this:

```
my_module/
├── main.cpp
├── CREDITS
├── resources/
│   └── icons/
├── src/
│   ├── module.hpp
│   └── module.cpp
└── ui/
    └── instances/
```

`main.cpp` is the entry point, `src/` holds your logic and your context, and `ui/` holds your windows (see the UI guide).

You only really need two things: a class that inherits from `ModuleInterface`, and an exported function named `create_em` that returns an instance of it.

```cpp
#include "./src/module.hpp"

class infinitehq_helloworldmodule : public ModuleInterface {
public:
  void execute() override {}
  void init_ui() override {}
  void destroy() override {}
};

#ifdef _WIN32
extern "C" __declspec(dllexport) ModuleInterface *create_em() {
  return new infinitehq_helloworldmodule();
}
#else
extern "C" ModuleInterface *create_em() {
  return new infinitehq_helloworldmodule();
}
#endif
```

`create_em` is how Vortex finds your module. On Windows it needs `__declspec(dllexport)`, so keep both versions.

## The lifecycle

Vortex calls three functions on your module.

`execute()` is called when the module starts, once the dependencies and the version are validated. This is where you create your context and register everything: handlers, toolbar entries, events.

`init_ui()` is called when the UI part starts. Load fonts and prepare anything visual here.

`destroy()` is called when the module stops. Clean up everything you created.

A rule of thumb: whatever you create in `execute`, you remove in `destroy`. A module can be stopped and started again while Vortex is running, so a clean `destroy` matters.

You can also start and stop a module by hand:

```cpp
auto module = ModuleInterface::get_editor_module_by_name("my_module");
if (module)
  module->start();
```

## The module context

Your module will quickly need to share data between its functions and its windows: the list of open windows, a pointer to the module interface, some settings.

The clean way is a small context struct, with a global pointer to it.

```cpp
// module.hpp
namespace HelloWorldModule {
struct Context {
  std::shared_ptr<ModuleInterface> m_interface;
  std::vector<std::shared_ptr<ModuleUI::HelloWindow>> m_text_editor_instances;
};

std::shared_ptr<Context> create_context();
void destroy_context(std::shared_ptr<Context> ctx);
void set_current_context(std::shared_ptr<Context> ctx);
std::shared_ptr<Context> get_current_context();

std::string get_path(const std::string &path);
}
```

```cpp
// module.cpp
std::weak_ptr<HelloWorldModule::Context> CTextEdit;

std::shared_ptr<HelloWorldModule::Context> HelloWorldModule::create_context() {
  auto ctx = std::make_shared<HelloWorldModule::Context>();
  set_current_context(ctx);
  return ctx;
}

void HelloWorldModule::destroy_context(std::shared_ptr<HelloWorldModule::Context> ctx) {
  set_current_context(nullptr);
}

void HelloWorldModule::set_current_context(std::shared_ptr<HelloWorldModule::Context> ctx) {
  CTextEdit = ctx;
}

std::shared_ptr<HelloWorldModule::Context> HelloWorldModule::get_current_context() {
  return CTextEdit.lock();
}

std::string HelloWorldModule::get_path(const std::string &path) {
  return get_current_context()->m_interface->cook_path(path);
}
```

A few things worth knowing. The global pointer is a `weak_ptr`: the real owner is the module (the `ctx` member in `main.cpp`), so when the module lets go of it, the context disappears and nobody keeps it alive by accident. `get_path` is a small helper around `cook_path`, it turns `resources/icons/edit.png` into the real path inside your module folder, and you will use it a lot. Finally, check that `get_current_context()` is not null if you call it from code that could run after `destroy`.

At the start of `execute`:

```cpp
void execute() override {
  ctx = HelloWorldModule::create_context();

  auto m = ModuleInterface::get_editor_module_by_name(this->name());
  HelloWorldModule::get_current_context()->m_interface = m;
}
```

## Logs

Logging is the first tool you will use to understand what your module does. The module name is used as the source automatically.

```cpp
log_info("Module started");
log_warning("Config file not found, using defaults");
log_error("Unable to open the file");
log_fatal("Data is corrupted");
```

Outside of your module class, in a plain function for example, go through the interface of the context:

```cpp
get_current_context()->m_interface->log_info("Hello");
```

Or use the global Vortex logger: `vxe::log_info("My module", "Hello")`.

## Add features to the content browser

This is where modules get fun. The content browser can learn three things from your module: an item handler (an action on an item, like "Edit" or "Open picture"), an item identifier (a way to recognize a special kind of item), and an item creator (a way to create a new item from the browser).

### Add an action on existing files

A handler is an action shown for a given type of item. When the user triggers it, you receive the path of the item.

```cpp
this->add_content_browser_item_handler(ItemHandlerInterface(
    "file_txt",
    HelloWorldModule::StartTextEditorInstance,
    "Edit",
    "Edit this txt file",
    HelloWorldModule::get_path("resources/icons/edit.png")));
```

The parameters are the type, the function to call (it receives the path), the title, a description and an icon. Here, `file_txt` is a built-in type: every text file gets an "Edit" action from your module.

### Create your own item type

Say your module works with folders that contain a file named `SampleConfig.txt`. You want Vortex to recognize these folders as a special thing, a "Super file", with its own color and its own actions.

You need two pieces, an identifier and a handler, linked by the same type name.

First, write the detection function. It receives a path and answers `true` or `false`.

```cpp
bool HelloWorldModule::IsValidFile(const std::string &path) {
  namespace fs = std::filesystem;

  if (!fs::is_directory(path))
    return false;

  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.is_regular_file() && entry.path().filename() == "SampleConfig.txt")
      return true;
  }

  return false;
}
```

It must be a plain function (or a lambda without captures), so it cannot use variables from your class. It is called often, so keep it fast.

Then register the identifier. Its name is your new item type. Use the form `module_name:item_type` to avoid clashes with other modules.

```cpp
this->add_content_browser_item_identifier(ItemIdentifierInterface(
    HelloWorldModule::IsValidFile,
    "infinitehq.helloworldmodule:superfile",
    "Super file",
    "#553333"));
```

The parameters are the detection function, the type name, a description and the line color (hex). You can also add a logo and a background image at the end.

Finally, register a handler for this type, with the exact same type name.

```cpp
this->add_content_browser_item_handler(ItemHandlerInterface(
    "infinitehq.helloworldmodule:superfile",
    HelloWorldModule::StartTextEditorInstance,
    "Super Edit",
    "Edit this super file",
    HelloWorldModule::get_path("resources/icons/edit.png")));
```

Every folder that passes `IsValidFile` now shows up as a "Super file" and offers "Super Edit".

### Let the user create your items

To add an entry to the "create" menu of the content browser, register an item creator:

```cpp
this->add_content_browser_item_creator(ItemCreatorInterface(
    [](const std::string &path) {
      auto folder = vxe::create_folder(path);
      std::ofstream(folder + "/SampleConfig.txt");
    },
    "Super file",
    "Create a new super file",
    "#553333",
    HelloWorldModule::get_path("resources/icons/edit.png")));
```

The function receives the folder where the user wants the item. Here we create a folder with `vxe::create_folder` and drop the config file inside, so the new item is immediately recognized by the identifier above.

## Add a button to the toolbar

Toolbar entries are for actions that are not tied to a file: opening a tool, a window, a panel.

```cpp
this->add_toolbar_handler(ToolbarHandlerInterface(
    []() { HelloWorldModule::StartTextEditorInstance(""); },
    "Hello window",
    "Open a hello window",
    "Hello/Test",
    HelloWorldModule::get_path("resources/icons/edit.png")));
```

The fourth parameter is the topic, the group where the entry is displayed. Give the topic its own icon, and your module its main logo:

```cpp
this->add_toolbar_topic_logo("Hello/Test", HelloWorldModule::get_path("resources/icons/edit.png"));
this->set_toolbar_main_logo_path(HelloWorldModule::get_path("resources/icons/edit.png"));
```

## Module functions

Module functions are named features of your module that can be executed by name, with optional arguments and a return value.

```cpp
add_function([](ArgumentValues &args, ReturnValues &ret) {
  ret.set("Hello " + args.get());
}, "greet");
```

```cpp
ArgumentValues args;
args.set("World");

ReturnValues ret;
execute_function("greet", args, ret);

log_info(ret.get());
```

`ArgumentValues` and `ReturnValues` store a string, built for JSON. Use `set_json` and `get_json` to move structured data around:

```cpp
args.set_json({ { "name", "dark" }, { "size", 12 } });
std::string name = args.get_json()["name"];
```

## Talk to the rest of Vortex with events

Events are how your module communicates with the core, with plugins and with other modules, without knowing them directly. An input event is a door you open: others can call it. An output event is a broadcast: Vortex (or someone else) shouts, and everyone listening reacts.

### Listen to something

Vortex broadcasts `save_all` when the user saves everything. Listen to it to save your own data:

```cpp
this->add_output_event(ModuleOutputEvent(HelloWorldModule::oe_save_all, "save_all"));
```

```cpp
void HelloWorldModule::oe_save_all() {
  for (auto &window : get_current_context()->m_text_editor_instances)
    window->save();
}
```

### Offer something

Declare an input event so others can ask your module to do something:

```cpp
add_input_event([this](ArgumentValues &args, ReturnValues &ret) {
  HelloWorldModule::StartTextEditorInstance(args.get());
  ret.set_json({ { "ok", true } });
}, "open_file");
```

### Call something

```cpp
ArgumentValues args;
args.set("/path/to/file.txt");

ReturnValues ret;
call_input_event("infinitehq.helloworldmodule", "open_file", args, ret);
```

To call the core, use `"vortex"` as the module name, for example `call_input_event("vortex", "spawn_content_browser", args, ret)`.

### Report problems

If something odd happens inside an event handler, record it with `trigger_happening`. It is stored on the event and written in the logs:

```cpp
event->trigger_happening("my_module", HappeningState::WARNING, "Theme not found");
```

## Use the Vortex API from your module

The `vxe` namespace is available from anywhere in your module. A few things you will probably need.

To know where you are:

```cpp
auto ctx = vxe::get_current_context();
vxe::log_info("My module", "Project: " + ctx->name + " in " + ctx->projectPath.string());
```

To store data, each module has its own data folder, created for you: `datapath()`. Use it for settings, caches and saves.

```cpp
vxe::populate_json({ { "last_file", "notes.txt" } }, datapath() + "/settings.json");
auto settings = vxe::dump_json(datapath() + "/settings.json");
```

To use the internet, remember that the user decides if Vortex can go online. `VortexNet` does not check this for you, so check it yourself:

```cpp
auto ctx = vxe::get_current_context();
if (ctx->IO.allow_net)
  std::string response = ctx->net.GET("https://example.com/api/data");
```

To add to the menus:

```cpp
vxe::push_edit_menu_item("My action", []() { vxe::log_info("My module", "Clicked"); },
                         "icons/action.png", "My section");
```

If you add menu items in `execute`, remove them in `destroy` with `pop_edit_menu_item`.

## Use external libraries

A module is a normal C++ project, so you can link whatever helps you: a JSON or markdown parser, an image decoder, a network client, an audio library, a scripting engine.

The usual path is to add the library to your build (a git submodule, a package, or a copy in `third_party/`), link it in the build file of your module, then include it where you need it and use it like in any C++ program.

A good habit is to keep the external library behind a small function of your own, so the rest of the module does not depend on it. If you decide to change it later, you only touch one place.

## Documentation and credits

Ship a bit of help with your module. Documentation pages are markdown files, grouped by section:

```cpp
add_documentation("Getting started", "Introduction", get_path("docs/intro.md"));
```

Credits come from a simple file. Create a `CREDITS` file in your module:

```
-----BEGIN CREDITS-----
# Authors
Jane Doe <jane@example.com>

# Contributors
Alex
-----END CREDITS-----
```

And register it:

```cpp
this->set_credits_file(HelloWorldModule::get_path("CREDITS"));
```

## Cleaning up in destroy

Everything comes together here. A good `destroy` removes what the module added and closes what it opened:

```cpp
void destroy() override {
  this->reset_module();

  for (auto i : HelloWorldModule::get_current_context()->m_text_editor_instances)
    CherryApp.DeleteAppWindow(i->get_app_window());

  HelloWorldModule::destroy_context(ctx);
  ctx.reset();
}
```

`reset_module()` clears the content browser handlers and identifiers of your module. The open windows are deleted (more about windows in the UI guide). The context is destroyed last, because the steps above may still need it.

## Troubleshooting

If your module does not start and shows up as failed, open the logs first: Vortex writes the reason there, and most of the time it is a missing dependency or a Vortex version that your module does not declare with `add_supported_version`. Remember that the version is compared on its first two numbers, so `"1.0"` is what you want, not `"1.0.0"`.

If your module is not found at all, check that `create_em` is exported with the exact name and, on Windows, with `__declspec(dllexport)`.

If a handler never appears in the content browser, the cause is almost always a type name that differs by one character between the identifier and the handler. Copy and paste the name, and prefix your own types with `module_name:` so they cannot collide with someone else's. If an item is never recognized, look at your detection function: it must be a plain function without captures, and it must return `true` for the exact structure you expect.

If Vortex crashes when you stop or reload the module, you most likely forgot something in `destroy`: a window still open, a context still used after it was destroyed, or a menu item whose function points to code that is gone. Undo in `destroy` everything you did in `execute`.

If a network call returns an empty string, the request may have failed, but also the user may simply have kept the internet disabled. Check `allow_net` and log a clear message when it is off.