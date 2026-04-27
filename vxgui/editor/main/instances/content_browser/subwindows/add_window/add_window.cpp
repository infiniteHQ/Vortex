//
//  add_window.cpp
//  Sources for the "add window" of the content browser
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./add_window.hpp"

namespace vxe {
  AddWindow::AddWindow(const std::string &name, const std::string &path) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [this]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(8.0f);
    app_window_->SetInternalPaddingY(8.0f);

    // TODO : Calc text size for translations
    app_window_->SetRightBottombarCallback([this]() {
      CherryStyle::RemoveMarginY(7.5f);
      CherryStyle::RemoveMarginX(130.0f);
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      if (CherryKit::ButtonImageText("Cancel", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
              .GetDataAs<bool>("isClicked")) {
        Cherry::DeleteAppWindow(app_window_);
      }

      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");

      if (CherryKit::ButtonImageText(
              Cherry::GetLocale("loc.window.content.content_browser.add"),
              Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
              .GetDataAs<bool>("isClicked")) {
      }
    });

    creation_path_ = path;

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &AddWindow::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<AddWindow> AddWindow::create(const std::string &name, const std::string &path) {
    auto instance = std::shared_ptr<AddWindow>(new AddWindow(name, path));
    instance->setup_render_callback();
    return instance;
  }

  void AddWindow::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void AddWindow::render() {
    float window_width = CherryGUI::GetWindowSize().x;
    float main_header_width = window_width - 46.0f;
    float main_buttons_width = window_width - 17.0f;
    float main_image_height = window_width / 4.720f;

    CherryGUI::Image(
        Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_add.png")), ImVec2(window_width, main_image_height));

    CherryStyle::AddMarginX(10.0f);
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText("Filters", Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"))
            .GetDataAs<bool>("isClicked")) {
    }

    CherryGUI::SameLine();

    CherryNextComponent.SetProperty("size_x", "240");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("description", "Search content...");
    CherryNextComponent.SetProperty(
        "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
    CherryNextComponent.SetProperty("description_logo_place", "r");
    CherryKit::InputString("", &add_window_search_);

    CherryStyle::AddMarginX(8.0f);
    CherryKit::SeparatorText("Basics");
    // Little creation block
    CherryStyle::AddMarginX(8.0f);
    CherryNextComponent.SetProperty("size_x", main_buttons_width);
    CherryNextComponent.SetProperty("block_color", "#121212");
    CherryNextComponent.SetProperty("block_border_color", "#232323");
    if (CherryKit::BlockHorizontalCustom(
            [main_buttons_width]() {},
            main_buttons_width,
            25.0f,
            {
                [main_buttons_width]() {
                  CherryNextComponent.SetProperty("size_x", "18.0f");
                  CherryStyle::RemoveMarginY(2.0f);
                  CherryStyle::AddMarginX(5.0f);
                  CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/icons/misc/icon_add_folder.png"), 18.0f, 18.0f);
                },
                []() {
                  CherryStyle::AddMarginY(6.0f);
                  CherryNextComponent.SetProperty("color_text", "#CDCDCD");
                  CherryKit::TextSimple("New folder");
                },
            })
            .GetDataAs<bool>("isClicked")) {
      if (create_folder_callback_) {
        create_folder_callback_();
      }
      Cherry::DeleteAppWindow(app_window_);
    }

    // Little creation block
    CherryStyle::AddMarginX(8.0f);
    CherryNextComponent.SetProperty("size_x", main_buttons_width);
    if (CherryKit::BlockHorizontalCustom(
            [main_buttons_width]() {},
            main_buttons_width,
            25.0f,
            {
                [main_buttons_width]() {
                  CherryNextComponent.SetProperty("size_x", "18.0f");
                  CherryStyle::RemoveMarginY(2.0f);
                  CherryStyle::AddMarginX(5.0f);
                  CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/icons/misc/icon_add_file.png"), 18.0f, 18.0f);
                },
                []() {
                  CherryStyle::AddMarginY(8.0f);
                  CherryNextComponent.SetProperty("color_text", "#CDCDCD");
                  CherryKit::TextSimple("New file");
                },
            },
            2)
            .GetDataAs<bool>("isClicked")) {
      if (create_file_callback_) {
        create_file_callback_();
      }
      Cherry::DeleteAppWindow(app_window_);
    }

    // Little creation block
    CherryStyle::AddMarginX(8.0f);
    CherryNextComponent.SetProperty("size_x", main_buttons_width);
    if (CherryKit::BlockHorizontalCustom(
            [main_buttons_width]() {},
            main_buttons_width,
            25.0f,
            {
                [main_buttons_width]() {
                  CherryNextComponent.SetProperty("size_x", "25.0f");
                  CherryStyle::RemoveMarginY(6.0f);
                  CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/favicon.png"), 25.0f, 25.0f);
                },
                []() {
                  CherryStyle::AddMarginY(6.0f);
                  CherryNextComponent.SetProperty("color_text", "#CDCDCD");
                  CherryKit::TitleSix("Import content");
                },
            },
            3)
            .GetDataAs<bool>("isClicked")) {
      if (import_content_callback_) {
        import_content_callback_();
      }
      Cherry::DeleteAppWindow(app_window_);
    }
    CherryStyle::AddMarginX(8.0f);
    CherryKit::SeparatorText("Basics");
    CherryStyle::AddMarginX(8.0f);
    CherryNextComponent.SetProperty("size_x", main_header_width);
    CherryNextComponent.SetProperty("color_border", "#343434");
    CherryNextComponent.SetProperty("color_bg", "#232323");
    CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
    CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
    CherryKit::HeaderImageText("Files", Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"), [&]() {
      float header_width = window_width - 17.0f;
      // Little creation block
      CherryStyle::AddMarginX(8.0f);
      CherryNextComponent.SetProperty("size_x", header_width);
      CherryKit::BlockHorizontalCustom(
          [header_width]() {},
          header_width,
          25.0f,
          {
              [header_width]() {
                CherryNextComponent.SetProperty("size_x", "25.0f");
                CherryStyle::RemoveMarginY(6.0f);
                CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/favicon.png"), 25.0f, 25.0f);
              },
              []() {
                CherryStyle::AddMarginY(6.0f);
                CherryKit::TitleSix("New file");
              },
          });

      // Big creation block
      CherryStyle::AddMarginX(8.0f);
      CherryNextComponent.SetProperty("size_x", header_width);
      CherryKit::BlockHorizontalCustom(
          [header_width]() {},
          header_width,
          60.0f,
          {
              []() {
                CherryNextComponent.SetProperty("size_x", "60.0f");
                CherryStyle::RemoveMarginY(6.0f);
                CherryKit::ImageLocal(Cherry::GetPath("resources/imgs/favicon.png"), 60.0f, 60.0f);
              },
              []() {
                CherryStyle::AddMarginY(6.0f);
                CherryGUI::BeginChild("test2", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
                CherryNextComponent.SetProperty("color_text", "#B1FF31");
                CherryKit::TitleSix("Markdown file");
                CherryNextComponent.SetProperty("color_text", "#B1FF31");
                CherryKit::TextSimple("asf");
                CherryKit::TextSimple("from the module :");
                CherryGUI::EndChild();
              },
          });
    });

    CherryStyle::AddMarginX(8.0f);
    CherryKit::SeparatorText("From modules");

    for (auto m : vxe::get_current_context()->IO.em) {
      if (m) {
        auto item_creators = m->get_content_browser_item_creators();
        if (!item_creators.empty()) {
          float window_width = CherryGUI::GetWindowSize().x;
          float header_width = window_width - 17.0f;

          CherryStyle::AddMarginX(8.0f);
          CherryNextComponent.SetProperty("size_x", main_header_width);
          CherryNextComponent.SetProperty("color_border", "#343434");
          CherryNextComponent.SetProperty("color_bg", "#232323");
          CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
          CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
          CherryKit::HeaderImageText(
              m->proper_name(),
              Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),  // module logo
              [&]() {
                for (auto ic : item_creators) {
                  CherryStyle::AddMarginX(8.0f);
                  CherryNextComponent.SetProperty("size_x", header_width);
                  if (CherryKit::BlockHorizontalCustom(
                          [&]() {},
                          header_width,
                          60.0f,
                          {
                              [&]() {
                                CherryNextComponent.SetProperty("size_x", "60.0f");
                                CherryStyle::RemoveMarginY(6.0f);
                                CherryKit::ImageLocal(ic->logo_path, 60.0f, 60.0f);
                              },
                              [&]() {
                                CherryStyle::AddMarginY(6.0f);
                                CherryGUI::BeginChild("test2", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
                                CherryNextComponent.SetProperty("color_text", "#B1FF31");
                                CherryKit::TitleSix(ic->name);
                                CherryNextComponent.SetProperty("color_text", "#B1FF31");
                                CherryKit::TextSimple(ic->description);
                                CherryKit::TextSimple("from the module :");
                                CherryGUI::EndChild();
                              },
                          })
                          .GetDataAs<bool>("isClicked")) {
                    if (ic->create_function && !creation_path_.empty()) {
                      ic->create_function(creation_path_);
                    }
                    Cherry::DeleteAppWindow(app_window_);
                  }
                }
              });
        }
      }
    }
  }

  void AddWindow::set_create_file_callback(const std::function<void()> &callback) {
    create_file_callback_ = callback;
  }

  void AddWindow::set_create_folder_callback(const std::function<void()> &callback) {
    create_folder_callback_ = callback;
  }

  void AddWindow::set_import_content_callback(const std::function<void()> &callback) {
    import_content_callback_ = callback;
  }

}  // namespace vxe
