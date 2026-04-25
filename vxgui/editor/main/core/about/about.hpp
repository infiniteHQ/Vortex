//
//  about.hpp
//  Header and declarations for the about window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef ABOUT_WINDOW_HPP
#define ABOUT_WINDOW_HPP

namespace vxe {

  class AboutVortex : public std::enable_shared_from_this<AboutVortex> {
   public:
    AboutVortex(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<AboutVortex> create(const std::string &name);
    void setup_render_callback();

    // rendering
    void render();

   private:
    std::shared_ptr<Cherry::AppWindow> app_window_;
  };

}  // namespace vxe

#endif  // ABOUT_WINDOW_HPP