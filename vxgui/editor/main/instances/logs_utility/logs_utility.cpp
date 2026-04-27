//
//  logs_utility.cpp
//  Sources of the logs utility main features,logics and utils.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "logs_utility.hpp"

namespace vxe {

  LogsUtility::LogsUtility(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    std::shared_ptr<Cherry::AppWindow> win = app_window_;

    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png"));
    app_window_->SetClosable(true);
    app_window_->SetCloseCallback([this]() { Cherry::DeleteAppWindow(app_window_); });
    app_window_->SetLeftMenubarCallback([this]() { render_left_menubar(); });
    app_window_->SetRightMenubarCallback([this]() { render_right_menubar(); });
    app_window_->SetLeftBottombarCallback([this]() { render_left_bottombar(); });

    Cherry::Log::AddCallback([this](Cherry::Log::Level level, const std::string &msg) { add_log_entry(level, msg); });

    LogUtilityChild filterbar("render_filters_bar", [this]() { render_filters_bar(); });
    filterbar.Disable();
    filterbar.m_DefaultSize = filterbar_width_;
    filterbar.m_BackgroundColor = Cherry::HexToRGBA("#35353535");
    add_child(filterbar);

    LogUtilityChild contentbar("render_content_bar", [this]() { render_content_bar(); });
    contentbar.Enable();
    contentbar.m_DefaultSize = 0.0;
    add_child(LogUtilityChild(contentbar));
  }

  void LogsUtility::add_child(const LogUtilityChild &child) {
    childs_.push_back(child);
  }

  std::shared_ptr<Cherry::AppWindow> &LogsUtility::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<LogsUtility> LogsUtility::create(const std::string &name) {
    auto instance = std::shared_ptr<LogsUtility>(new LogsUtility(name));
    instance->setup_render_callback();
    return instance;
  }

  void LogsUtility::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void LogsUtility::add_log_entry(Cherry::Log::Level level, const std::string &msg) {
    if (!entries_.empty() && entries_.back().message == msg && entries_.back().level == level) {
      entries_.back().count++;
    } else {
      entries_.push_back({ level, msg, 1 });
    }
    scroll_to_bottom_ = true;
  }

  void LogsUtility::send_command(const std::string &cmd) {
    std::string finalCmd = cmd;
    if (cmd.rfind("Vortex.", 0) != 0) {
      finalCmd = "Vortex." + cmd;
    }
    vxe::script::get_scripting_engine().execute(finalCmd);
  }

  void LogsUtility::activate_topic_filter(const std::string &topic) {
    topics_filter_states_[topic] = true;
  }

  bool LogsUtility::has_common_subsequence(const std::string &a, const std::string &b) {
    int j = 0;
    for (int i = 0; i < a.size() && j < b.size(); i++) {
      if (a[i] == b[j])
        j++;
    }
    return j == b.size();
  }

}  // namespace vxe