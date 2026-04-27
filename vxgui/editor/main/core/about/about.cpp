//
//  about.cpp
//  Rendering sources for the about window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./about.hpp"

namespace vxe {
  AboutVortex::AboutVortex(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [=]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

#if defined(__linux__)
    system_desktop = " - " + vxe::get_linux_desktop_env_and_display_server();
#endif

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &AboutVortex::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<AboutVortex> AboutVortex::create(const std::string &name) {
    auto instance = std::shared_ptr<AboutVortex>(new AboutVortex(name));
    instance->setup_render_callback();
    return instance;
  }

  void AboutVortex::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void AboutVortex::render() {
    float window_width = CherryGUI::GetWindowSize().x;
    float image_height = window_width / 3.435f;

    CherryGUI::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortex_banner.png")), ImVec2(window_width, image_height));

    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));  // CherryStyle::Padding
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));   // CherryStyle::Padding
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryStyle::AddMarginX(20.0f);
    CherryStyle::AddMarginY(5.0f);
    CherryGUI::BeginChild("aboutchild", ImVec2(430, 0));  // cherry api

    Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
    CherryKit::TextSimple("Vortex Editor");

    std::string vortex_launcher_version = VORTEX_VERSION;
    std::string cherry_version = CHERRY_VERSION;
    std::string os_name = OS_NAME;
    std::string os_arch = ARCH_NAME;

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Version: " + vortex_launcher_version);

    CherryGUI::SameLine();
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple(vxe::get_vortex_build_name());

    CherryKit::Space(12.0f);

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("Cherry version: " + cherry_version);
    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple(
        "Build: " + vxe::get_vortex_build_id() + " ; " + vxe::get_build_date() + " (" + vxe::get_vortex_editor_dist() + ")");
    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple(
        "Hash: "
        " exe(" +
        vxe::get_vortex_editor_hash() + ") git(" + vxe::get_git_commit() + ")");

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextSimple("System: " + os_name + " " + os_arch + system_desktop);

    CherryGUI::EndChild();
    CherryGUI::SameLine();

    CherryGUI::BeginChild("link");  // cherry api

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");

    if (CherryKit::ButtonImageTextImage(
            "Support Us",
            Cherry::GetPath("resources/imgs/icons/editor/heart.png"),
            Cherry::GetPath("resources/imgs/weblink.png"))
            .GetData("isClicked") == "true") {
      vxe::open_url("https://fund.infinite.si/");
    }

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");
    if (CherryKit::ButtonImageTextImage(
            "Credits & Contributors",
            Cherry::GetPath("resources/imgs/icons/misc/icon_people.png"),
            Cherry::GetPath("resources/imgs/weblink.png"))
            .GetData("isClicked") == "true") {
      vxe::open_url("https://github.com/infiniteHQ/Vortex/graphs/contributors");
    }

    CherryNextProp("color_bg", "#00000000");
    CherryNextProp("color_border", "#00000000");
    CherryNextProp("padding_x", "2");
    CherryNextProp("padding_y", "2");
    CherryNextProp("size_x", "20");
    CherryNextProp("size_y", "20");
    if (CherryKit::ButtonImageTextImage(
            "Learn and Documentation",
            Cherry::GetPath("resources/imgs/icons/editor/docs.png"),
            Cherry::GetPath("resources/imgs/weblink.png"))
            .GetData("isClicked") == "true") {
      vxe::open_url("https://vortex.infinite.si/learn");
    }

    CherryKit::Space(1.0f);
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple("Vortex is a open and free software.");
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple("Licensed under the  Apache-2.0 license ");
    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple("Never stop hacking !");

    CherryGUI::EndChild();

    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(2);  // CherryStyle::Padding
  }

}  // namespace vxe