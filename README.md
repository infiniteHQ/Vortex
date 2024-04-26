

<a href="https://infinite.si">
  <h1 align="center">
    <picture>
      <source media="(prefers-color-scheme: dark)" srcset="./.github/imgs/main.png">
      <img height="250px" src="./.github/imgs/main.png">
    </picture>
  </h1>
</a>

<div align="center">
<a title="Code Quality" href="https://www.codefactor.io/repository/github/infinitehq/VortexMaker"><img alt="Code Quality" src="https://img.shields.io/codefactor/grade/github/infinitehq/VortexMaker?longCache=true&style=for-the-badge&label=Code%20Quality&logoColor=fff&logo=CodeFactor&branch=master"></a>
  <a title="Discord Server" href="https://discord.gg/H2wptkecUg"><img alt="Discord Server" src="https://img.shields.io/discord/1095333825762046194?label=Discord&logo=Discord&logoColor=fff&style=for-the-badge"></a>
<a title="'Build' workflow Status" href="https://img.shields.io/github/actions/workflow/status/infiniteHQ/VortexMaker/build.yml"><img alt="Build" src="https://img.shields.io/github/actions/workflow/status/infiniteHQ/VortexMaker/build.yml?longCache=true&style=for-the-badge&label=Build&logoColor=fff&logo=GitHub%20Actions&branch=main"></a>
  <a title="Modules & Plugins" href=""><img alt="Modules & Plugins" src="https://img.shields.io/badge/Modules-Supported-brightgreen?logo=stackedit&logoColor=%23FFFFFF&style=for-the-badge"></a>
</div>

## What is it ?
Vortex is a comprehensive open creation platform for creators and makers. The primary goal of this platform is to empower everyone to create systems, real-world apps, embedded applications, user environment apps, networking solutions, and more—while upholding the values of top-notch technology: security, safety, efficiency, and quality. This editor aims to provide users access to best practices and excellent tools.

## Allow everyone to make everythings

Transform your environment into a playground, a sandbox with Vortex. Master this environment to manipulate data, services, apps, etc. Turn the real world into a sandbox and shape it to your liking, regardless of the project.

## Features
<img height="60px" alt="Code Quality" src=".github/imgs/projects.png">

With Vortex, create and collaborate on new or existing projects. You can simply create your project with the command **vortex -cp \<ProjectName>**, go into the project folder and execute the command **vortex -g**. (Why not create a project manager at the future.) And then, you can add modules/plugins in your project to make it as you want. You can also create or add assets and start a powerfull adventure !

<img height="60px" alt="Code Quality" src=".github/imgs/modules.png">

This project was made for modularity and modifications, you can so add or remove all components you want, and adapt Vortex with your usecases and your problematics. Add modules to have dedicated featrures relative to what you want to create, you can have toolchains modules. You can also create your owns modules easely (check our template) and give it to the community for example ! :)

<img height="60px" alt="Code Quality" src=".github/imgs/components.png">

Vortex give you the possibility to create your own assets and components, you can so create all things you want (like toolchains, systems, apps, libs), and add it into your project.

### Builtin modules/plugins

With builtin modules, you can develop embedded applications (for microcontrollers, etc.) is also an area of interest for Vortex. With its set of tools, it will now be easier to develop our future applications. Vortex builtin modules also allows the creation of various tools such as toolchains, packages, build hosts, scripts, deployment systems, maintenance, and update systems. 

Creating complete systems is now easy with Vortex. Imagine, develop, build, maintain, and update them with many tools to optimize the process and reduce the cost of development and production.

- Packages module
- Toolchains module
- Tasks/Tasklists module

### Download & build project

- Linux

```bash
git clone https://github.com/infiniteHQ/VortexMaker
cd VortexMaker/build/in_system/
mkdir build && cd build
bash ./build.sh
```

### Get started

- Create a project

```bash
vortex -cp ProjectName
```

- Open a project with the graphical editor (execute it into the project folder)

---



## General Features
- Operating systems creation tools
- Embedded systems creation tools
- Embedded apps creation tools
- Independant components creation tools (toolchains, packages, scripts, etc...)
- Pre/production & Post/Production tools
- Maintenance tools
- Update tools

- #### Projects
    - Project gestion & saves handle
    - Export to production
    - Easy collaboration
- ####  Toolchains
    - Toolchain development (manage packages, scripts and tasks)
    - Toolchain compilation
    - Toolchain debugging (gdb, testers, etc...)
- #### Hosts & Build Environments
    - Host development (packages, skeletons, routines, etc...)
    - Host compilation (from a toolchain for exemple)
    - Host linking and final components build
    - Host debugging runtime
- #### General Purpose Operating Systems (GPOS)
    - System development & production


- Operating Systems, Embedded Filesystems & Embbeded app creation
- Full toolchain creation
- Complete package & tarball creation
- Advanced snapshot/recovery of a project
- Production tools



## Build & Installation

#### Linux/Unix Build
```
git clone https://github.com/infiniteHQ/VortexMaker.git
cd VortexMaker
mkdir build && cd build
cmake ..
sudo make install
```


#### Linux Distributions
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
