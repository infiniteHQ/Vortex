Vortex Maker
=====
Vortex makes it easy to create a system, application or toolchain. It also offers a wide range of options for system maintenance and supervision. 

![VortexMaker Editor](./.github/imgs/editor.png)

## Build & Installation
```
git clone https://github.com/infiniteHQ/VortexMaker.git
cd VortexMaker
mkdir build && cd build
cmake ..
sudo make install
```

## Usage
Usage : vortex <paramater(s)...> <information(s)...>

- -h --help :               See all parameters
- -g --gui :                Open the Vortex graphical interface
- -I --init  <...> :        Initialize a new project
- -i --install  <...> :     Install a new content
- -b --build <...> :        Build the project
- -a --add <...> :          Add a content (packages, libs, toolchains...)
- -p --preview <...> :      Preview final component(s) of the project


## Create your first operating system
##### Create your first project
For this example, let's initialize a project with a sample, such as a basic Linux operating system.

*vortex -cp \<project_name\> \<template\>*
```
vortex -cp MySystem linux_os
```
This command will create a system called *MySystem* based on *linux_os* template.


##### Develop your system...
First, enter your project with 

```
cd ./MySystem
```


Once inside, you can interact with it using various commands:

*Open the project editor*
```
vortex -g
```
or
```
vortex --gui
```

*Add a package into the project*
```
vortex -a package <path_to_your_package>
```

*Build all toolchains*
```
vortex -b toolchains
```

*Build all project*
```
vortex -b all
```
