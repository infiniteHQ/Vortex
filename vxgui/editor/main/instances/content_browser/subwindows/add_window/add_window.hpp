//
//  add_window.hpp
//  Headers and declarations for the "add window" of the content browser
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../../../vxcore/include/vortex.h"
#include "../../../../../../../vxcore/include/vortex_internals.h"

#ifndef CONTENT_BROWSER_ADD_WINDOW_HPP
#define CONTENT_BROWSER_ADD_WINDOW_HPP

namespace vxe {

  class AddWindow : public std::enable_shared_from_this<AddWindow> {
   public:
    AddWindow(const std::string &name, const std::string &path);

    // window and rendering
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<AddWindow> create(const std::string &name, const std::string &path);
    void setup_render_callback();
    void render();

    // utils
    void set_create_file_callback(const std::function<void()> &callback);
    void set_create_folder_callback(const std::function<void()> &callback);
    void set_import_content_callback(const std::function<void()> &callback);

   private:
    // callbacks
    std::function<void()> create_file_callback_;
    std::function<void()> create_folder_callback_;
    std::function<void()> import_content_callback_;

    std::string creation_path_;
    std::string add_window_search_;

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // CONTENT_BROWSER_ADD_WINDOW_HPP