//
//  project_settings_helpers.hpp
//  Structures for project settings utility
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "../utils.hpp"

#ifndef PROJECT_SETTINGS_HELPERS_HPP
#define PROJECT_SETTINGS_HELPERS_HPP

namespace vxe {

  struct ProjectSettingsSave {
    char name[128] = "unknow";
    char author[128] = "unknow";
    char version[128] = "unknow";
    char description[128] = "unknow";
    char type[128] = "unknow";
    bool include_system_templates;
  };

  struct ProjectSettingsChild {
    std::function<void()> RenderCallback;
    std::string LogoPath;
    std::string Name;
    ProjectSettingsChild(
        const std::string &name,
        const std::function<void()> &rendercallback = []() { },
        const std::string &logopath = "undefined")
        : Name(name),
          RenderCallback(rendercallback),
          LogoPath(logopath) { };
  };

  struct GroupNode {
    std::unordered_map<std::string, GroupNode> children;
    std::vector<Cherry::Component> components;
  };

  struct ProjectInfo {
    std::string name;
    std::string description;
    std::string version;
    std::string thumbnail_path;
    std::string banner_path;
    std::string type;
    std::string tags;
    std::string author;
    std::string website;
    std::string support_contact;
    std::string copyright_file;
    std::string license_file;
    std::string startup_script;
    std::string readme_file;
    std::string requirements_file;
    std::string code_of_conduct_file;
    std::string security_file;
    std::string root_content_path;
  };

}  // namespace vxe

#endif  // PROJECT_SETTINGS_HELPERS_HPP