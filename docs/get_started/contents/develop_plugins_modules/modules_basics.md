A module is a big piece of program that will run alongside Vortex, not at the top of it like a simple plugin. The module is capable of doing big tasks.

In the philosophy of Vortex, all modules give main features of the user editor, plugins can help and modify improve the experience with these modules.

At the top a module has an API of events callable from the vortex core and other plugins or modules. It's extremely modular but really powerful and easy to use.

### Module or plugin ?
You want to create a module if:
- You want to create big features.
- The Vortex Core and other modules don't give you the features you want.
- You want to develop alongside Vortex, and not only on top-level features.
- You are ready to do cross-platform or handle all platform compatibility

You don't want to create a module and instead create a plugin if:
- You begin with Vortex
- You want an easy distribution process.
- You want really simple development.

### In this tutorial

In this section, you will learn how to create your own module, with a user interface or not, and distribute it for everyone! 

### What do I need?

To develop modules for Vortex, you need to know programming languages like C++. You also need to understand concepts of dynamic shared libraries and cross-platform programming.

To program your module you need a code editor / IDE. C/C++ compiler alongside CMake (for C++ modules).

In order to make graphical user interfaces (GUIs), you need to know a little about Cherry (our own custom user interface framework), and particularly the Cherry Lua API that you can directly use to make interfaces for Vortex Plugins.

### Quick start

To start developing modules you can learn from scratch with all the following pages, or go to our ready-to-start example here.