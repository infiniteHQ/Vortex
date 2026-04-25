//
//  doc_viewer.hpp
//  Header and declarations for the documentation viewer, including rendering
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

#ifndef VORTEX_EDITOR_DOC_VIEWER_HPP
#define VORTEX_EDITOR_DOC_VIEWER_HPP

namespace vxe {

  class DocViewer : public std::enable_shared_from_this<DocViewer> {
   public:
    DocViewer(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<DocViewer> create(const std::string &name);
    void setup_render_callback();

    // rendering
    void render_right_menubar();
    void render_left_menubar();
    void render();

    // utils
    void load_markdown(const std::string &path);
    std::string get_label_for_topic(const std::string &topic);

   private:
    // logic
    std::string selected_topic_ = "vx";
    std::string selected_section_ = "";
    std::string selected_chapter_ = "";
    std::string cached_markdown_content_ = "";

    // ui
    float sidebar_width_ = 250.0f;
    bool console_font_ = false;

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // VORTEX_EDITOR_DOC_VIEWER_HPP
