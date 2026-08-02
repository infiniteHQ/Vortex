//
//  add_window.hpp
//  Headers and declarations for the "add window" of the content browser
//
//  Copyright (c) 2026 Infinite
//
//  This work is licensed under the terms of the Apache-2.0 license.
//  For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <atomic>
#include <mutex>

#include "../../../../../../../vxcore/include/vortex.h"
#include "../../../../../../../vxcore/include/vortex_internals.h"

#ifndef CONTENT_BROWSER_ADD_WINDOW_HPP
#define CONTENT_BROWSER_ADD_WINDOW_HPP

namespace vxe {

  struct ContentRelease {
    std::string uuid;
    std::string name;
    std::string platform;
    std::string arch;
    std::string major;
    std::string version;
  };

  struct ContentInfo {
    std::string uuid;
    std::string name;
    std::string proper_name;
    std::string description;
    std::string picture_link;
    std::string banner_link;
    std::string host;
    std::string host_link;
    bool certified = false;
    std::string state;
  };

  struct ContentFetchResult {
    bool success = false;
    std::string error;
    ContentInfo info;
    std::vector<ContentRelease> compatible_releases;
    std::vector<ContentRelease> compatible_cross_releases;
  };

  ContentFetchResult fetch_content_from_flashlink(const std::string &flashlink);

  class AddWindow : public std::enable_shared_from_this<AddWindow> {
   public:
    AddWindow(const std::string &name, const std::string &path);

    // window and rendering
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<AddWindow> create(const std::string &name, const std::string &path);
    void setup_render_callback();
    void render();
    void render_default();
    void render_content_flash_link();

    // utils
    void set_create_folder_callback(const std::function<void()> &callback);
    void set_create_file_callback(const std::function<void()> &callback);
    void set_create_item_callback(
        const std::function<void(const std::function<void(const std::string &)> &creator)> &callback);
    void set_import_content_callback(const std::function<void()> &callback);

   private:
    // callbacks
    std::function<void()> create_file_callback_;
    std::function<void()> create_folder_callback_;
    std::function<void()> import_content_callback_;
    std::function<void(const std::function<void(const std::string &)> &creator)> create_item_callback_;

    std::string creation_path_;
    std::string add_window_search_;

    // content flash link ui state
    bool show_content_flash_link_ = false;

    std::shared_ptr<vxe::ModuleInstallProgress> install_progress_;
    bool detected = false;
    std::string decoded_text;

    std::string manual_search_error_;

    enum class PlatformTargetMode { Cross, Native };
    PlatformTargetMode selected_platform_mode_ = PlatformTargetMode::Native;
    bool platform_mode_initialized_ = false;
    enum class FlashLinkState { WaitingForClipboard, Loading, Ready, Error };

    std::atomic<FlashLinkState> state{ FlashLinkState::WaitingForClipboard };
    std::string current_flashlink;
    std::atomic<uint64_t> search_token{ 0 };
    char input_buffer[128] = "";

    std::mutex result_mutex_;
    ContentFetchResult fetch_result_;
    int selected_release_index_ = 0;

    void startSearch(const std::string &flashlink);

    bool tryProcessCandidate(const std::string &raw, bool clear_input_on_success);

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // CONTENT_BROWSER_ADD_WINDOW_HPP