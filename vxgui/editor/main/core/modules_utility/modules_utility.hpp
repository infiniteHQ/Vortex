//
//  modules_utility.hpp
//  Header and declarations for the modules utility front-end
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include <unordered_set>

#include "../../../../../vxcore/include/modules/delete.hpp"
#include "../../../../../vxcore/include/modules/load.hpp"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "../../instances/modules_details/modules_details.hpp"
#include "./ui/module_card_item.hpp"
#include "modules_utility_helpers.hpp"

#ifndef MODULES_UTILITY_WINDOW_H
#define MODULES_UTILITY_WINDOW_H

namespace vxe {

  class ModulesUtility : public std::enable_shared_from_this<ModulesUtility> {
   public:
    ModulesUtility(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<ModulesUtility> create(const std::string &name);
    void setup_render_callback();

    // rendering
    void render();
    void render_installed();
    void render_import();
    void render_download();
    void render_module_deletion_modal();
    void render_left_menubar();
    void render_right_menubar();

    // logic
    void refresh_categories();

    // utils
    bool has_common_subsequence(const std::string &a, const std::string &b);
    void set_module_to_delete(const std::shared_ptr<ModuleInterface> &mod);
    void set_selected_category(const std::string &c);
    std::string get_selected_category();

   private:
    std::vector<ModulesUtilityChild> childs_;

    std::string modules_search_;
    bool trigger_module_deletion_modal_ = false;
    std::string module_to_delete_name_;
    std::string module_to_delete_proper_name_;
    std::string module_to_delete_description_;
    std::string module_to_delete_version_;
    std::string module_to_delete_logo_path_;

    ShowModes selected_show_mode_ = ShowModes::Thumbmails;
    Pannels selected_pannel_ = Pannels::Installed;

    std::string selected_category_;
    bool selected_category_changed_ = false;
    std::unordered_map<std::string, int> all_categories;

    float left_panel_width = 290.0f;

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe
#endif  // MODULEs_UTILITY_WINDOW_H