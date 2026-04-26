//
//  credits.cpp
//  Sources of rendering and logics features for the "credits" window.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./credits.hpp"

namespace vxe {
  Credits::Credits(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };
    app_window_->SetLeftMenubarCallback([this]() { render_left_menubar(); });

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

#if defined(__linux__)
    system_desktop = " - " + vxe::GetLinuxDesktopEnvAndDisplayServer();
#endif

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &Credits::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<Credits> Credits::create(const std::string &name) {
    auto instance = std::shared_ptr<Credits>(new Credits(name));
    instance->setup_render_Callback();
    return instance;
  }

  void Credits::setup_render_Callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }
  void Credits::render() {
    auto ctx = vxe::get_current_context();
    float window_width = CherryGUI::GetWindowSize().x;
    float image_height = window_width / 3.435f;
    CherryGUI::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/credits_banner.png")), ImVec2(window_width, image_height));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryStyle::AddMarginX(20.0f);
    CherryStyle::AddMarginY(5.0f);
    CherryGUI::BeginChild("aboutchild", ImVec2(430, 0));

    float maxScroll = CherryGUI::GetScrollMaxY();
    float dt = CherryGUI::GetIO().DeltaTime;
    if (auto_scroll_) {
      if (CherryGUI::IsWindowHovered() && CherryGUI::GetIO().MouseWheel != 0.0f) {
        user_scrolled_ = true;
        user_scroll_cooldown_ = user_scroll_pause_;
      }
      if (user_scrolled_) {
        user_scroll_cooldown_ -= dt;
        if (user_scroll_cooldown_ <= 0.0f) {
          user_scrolled_ = false;
          scroll_y_ = CherryGUI::GetScrollY();
        }
      } else {
        scroll_y_ += scroll_speed_ * dt;
        if (scroll_y_ >= maxScroll)
          scroll_y_ = 0.0f;
        CherryGUI::SetScrollY(scroll_y_);
      }
    }

    auto RenderTopic = [&](const std::string &topic, bool is_main) {
      std::vector<std::string> titles = vxe::get_titles_from_topic(topic);
      if (titles.empty())
        return;

      CherryStyle::AddMarginY(20.0f);

      if (is_main) {
        std::string vortex_version = VORTEX_VERSION;
        Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
        CherryKit::TitleTwo("Vortex Editor (" + vortex_version + ")");
      } else {
        Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
        CherryKit::TitleThree(topic);
      }

      for (const auto &title : titles) {
        CherryStyle::AddMarginY(8.0f);
        Cherry::SetNextComponentProperty("color_text", "#BBBBBB");
        CherryKit::TitleSix(title);

        std::vector<std::string> names = vxe::get_names_from_topic_and_title(topic, title);
        for (const auto &name : names) {
          Cherry::SetNextComponentProperty("color_text", "#878787");
          CherryKit::TextSimple(name);
        }
      }
    };

    RenderTopic("vx", true);

    for (const auto &[topic, _] : ctx->credits) {
      if (topic == "vx")
        continue;
      RenderTopic(topic, false);
    }

    CherryStyle::AddMarginY(20.0f);
    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);
    CherryGUI::PopStyleVar(2);
  }

  void Credits::render_left_menubar() {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("disable_callback", "true");
    if (CherryKit::ButtonImageTextDropdown("View", Cherry::GetPath("resources/imgs/icons/misc/icon_eye.png"))
            .GetDataAs<bool>("isClicked")) {
      ImVec2 mousePos = CherryGUI::GetMousePos();
      ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
      ImVec2 popupSize(350, 100);

      if (mousePos.x + popupSize.x > displaySize.x) {
        mousePos.x -= popupSize.x;
      }
      if (mousePos.y + popupSize.y > displaySize.y) {
        mousePos.y -= popupSize.y;
      }

      CherryGUI::SetNextWindowSize(popupSize, ImGuiCond_Appearing);
      CherryGUI::SetNextWindowPos(mousePos, ImGuiCond_Appearing);
      CherryGUI::OpenPopup("ViewMenuPopup");
    }
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);               // TODO : Props
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);      // TODO : Props
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // TODO : Props
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);        // TODO : Props

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (CherryGUI::BeginPopup("ViewMenuPopup")) {
      CherryKit::SeparatorText("Pannels");
      CherryKit::CheckboxText("Auto scroll", &auto_scroll_);

      int default_index = 0;

      switch (CherryNextComponent.SetProperty("size_x", 150.0f);
              CherryKit::ComboImageText(
                  "",
                  { { "All", Cherry::GetPath("resources/imgs/icons/misc/icon_thumbnails.png") },
                    { "Modules", Cherry::GetPath("resources/imgs/icons/misc/icon_lines.png") },
                    { "Objects", Cherry::GetPath("resources/imgs/icons/misc/icon_objects.png") } },
                  default_index)
                  .GetPropertyAs<int>("selected")) {
        case 0: {
          break;
        }
        case 1: {
          break;
        }
        case 2: {
          break;
        }
        default: {
          break;
        }
      }
    }

    CherryGUI::EndPopup();
  }

  void Credits::set_selected_category(const std::string &c) {
    selected_category_ = c;
    selected_category_changed_ = true;
  }

  void Credits::refresh_categories() {
    all_categories_.clear();
    for (int i = 0; i < vxe::get_current_context()->IO.em.size(); i++) {
      if (!vxe::get_current_context()->IO.em[i]) {
        continue;
      }
      all_categories_[vxe::get_current_context()->IO.em[i]->m_group]++;
    }
  }

  std::string Credits::get_selected_category() {
    return selected_category_;
  }

}  // namespace vxe
