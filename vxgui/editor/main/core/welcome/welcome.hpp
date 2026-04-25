//
//  welcome.hpp
//  Header and declarations for the "welcome" window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef WELCOME_HPP
#define WELCOME_HPP

namespace vxe {

  class Welcome : public std::enable_shared_from_this<Welcome> {
   public:
    Welcome(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<Welcome> create(const std::string &name);
    void setup_render_callback();

    // rendering
    void render();

   private:
    // window
    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe
#endif  // WELCOME_HPP