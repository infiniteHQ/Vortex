# Develop modules UI

This guide shows how to give your module a face: windows, instances, icons and fonts. It continues the [logic guide](#), with the same `HelloWorld` module.

The interface of Vortex is built with **Cherry**. A module does not draw on screen by itself: it creates a Cherry app window, gives it a function to render its content, and adds it to the application. That is the whole idea, and the rest of this page is just how to do it cleanly.

This guide covers how a module uses windows. For everything Cherry itself can do (components, layouts, styling, the full list of available features), the reference is the Cherry website: [cherry.infinite.si](https://cherry.infinite.si). Keep it open next to this page, you will want it as soon as your window has more than one line of text.

## Enable the Cherry Kit first

Before writing any window code, check one thing. The components used in this guide (`CherryKit::TextSimple` and all the others of the kit) are only compiled if the kit is enabled.

In the Cherry that comes with Vortex, open the `options.hpp` file at the base of Cherry and uncomment:

```cpp
#define CHERRY_ENABLE_KIT
```

If your module uses other Cherry options, enable them in the same file too. This is a build option of Cherry, not of your module, so it must be set before everything is compiled. If you skip it, you will get compilation errors on `CherryKit::`, and the error messages will not tell you that the cause is a disabled option. After changing `options.hpp`, rebuild Cherry and then your module.

## The big picture

A module UI is made of three parts.

A window class (`HelloWindow`) owns a `Cherry::AppWindow` and knows how to draw it. A function in your logic creates the window and registers it. And a place to keep it, the module context, so you can find it and close it later.

The flow is always the same: the user clicks a toolbar button or a content browser action, your logic function creates the window class, `Cherry::AddAppWindow` makes it appear, and from then on `render()` is called every frame.

## Your first window

A window class is small. Here is the header:

```cpp
// hello_window.hpp
#pragma once
#include <vxcore/include/vortex.h>
#include <vxcore/include/vortex_internals.h>

namespace ModuleUI {

class HelloWindow : public std::enable_shared_from_this<HelloWindow> {
public:
  HelloWindow(const std::string &name);

  void render();

  std::shared_ptr<Cherry::AppWindow> &get_app_window();
  static std::shared_ptr<HelloWindow> create(const std::string &name);
  void setup_render_callback();

private:
  std::shared_ptr<Cherry::AppWindow> app_window_;
  ComponentsPool component_pool_;
};

}
```

And the implementation:

```cpp
// hello_window.cpp
#include "hello_window.hpp"
#include "../../../src/module.hpp"

namespace ModuleUI {

HelloWindow::HelloWindow(const std::string &name) {
  app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
  app_window_->SetIcon(HelloWorldModule::get_path("resources/icons/edit.png"));

  app_window_->m_CloseCallback = [=]() {
    Cherry::DeleteAppWindow(app_window_);
    app_window_->SetVisibility(false);
  };
}

std::shared_ptr<Cherry::AppWindow> &HelloWindow::get_app_window() {
  return app_window_;
}

std::shared_ptr<HelloWindow> HelloWindow::create(const std::string &name) {
  auto instance = std::shared_ptr<HelloWindow>(new HelloWindow(name));
  instance->setup_render_callback();
  return instance;
}

void HelloWindow::setup_render_callback() {
  auto self = shared_from_this();
  app_window_->SetRenderCallback([self]() {
    if (self)
      self->render();
  });
}

void HelloWindow::render() {
  CherryApp.PushComponentPool(&component_pool_);

  CherryKit::TextSimple("Hello module");

  CherryApp.PopComponentPool();
}

}
```

## Why create() and shared_from_this

The render callback needs to call `render()` on your window object. For that it must hold a pointer to the window, and that pointer must be a `shared_ptr`, otherwise the window could be destroyed while Cherry still calls it.

`shared_from_this()` gives you that pointer, but it only works after the object is already owned by a `shared_ptr`. That is why the constructor cannot set the callback itself, and why we use a static `create()`: it builds the object and wraps it in a `shared_ptr`, then calls `setup_render_callback()`, which can safely use `shared_from_this()`.

Follow this pattern for every window of your module and you will avoid the classic crash of a callback called on a dead object.

## Drawing the content

Everything you draw goes into `render()`. It runs every frame, so keep it light: no file reading, no heavy computation, no network call inside it. Do the work elsewhere and keep the result in a member.

```cpp
void HelloWindow::render() {
  CherryApp.PushComponentPool(&component_pool_);

  CherryKit::TextSimple("Hello module");

  CherryApp.PopComponentPool();
}
```

The window owns a `ComponentsPool`. Cherry components keep some state (hover, focus, animation), and the pool is where this state lives for this window. That is why the drawing is wrapped between `PushComponentPool` and `PopComponentPool`. Without it, two windows of your module could mix their component states. Always pop what you pushed, even if you want to leave `render()` early.

Display state from your window class and not from globals. Each instance then has its own data, which is exactly what you want when you open several windows.

To go further than a line of text, the website [cherry.infinite.si](https://cherry.infinite.si) shows the components of the kit and how to use them. A window is just a place to call them from `render()`.

## Open a window from your logic

The function that creates the window lives in the logic part, so it can be used by a content browser handler and a toolbar button alike:

```cpp
void HelloWorldModule::StartTextEditorInstance(const std::string &path) {
  std::string window_name =
      std::to_string(get_current_context()->m_text_editor_instances.size());

  auto inst = ModuleUI::HelloWindow::create(window_name);
  Cherry::AddAppWindow(inst->get_app_window());
  get_current_context()->m_text_editor_instances.push_back(inst);
}
```

Three steps, always the same. Create the window instance, add its `AppWindow` to Cherry with `Cherry::AddAppWindow`, and keep the instance in the context. The third step is easy to forget, and it matters: without it, nobody can find the window to close it when the module stops.

## Several instances of the same window

A big strength of this design: calling `StartTextEditorInstance` ten times gives you ten independent windows, because every instance has its own `AppWindow`, its own `ComponentsPool` and its own members.

Give each window a unique name, since the name is used to identify the window. Counting the instances works at first, but after closing one it can produce a name that already exists. A counter that only goes up is safer:

```cpp
struct Context {
  std::shared_ptr<ModuleInterface> m_interface;
  std::vector<std::shared_ptr<ModuleUI::HelloWindow>> m_text_editor_instances;
  int m_next_id = 0;
};
```

```cpp
std::string window_name = "Editor " + std::to_string(ctx->m_next_id++);
```

Also think about what an instance is for. For a text editor, it is one instance per file. You may want to avoid opening the same file twice: look through `m_text_editor_instances` first and reuse the existing window.

## Give the window some data

Handlers receive the path of the item. The example ignores it, but this is how you pass it to your window:

```cpp
class HelloWindow : public std::enable_shared_from_this<HelloWindow> {
public:
  HelloWindow(const std::string &name, const std::string &path);
  static std::shared_ptr<HelloWindow> create(const std::string &name, const std::string &path);

private:
  std::string path_;
  std::string content_;
};
```

```cpp
HelloWindow::HelloWindow(const std::string &name, const std::string &path)
    : path_(path) {
  app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
}
```

Load the content once (in the constructor or in `create`), keep it in `content_`, and only display it in `render()`.

Then, in the logic:

```cpp
void HelloWorldModule::StartTextEditorInstance(const std::string &path) {
  auto ctx = get_current_context();
  auto inst = ModuleUI::HelloWindow::create("Editor " + std::to_string(ctx->m_next_id++), path);
  Cherry::AddAppWindow(inst->get_app_window());
  ctx->m_text_editor_instances.push_back(inst);
}
```

The toolbar button can call the same function with an empty path to open a blank window. Your window just needs to handle `path_.empty()`.

## Window icon

```cpp
app_window_->SetIcon(HelloWorldModule::get_path("resources/icons/edit.png"));
```

Use `get_path` for every resource. It builds the path from the module folder, so your icons keep working wherever the user installs the module. For the toolbar, the logos are set in the logic part, in the toolbar section of the logic guide.

## Closing a window properly

When the user closes a window, Cherry calls `m_CloseCallback`:

```cpp
app_window_->m_CloseCallback = [=]() {
  Cherry::DeleteAppWindow(app_window_);
  app_window_->SetVisibility(false);
};
```

This removes the window from Cherry. But your context still holds the instance in its vector. If you want to really release it, remove it from the context too:

```cpp
auto self = shared_from_this();
app_window_->m_CloseCallback = [self]() {
  Cherry::DeleteAppWindow(self->app_window_);
  self->app_window_->SetVisibility(false);

  auto ctx = HelloWorldModule::get_current_context();
  if (ctx) {
    auto &list = ctx->m_text_editor_instances;
    list.erase(std::remove(list.begin(), list.end(), self), list.end());
  }
};
```

Set this callback from `setup_render_callback`, not from the constructor, for the same `shared_from_this` reason as before.

When the whole module stops, close everything that is still open in `destroy()`:

```cpp
for (auto i : HelloWorldModule::get_current_context()->m_text_editor_instances)
  CherryApp.DeleteAppWindow(i->get_app_window());
```

If you forget this, windows can stay on screen with a render callback pointing to code that no longer exists.

## Fonts

Fonts are loaded in `init_ui()`, once, when the UI starts:

```cpp
void init_ui() override {
  CherryApp.AddFont(
      "JetBrainsMono",
      HelloWorldModule::get_path("resources/fonts/JetBrainsMono-Regular.ttf"),
      40.0f);
}
```

Ship the font file in `resources/fonts/` and load it by its module path. A code editor, a log viewer or a terminal-like window are good reasons to bring your own font.

## A main window for your module

Some modules are not about many small windows, but about one big workspace (a map editor, an asset viewer). For that, a module can provide a main window:

```cpp
set_main_window(my_window->get_app_window());
```

If the application is already running, the window is refreshed immediately. If you rebuild your window later, call `refresh_main_window()`.

## Make your window talk to the module

A window should not know everything about the module. There are two simple ways to connect them.

Through the context: the window can call `HelloWorldModule::get_current_context()` to reach the interface, the other windows and your settings.

Through events: if something must happen when the user saves, listen to `save_all` in your logic and forward it to your windows. They do not need to know who triggered it.

```cpp
void HelloWorldModule::oe_save_all() {
  for (auto &window : get_current_context()->m_text_editor_instances)
    window->save();
}
```

## Using external libraries in the UI

Because a module is a normal C++ project, you can bring in any library for your windows too: a syntax highlighter, a charting library, an image loader, a markdown renderer.

A simple way to keep things tidy is to hold the library objects as members of your window class, so each instance has its own. Create them when the window is created, use them in `render()`, and let them be destroyed with the window. Keep the code that talks directly to the library in a few small functions and call them from `render()`, so that if the library changes, only these functions change.

## Troubleshooting

If you get compilation errors on `CherryKit::` or on other Cherry components, the kit is almost certainly not enabled. Uncomment `#define CHERRY_ENABLE_KIT` (and any other option you use) in the `options.hpp` at the base of Cherry, then rebuild Cherry and your module.

If your window never appears, check that you called `Cherry::AddAppWindow` with the `AppWindow` of your instance, and that the function creating it is really called: add a `log_info` in it to be sure.

If the module crashes as soon as the window renders, look at how it was built: the render callback must be set after the instance is owned by a `shared_ptr`, which means going through `create()` and never building the window with a plain `new` or on the stack.

If two windows behave strangely together (a hover that stays on the wrong window, a field that keeps the focus of another), one of them is probably missing its `PushComponentPool` or `PopComponentPool`. Each window needs its own pool, and every push needs its pop.

If your window stays on screen after the module is stopped, or crashes Vortex when it stops, make sure `destroy()` deletes every window still stored in the context before the context itself is destroyed.

If the interface freezes or becomes slow, something heavy is running in `render()`. Move file reading, parsing and network calls out of it and keep only the result.