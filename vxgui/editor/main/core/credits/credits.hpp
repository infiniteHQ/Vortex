//
//  credits.hpp
//  Header and declarations for the "credits" window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef CREDITS_WINDOW_HPP
#define CREDITS_WINDOW_HPP

namespace vxe {

  class Credits : public std::enable_shared_from_this<Credits> {
   public:
    Credits(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<Credits> create(const std::string &name);
    void setup_render_Callback();

    // rendering
    void render();
    void render_left_menubar();

    // logic
    void set_selected_category(const std::string &c);
    void refresh_categories();
    std::string get_selected_category();

   private:
    // logic
    std::string selected_category_;
    std::unordered_map<std::string, int> all_categories_;

    // ui states
    bool selected_category_changed_ = false;
    bool user_scrolled_ = false;
    float scroll_y_ = 0.0f;

    // ui properties
    bool auto_scroll_ = true;
    float user_scroll_cooldown_ = 0.0f;
    float user_scroll_pause_ = 3.0f;
    float scroll_speed_ = 50.0f;

    // window
    std::shared_ptr<Cherry::AppWindow> app_window_;
  };

}  // namespace vxe

#endif  // CREDITS_WINDOW_HPP