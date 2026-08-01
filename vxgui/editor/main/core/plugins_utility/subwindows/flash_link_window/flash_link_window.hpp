//
//  flash_link_window.hpp
//  Headers and declarations for the "flash link window" of the plugins utility
//
//  Copyright (c) 2026 Infinite
//
//  This work is licensed under the terms of the Apache-2.0 license.
//  For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../../../vxcore/include/vortex.h"
#include "../../../../../../../vxcore/include/vortex_internals.h"

#ifndef PLUGINS_UTILITY_FLASH_LINK_WINDOW_HPP
#define PLUGINS_UTILITY_FLASH_LINK_WINDOW_HPP

namespace vxe {

  struct PluginRelease {
    std::string uuid;
    std::string name;
    std::string platform;
    std::string arch;
    std::string major;
    std::string version;
  };

  struct PluginInfo {
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

  struct PluginFetchResult {
    bool success = false;
    std::string error;
    PluginInfo info;
    std::vector<PluginRelease> compatible_releases;
    std::vector<PluginRelease> compatible_cross_releases;
  };

  PluginFetchResult fetch_plugin_from_flashlink(const std::string &flashlink);

  class FlashLinkPluginWindow : public std::enable_shared_from_this<FlashLinkPluginWindow> {
   public:
    FlashLinkPluginWindow(const std::string &name, const std::string &mode);

    // window and rendering
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<FlashLinkPluginWindow> create(const std::string &name, const std::string &mode);
    void setup_render_callback();
    void render();

   private:
    std::string creation_path_;
    std::string add_window_search_;

    std::shared_ptr<vxe::ModuleInstallProgress> install_progress_;
    std::shared_ptr<Cherry::AppWindow> app_window_;
    bool clipboard_checked = false;
    bool detected = false;
    std::string decoded_text;

    std::string mode_;
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
    PluginFetchResult fetch_result_;
    int selected_release_index_ = 0;

    void startSearch(const std::string &flashlink);

    bool tryProcessCandidate(const std::string &raw, bool clear_input_on_success);

    void renderCloseButton();
  };
}  // namespace vxe

#endif  // PLUGINS_UTILITY_FLASH_LINK_WINDOW_HPP