//
//  plugins_utility.hpp
//  Header and declarations for the plugins utility front-end
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include <unordered_set>

#include "../../../../../vxcore/include/plugins/delete.hpp"
#include "../../../../../vxcore/include/plugins/load.hpp"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "../../instances/modules_details/modules_details.hpp"
#include "./ui/plugin_card_item.hpp"
#include "plugins_utility_helpers.hpp"

#ifndef PLUGINS_UTILITY_WINDOW_H
#define PLUGINS_UTILITY_WINDOW_H

namespace vxe {

  class PluginsUtility : public std::enable_shared_from_this<PluginsUtility> {
   public:
    PluginsUtility(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<PluginsUtility> create(const std::string &name);
    void setup_render_callback();

    // rendering
    void render();
    void render_installed();
    void render_import();
    void render_download();
    void render_plugin_deletion_modal();
    void render_left_menubar();
    void render_right_menubar();

    // logic
    void refresh_categories();

    // utils
    bool has_common_subsequence(const std::string &a, const std::string &b);
    void set_plugin_to_delete(const std::shared_ptr<PluginInterface> &mod);
    void set_selected_category(const std::string &c);
    std::string get_selected_category();

   private:
    std::vector<PluginsUtilityChild> childs_;

    std::string plugins_search_;
    bool trigger_plugin_deletion_modal_ = false;
    std::string plugin_to_delete_name_;
    std::string plugin_to_delete_proper_name_;
    std::string plugin_to_delete_description_;
    std::string plugin_to_delete_version_;
    std::string plugin_to_delete_logo_path_;

    PluginsUtilityShowModes selected_show_mode_ = PluginsUtilityShowModes::Thumbmails;
    PluginsUtilityPannels selected_pannel_ = PluginsUtilityPannels::Installed;

    std::string selected_category_;
    bool selected_category_changed_ = false;
    std::unordered_map<std::string, int> all_categories;

    float left_panel_width = 290.0f;

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe
#endif  // MODULEs_UTILITY_WINDOW_H