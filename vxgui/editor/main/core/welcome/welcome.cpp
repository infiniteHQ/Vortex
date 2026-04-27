//
//  welcome.cpp
//  Sources of rendering and lua script integration for the "welcome" window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./welcome.hpp"

#include "../../editor.hpp"

namespace vxe {

  Welcome::Welcome(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [=]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(0.0f);
    app_window_->SetInternalPaddingY(0.0f);
  }

  std::shared_ptr<Cherry::AppWindow> &Welcome::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<Welcome> Welcome::create(const std::string &name) {
    auto instance = std::shared_ptr<Welcome>(new Welcome(name));
    instance->setup_render_callback();
    return instance;
  }

  void Welcome::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void Welcome::render() {
    Cherry::script::render_lua_fresh_script(Cherry::GetPath("ui/windows/welcome/main.lua"));

    // If Browser Clicked
    if (CherryApp.GetComponentData(CherryID("welcome_browser"), "isClicked") == "true") {
      c_Editor->spawn_content_browser_on_bottom();
    }

    // If Terminal Clicked
    if (CherryApp.GetComponentData(CherryID("welcome_terminal"), "isClicked") == "true") {
      c_Editor->spawn_logs_utility();
    }

    // If Scripts Clicked
    if (CherryApp.GetComponentData(CherryID("welcome_settings"), "isClicked") == "true") {
      c_Editor->toggle_project_settings();
    }

    if (CherryApp.GetComponentData(CherryID("open_terminal"), "isClicked") == "true") {
      c_Editor->spawn_logs_utility();
    }

    if (CherryApp.GetComponentData(CherryID("open_content_browser"), "isClicked") == "true") {
      c_Editor->spawn_content_browser();
    }

    if (CherryApp.GetComponentData(CherryID("open_project_settings"), "isClicked") == "true") {
      c_Editor->toggle_project_settings();
    }

    if (CherryApp.GetComponentData(CherryID("taking_control"), "isClicked") == "true") {
      vxe::open_url(
          "https://vortex.infinite.si/"
          "docpage?version=1.0&content_name=take_vortex_editor&"
          "section=get_started&page_name=discover_interface");
    }

    if (CherryApp.GetComponentData(CherryID("learn_modules"), "isClicked") == "true") {
      vxe::open_url(
          "https://vortex.infinite.si/"
          "docpage?version=1.0&content_name=take_vortex_editor&"
          "section=get_started&page_name=handle_modules");
    }

    if (CherryApp.GetComponentData(CherryID("visit_website"), "isClicked") == "true") {
      vxe::open_url("https://vortex.infinite.si/");
    }
  }

}  // namespace vxe
