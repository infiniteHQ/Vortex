//
//  project_settings.hpp
//  Headers and declarations of logics and rendering of the project
//  settings utility.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "project_settings_helpers.hpp"

#ifndef PROJECT_SETTINGS_HPP
#define PROJECT_SETTINGS_HPP

namespace vxe {

  class ProjectSettings : public std::enable_shared_from_this<ProjectSettings> {
   public:
    ProjectSettings(const std::string &name);

    // window
    static std::shared_ptr<ProjectSettings> create(const std::string &name);
    void setup_render_callback();
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    ProjectSettingsChild *get_child(const std::string &child_name);
    void add_child(const ProjectSettingsChild &child);
    void remove_child(const std::string &child_name);
    void add_informations_child();
    void add_configurations_child();
    void add_appearance_child();
    void add_interface_child();
    void add_contents_templates_child();
    void add_modules_child();
    void add_plugins_child();
    void add_startup_child();
    void add_accessibility_child();
    void add_help_child();
    void add_all_childs();

    // rendering
    void render();
    void insert_into_tree(
        GroupNode &node,
        const std::vector<std::string> &pathParts,
        size_t index,
        const std::vector<Cherry::Component> &components);
    Cherry::Component build_component_tree(const std::string &name, const GroupNode &node);

    // logic
    void refresh();
    void update();
    void refresh_project_informations();
    void update_project_informations();
    void refresh_project_themes();
    void load_tab_user_want(const std::string &tab);
    void project_info_reset();

   private:
    // main project settings
    std::shared_ptr<ProjectSettingsSave> current_save_;
    ProjectInfo initial_project_info_;
    ProjectInfo current_project_info_;

    // theme settings
    std::string new_theme_name_;
    std::vector<std::string> available_themes_;

    // ui
    std::vector<ProjectSettingsChild> childs_;
    std::shared_ptr<Cherry::AppWindow> app_window_;
    std::string selected_child_name_;
    float left_panel_width_ = 290.0f;
  };
}  // namespace vxe

#endif  // PROJECT_SETTINGS_HPP