//
//  about_project.cpp
//  Rendering sources for the "about project" window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./about_project.hpp"

namespace vxe {
  AboutProject::AboutProject(const std::string &name) {
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

  std::shared_ptr<Cherry::AppWindow> &AboutProject::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<AboutProject> AboutProject::create(const std::string &name) {
    auto instance = std::shared_ptr<AboutProject>(new AboutProject(name));
    instance->setup_render_callback();
    return instance;
  }

  void AboutProject::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void AboutProject::render() {
    float window_width = CherryGUI::GetWindowSize().x;
    float image_height = window_width / 3.835f;

    std::string project_version = vxe::get_current_context()->version;
    std::string project_name = vxe::get_current_context()->name;
    std::string project_description = vxe::get_current_context()->description;
    std::string project_author = vxe::get_current_context()->author;
    std::string logo_path_str =
        vxe::get_current_context()->projectPath.string() + "" + vxe::get_current_context()->logo_path;
    std::string banner_path_str =
        vxe::get_current_context()->projectPath.string() + "" + vxe::get_current_context()->banner_path;

    std::string logo_path = std::filesystem::exists(logo_path_str) && std::filesystem::is_regular_file(logo_path_str)
                                ? logo_path_str
                                : Cherry::GetPath("resources/imgs/default_logo.png");

    std::string banner_path = std::filesystem::exists(banner_path_str) && std::filesystem::is_regular_file(banner_path_str)
                                  ? banner_path_str
                                  : Cherry::GetPath("resources/imgs/default_banner.png");

    CherryGUI::Image(Cherry::GetTexture(banner_path), ImVec2(window_width, image_height));

    CherryStyle::AddMarginX(25.0f);
    CherryStyle::RemoveMarginY(25.0f);
    CherryGUI::Image(Cherry::GetTexture(logo_path), ImVec2(65, 65));

    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));  // CherryStyle::Padding
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));   // CherryStyle::Padding
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryStyle::AddMarginX(20.0f);
    CherryStyle::AddMarginY(5.0f);
    CherryGUI::BeginChild("aboutchild", ImVec2(430, 0));  // cherry api

    Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
    CherryKit::TextSimple(project_name);
    CherryGUI::SameLine();
    Cherry::SetNextComponentProperty("color_text", "#343434");
    CherryKit::TextSimple(project_version);

    if (vxe::get_current_context()->type == "tool") {
      CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/tools.png"), 13, 13);
      CherryGUI::SameLine();
      Cherry::SetNextComponentProperty("color_text", "#565656");
      CherryKit::TextSimple("Tool");

    } else if (vxe::get_current_context()->type == "project") {
      CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/light.png"), 13, 13);
      CherryGUI::SameLine();
      Cherry::SetNextComponentProperty("color_text", "#565656");
      CherryKit::TextSimple("Project");
    }

    Cherry::SetNextComponentProperty("color_text", "#878787");
    CherryKit::TextWrapped(project_description);

    Cherry::SetNextComponentProperty("color_text", "#565656");
    CherryKit::TextSimple(project_author);

    CherryGUI::EndChild();

    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(2);  // CherryStyle::Padding
  }

}  // namespace vxe
