//
//  flash_link_window.hpp
//  Headers and declarations for the "flash link window" of the modules utility
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../../../vxcore/include/vortex.h"
#include "../../../../../../../vxcore/include/vortex_internals.h"

#ifndef MODULES_UTILITY_FLASH_LINK_WINDOW_HPP
#define MODULES_UTILITY_FLASH_LINK_WINDOW_HPP

namespace vxe {

  class FlashLinkWindow : public std::enable_shared_from_this<FlashLinkWindow> {
   public:
    FlashLinkWindow(const std::string &name);

    // window and rendering
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<FlashLinkWindow> create(const std::string &name);
    void setup_render_callback();
    void render();

   private:
    std::string creation_path_;
    std::string add_window_search_;

    std::shared_ptr<Cherry::AppWindow> app_window_;
    bool clipboard_checked = false;
    bool detected = false;
    std::string decoded_text;

    enum class FlashLinkState { WaitingForClipboard, Loading, Ready };

    FlashLinkState state = FlashLinkState::WaitingForClipboard;
    std::string current_flashlink;
    std::atomic<uint64_t> search_token{ 0 };
    char input_buffer[128] = "";

    void startSearch(const std::string &flashlink);
  };
}  // namespace vxe

#endif  // MODULES_UTILITY_FLASH_LINK_WINDOW_HPP