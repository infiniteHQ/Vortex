### Setup the module development

Let's start! A module is bigger than a plugin. In order to test and develop properly, we recommand base your module [on this base "HelloWorldModule"](https://github.com/infiniteHQ/HelloWorldModule).

We recommend using git, to track progress and make versions of your module.

This sample (HelloWorldModule) embed vortex in (HelloWorldModule)/lib/ folder, it's required for compilation and allows you live testing. If you want to start from this base we recommand you to read the "Start from the official template"

If you don't want to base your module on (HelloWorldModule) example, we will consider at this point that you have all the things registered in the section Standard format of a module.

### Standard format of a module

REQUIRED
- lib/ folder with vortex ([BASE]/lib/vortex)
- modules.json at the root
- main/ folder with main.cpp and src inside
- scripts/ folder with 2 scripts to build vx, 2 scripts to test the project and 2 scripts to build module (2 fo macOS/Linux AND Microsoft windows)
- CMakeLists.txt at the root, containing compilation informations

OPTIONNAL BUT RECOMMANDED
- docs/ folder with the complete documentation of your module
- CREDITS file with the standard Vortex Credits Format
- LICENSE with the software licence of your module
- README.md with basic instructions for contibutors and developers

### Start from the official template