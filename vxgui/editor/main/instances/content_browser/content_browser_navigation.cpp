//
//  content_browser_navigation.cpp
//  Navigation/creation features of the content browser.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "content_browser.hpp"

namespace vxe {

  void ContentBrowser::go_back() {
    if (!back_history_.empty()) {
      forward_history_.push(current_directory_.string());
      current_directory_ = back_history_.top();
      back_history_.pop();
    }
  }

  void ContentBrowser::go_forward() {
    if (!forward_history_.empty()) {
      back_history_.push(current_directory_.string());
      current_directory_ = forward_history_.top();
      forward_history_.pop();
    }
  }

  void ContentBrowser::change_directory(const std::filesystem::path &new_directory) {
    if (new_directory != current_directory_.string()) {
      if (!current_directory_.empty()) {
        back_history_.push(current_directory_.string());

        while (!forward_history_.empty()) {
          forward_history_.pop();
        }
      }
      current_directory_ = new_directory;
    }
  }

  void ContentBrowser::path_changed() {
    // Fetch Modules item hamdlers
  }

  void ContentBrowser::select(const std::string &path) {
    for (auto already : selected_) {
      if (path == already) {
        return;
      }
    }
    selected_.push_back(path);
  }

  void ContentBrowser::create_file() {
    auto path = vxe::create_file(current_directory_.string());
    path_to_rename_ = path;
  }

  void ContentBrowser::create_folder() {
    auto path = vxe::create_folder(current_directory_.string());
    path_to_rename_ = path;
  }
}  // namespace vxe