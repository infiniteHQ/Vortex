

<a href="https://infinite.si">
  <h1 align="center">
    <picture>
      <source media="(prefers-color-scheme: dark)" srcset="./.github/imgs/main.png">
      <img src="./.github/imgs/main.png">
    </picture>
  </h1>
</a>

<div align="center">
  <a title="Discord Server" href="https://discord.gg/H2wptkecUg"><img alt="Discord Server" src="https://img.shields.io/discord/1095333825762046194?label=Discord&logo=Discord&logoColor=fff&style=for-the-badge"></a>
  <a title="'Build' workflow Status" href="https://img.shields.io/github/actions/workflow/status/infiniteHQ/Vortex/build.yml"><img alt="Build" src="https://img.shields.io/github/actions/workflow/status/infiniteHQ/Vortex/build.yml?longCache=true&style=for-the-badge&label=Build&logoColor=fff&logo=GitHub%20Actions&branch=main"></a>
  <a title="Modules & Plugins" href="#"><img alt="Modules & Plugins" src="https://img.shields.io/badge/Modules-Supported-brightgreen?logo=stackedit&logoColor=%23FFFFFF&style=for-the-badge"></a>
  <a title="Latest Release" href="https://github.com/infiniteHQ/Vortex/releases/latest"><img alt="Latest Release" src="https://img.shields.io/github/v/release/infiniteHQ/Vortex?style=for-the-badge&label=Release&logo=github"></a>
  <a title="Changelog" href="https://github.com/infiniteHQ/Vortex/blob/main/CHANGELOG.md"><img alt="Changelog" src="https://img.shields.io/badge/Changelog-View-blue?style=for-the-badge&logo=readme&logoColor=white"></a>
  <a title="Website" href="https://vortex.infinite.si/"><img alt="Website" src="https://img.shields.io/badge/Website-Visit-blueviolet?style=for-the-badge&logo=firefox-browser&logoColor=white"></a>
  <a title="Sponsor" href="https://fund.infinite.si/"><img alt="Sponsor" src="https://img.shields.io/badge/Sponsor-Infinite%20Fund-FF5999?style=for-the-badge&logo=githubsponsors&logoColor=white"></a>
</div>


> *Important: Vortex is part of [Infinite](https://infinite.si/)'s initiative to create free, independent, and ethical technology and computing. Vortex is a library powering many parts of this project, but maintaining projects like Vortex is a time-consuming and costly process. If you appreciate Vortex, you can support us [on our funding page](https://fund.infinite.si/) if you can.*

## What is *Vortex* ?
Vortex is a playground built by the Infinite community. It includes an editor, a launcher, and a content platform. It’s all about giving hackers the tools to hack, creators the freedom to create, and makers the space to build wild projects, mods, and services at the edge of cybersecurity, systems, apps, and the future of open technology!

### Allow everyone to make everythings
Transform your environment into a playground, a sandbox with Vortex. Master this environment to manipulate data, services, apps, etc. Turn the real world into a sandbox and shape it to your liking, regardless of the project.

## Repository guide

- `vxcore/` *: core of all Vortex editor features and backends*
- `vxgui/` *: all frontends, interfaces, and UI components*
  - `vxgui/editor/` *: editor graphical user interface*
  - `vxgui/crash_handler/` *: crash handler interface*
- `tests/`
  - `tests/project/` *: sample project for user feature testing*
- `docs/` *: documentation*
- `lib/` *: dependencies*
- `build/` *: build folder*

## About this repository  
> This repository represents the Vortex Editor. The Vortex Editor can open Vortex projects or tools, start modules and plugins. It helps hackers or creators manage content and create or hack many things. The goal of the editor is to create a convenient, interoperable, and simple stack to help every part of a project work together with the others.

### About the versioning
Vortex Editor uses a compatibility based versioning system rather than semantic versioning. Versions follow the format `MAJOR.MINOR+BUILD`, where the `MAJOR.MINOR` defines the compatibility branch (e.g. `1.0`, `1.1`, `2.0`) and `+BUILD` identifies each build within that branch (e.g. `1.0+build2`, `1.0+build2`). All projects created in the same branch remain compatible across its builds, while multiple branches (like `1.0` and `1.1`) can be installed and supported in parallel. This ensures long-term project/contents stability while allowing asynchronous development.


### How to install the Editor?  
> To properly install a version of the Editor on your system, it is recommended to use the [Vortex Launcher](https://github.com/infiniteHQ/VortexLauncher). The launcher helps you manage different versions of Vortex and allows you to work with many types of projects. It also helps you share content with the community and get templates, content, modules, and plugins from others. It definitely increases your productivity in this hacker space!

> [Download the Launcher here](https://vortex.infinite.si/)


### Compatibility
![✅ Ready](https://img.shields.io/badge/Microsoft%20Windows-Fully%20Compatible-green?longCache=true&style=for-the-badge)
![✅ Ready](https://img.shields.io/badge/Linux-Fully%20Compatible-green?longCache=true&style=for-the-badge)
![✅ Ready](https://img.shields.io/badge/MacOS-WIP-orange?longCache=true&style=for-the-badge)

### License
The Vortex Editor is licensed under the **Apache License 2.0** see [LICENSE](https://github.com/infiniteHQ/VortexLauncher/blob/main/LICENSE) for more information.

### Thanks
- For all authors of Lua programming language
- For all contributors of SDL2
- For all contributors of Vulkan/VulkanSDK
- For all contributors of STB
- For the log library of gabime and contributors (https://github.com/gabime/spdlog)
- For the json library of nlohmann and contributors (https://github.com/nlohmann/json)
- For the amazing immediate mode interface ImGui from Ocornut and contributors (https://github.com/ocornut/imgui)
- For the work of from "Studio Cherno" on Walnut (https://github.com/StudioCherno/Walnut)