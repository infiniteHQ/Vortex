A module is a big piece of programm that will run alonside Vortex, not at the top of it like a simple plugin. The module is capable of doing big tasks.

In the philosofy of Vortex, all modules gives main features of the user editor, plugins can help and modify improve the experience with theses modules.

At the top a module have a api of events callable from the vortex core and other plugins or modules. It's extremelly modular but really powerfull and easy to use.

### Module or plugin ?
You want a to create a module if :
- You want to create big features.
- The Vortex Core and other modules dont gives you the features you want.
- You want to develop along side of Vortex, and not only on top-level features.
- You are ready to do cross platform or handle all platform compatibility

You dont want to create a module and instead create a plugin if :
- You begins with Vortex
- You want a easy distribution process.
- You want developpement really simple.

### In this tutorial

In this section, you will learn how to create your own module, with a user interface or not, and distribute it for everyone! 

### What do I need?

To develop modules for Vortex, you need to know programming languages like C++. You also need to understand concepts of dynamic shared libraries and cross platform programming.

To program your module you need a code editor / IDE. C/C++ compiler alongside CMake (for C++ modules).

In order to make graphical user interfaces (GUIs), you need to know a little about Cherry (our own custom user interface framework), and particularly the Cherry Lua API that you can directly use to make interfaces for Vortex Plugins.

### Quick start

To start developing modules you can learn from scratch with all following pages, or go on our ready-to-start example here.