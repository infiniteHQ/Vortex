//
//  logs_utility.hpp
//  Header and declarations for the logs utility, including rendering
//  and front end logics
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "logs_utility_helpers.hpp"

#ifndef VORTEX_EDITOR_LOGS_UTILITY_HPP
#define VORTEX_EDITOR_LOGS_UTILITY_HPP

namespace vxe {

  class LogsUtility : public std::enable_shared_from_this<LogsUtility> {
   public:
    LogsUtility(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<LogsUtility> create(const std::string &name);
    void setup_render_callback();

    // rendering
    void add_child(const LogUtilityChild &child);
    void render();
    void render_content_bar();
    void render_filters_bar();
    void render_left_bottombar();
    void render_left_menubar();
    void render_right_menubar();

    // front-end logic
    void activate_topic_filter(const std::string &topic);
    void add_log_entry(Cherry::Log::Level level, const std::string &msg);
    void send_command(const std::string &cmd);

    // utils
    bool has_common_subsequence(const std::string &a, const std::string &b);

   private:
    // logics
    int history_pos_ = -1;
    int selection_idx_ = -1;
    char input_buffer_[256] = "";
    std::string project_search_;
    std::string log_search_value_;
    std::vector<std::string> command_history_;
    std::vector<ConsoleEntry> entries_;
    std::map<std::string, bool> topics_filter_states_;

    // ui properties
    bool use_warning_filter_ = false;
    bool use_error_filter_ = false;
    bool use_fatal_filter_ = false;
    bool use_info_filter_ = false;
    bool console_font_ = true;
    bool search_bar_ = false;
    bool scroll_to_bottom_ = false;
    bool show_filter_pannel_ = false;
    float filterbar_width_ = 250.0f;
    ShowMode show_mode_ = ShowMode::Simple;

    // ui states
    bool will_show_filter_pannel_ = false;
    bool childs_initialized_ = false;
    bool previous_filter_pannel_state_ = false;

    // ui
    std::shared_ptr<Cherry::AppWindow> app_window_;
    std::vector<LogUtilityChild> childs_;
  };
}  // namespace vxe

#endif  // LOGUTILITY_H
