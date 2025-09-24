#include "./add_window.hpp"

namespace VortexEditor {
static std::string AddWindowSearch;
ContentBrowserAddWindow::ContentBrowserAddWindow(const std::string &name,
                                                 const std::string &path) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [this]() {
    m_AppWindow->SetVisibility(false);
  };

  m_AppWindow->SetInternalPaddingX(8.0f);
  m_AppWindow->SetInternalPaddingY(8.0f);

  // TODO : Calc text size for translations
  m_AppWindow->SetRightBottombarCallback([this]() {
    CherryStyle::RemoveMarginY(7.5f);
    CherryStyle::RemoveMarginX(130.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
    CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText(
            "Cancel",
            Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
            .GetDataAs<bool>("isClicked")) {
      Cherry::DeleteAppWindow(m_AppWindow);
    }

    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");

    if (CherryKit::ButtonImageText(
            Cherry::GetLocale("loc.window.content.content_browser.add"),
            Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
            .GetDataAs<bool>("isClicked")) {
    }
  });

  m_CreationPath = path;

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

std::shared_ptr<Cherry::AppWindow> &ContentBrowserAddWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<ContentBrowserAddWindow>
ContentBrowserAddWindow::Create(const std::string &name,
                                const std::string &path) {
  auto instance = std::shared_ptr<ContentBrowserAddWindow>(
      new ContentBrowserAddWindow(name, path));
  instance->SetupRenderCallback();
  return instance;
}

void ContentBrowserAddWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void ContentBrowserAddWindow::Render() {
  float window_width = CherryGUI::GetWindowSize().x;
  float main_header_width = window_width - 46.0f;
  float main_buttons_width = window_width - 17.0f;
  float main_image_height = window_width / 4.720f;

  CherryGUI::Image(
      Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_add.png")),
      ImVec2(window_width, main_image_height));

  CherryStyle::AddMarginX(10.0f);
  CherryNextComponent.SetProperty("color_border", "#00000000");
  CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
  CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  if (CherryKit::ButtonImageText(
          "Filters",
          Cherry::GetPath("resources/imgs/icons/misc/icon_filter.png"))
          .GetDataAs<bool>("isClicked")) {
  }

  CherryGUI::SameLine();

  CherryNextComponent.SetProperty("size_x", "240");
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  CherryNextComponent.SetProperty("description", "Search content...");
  CherryNextComponent.SetProperty(
      "description_logo",
      Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
  CherryNextComponent.SetProperty("description_logo_place", "r");
  CherryKit::InputString("", &AddWindowSearch);

  CherryStyle::AddMarginX(8.0f);
  CherryKit::SeparatorText("Basics");
  // Little creation block
  CherryStyle::AddMarginX(8.0f);
  CherryNextComponent.SetProperty("size_x", main_buttons_width);
  CherryNextComponent.SetProperty("block_color", "#121212");
  CherryNextComponent.SetProperty("block_border_color", "#232323");
  if (CherryKit::BlockHorizontalCustom(
          [main_buttons_width]() {}, main_buttons_width, 25.0f,
          {
              [main_buttons_width]() {
                CherryNextComponent.SetProperty("size_x", "18.0f");
                CherryStyle::RemoveMarginY(2.0f);
                CherryStyle::AddMarginX(5.0f);
                CherryKit::ImageLocal(
                    Cherry::GetPath(
                        "resources/imgs/icons/misc/icon_add_folder.png"),
                    18.0f, 18.0f);
              },
              []() {
                CherryStyle::AddMarginY(6.0f);
                CherryNextComponent.SetProperty("color_text", "#CDCDCD");
                CherryKit::TextSimple("New folder");
              },
          })
          .GetDataAs<bool>("isClicked")) {
    if (m_CreateFolderCallback) {
      m_CreateFolderCallback();
    }
    Cherry::DeleteAppWindow(m_AppWindow);
  }

  // Little creation block
  CherryStyle::AddMarginX(8.0f);
  CherryNextComponent.SetProperty("size_x", main_buttons_width);
  if (CherryKit::BlockHorizontalCustom(
          [main_buttons_width]() {}, main_buttons_width, 25.0f,
          {
              [main_buttons_width]() {
                CherryNextComponent.SetProperty("size_x", "18.0f");
                CherryStyle::RemoveMarginY(2.0f);
                CherryStyle::AddMarginX(5.0f);
                CherryKit::ImageLocal(
                    Cherry::GetPath(
                        "resources/imgs/icons/misc/icon_add_file.png"),
                    18.0f, 18.0f);
              },
              []() {
                CherryStyle::AddMarginY(8.0f);
                CherryNextComponent.SetProperty("color_text", "#CDCDCD");
                CherryKit::TextSimple("New file");
              },
          },
          2)
          .GetDataAs<bool>("isClicked")) {
    if (m_CreateFileCallback) {
      m_CreateFileCallback();
    }
    Cherry::DeleteAppWindow(m_AppWindow);
  }

  // Little creation block
  CherryStyle::AddMarginX(8.0f);
  CherryNextComponent.SetProperty("size_x", main_buttons_width);
  if (CherryKit::BlockHorizontalCustom(
          [main_buttons_width]() {}, main_buttons_width, 25.0f,
          {
              [main_buttons_width]() {
                CherryNextComponent.SetProperty("size_x", "25.0f");
                CherryStyle::RemoveMarginY(6.0f);
                CherryKit::ImageLocal(
                    Cherry::GetPath("resources/imgs/favicon.png"), 25.0f,
                    25.0f);
              },
              []() {
                CherryStyle::AddMarginY(6.0f);
                CherryNextComponent.SetProperty("color_text", "#CDCDCD");
                CherryKit::TitleSix("Import content");
              },
          },
          3)
          .GetDataAs<bool>("isClicked")) {
    if (m_ImportContentCallback) {
      m_ImportContentCallback();
    }
    Cherry::DeleteAppWindow(m_AppWindow);
  }
  CherryStyle::AddMarginX(8.0f);
  CherryKit::SeparatorText("Basics");
  CherryStyle::AddMarginX(8.0f);
  CherryNextComponent.SetProperty("size_x", main_header_width);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Files",
      Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),
      [&]() {
        float header_width = window_width - 17.0f;
        // Little creation block
        CherryStyle::AddMarginX(8.0f);
        CherryNextComponent.SetProperty("size_x", header_width);
        CherryKit::BlockHorizontalCustom(
            [header_width]() {}, header_width, 25.0f,
            {
                [header_width]() {
                  CherryNextComponent.SetProperty("size_x", "25.0f");
                  CherryStyle::RemoveMarginY(6.0f);
                  CherryKit::ImageLocal(
                      Cherry::GetPath("resources/imgs/favicon.png"), 25.0f,
                      25.0f);
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
            [header_width]() {}, header_width, 60.0f,
            {
                []() {
                  CherryNextComponent.SetProperty("size_x", "60.0f");
                  CherryStyle::RemoveMarginY(6.0f);
                  CherryKit::ImageLocal(
                      Cherry::GetPath("resources/imgs/favicon.png"), 60.0f,
                      60.0f);
                },
                []() {
                  CherryStyle::AddMarginY(6.0f);
                  CherryGUI::BeginChild("test2", ImVec2(0, 0), false,
                                        ImGuiWindowFlags_NoScrollbar);
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

  for (auto m : VortexMaker::GetCurrentContext()->IO.em) {
    if (m) {
      auto item_creators = m->GetContentBrowserItemCreators();
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
            m->m_proper_name,
            Cherry::Application::CookPath(
                "resources/imgs/icons/misc/icon_star.png"), // module logo
            [&]() {
              for (auto ic : item_creators) {
                CherryStyle::AddMarginX(8.0f);
                CherryNextComponent.SetProperty("size_x", header_width);
                if (CherryKit::BlockHorizontalCustom(
                        [&]() {}, header_width, 60.0f,
                        {
                            [&]() {
                              CherryNextComponent.SetProperty("size_x",
                                                              "60.0f");
                              CherryStyle::RemoveMarginY(6.0f);
                              CherryKit::ImageLocal(ic->m_Logo, 60.0f, 60.0f);
                            },
                            [&]() {
                              CherryStyle::AddMarginY(6.0f);
                              CherryGUI::BeginChild(
                                  "test2", ImVec2(0, 0), false,
                                  ImGuiWindowFlags_NoScrollbar);
                              CherryNextComponent.SetProperty("color_text",
                                                              "#B1FF31");
                              CherryKit::TitleSix(ic->m_Name);
                              CherryNextComponent.SetProperty("color_text",
                                                              "#B1FF31");
                              CherryKit::TextSimple(ic->m_Description);
                              CherryKit::TextSimple("from the module :");
                              CherryGUI::EndChild();
                            },
                        })
                        .GetDataAs<bool>("isClicked")) {
                  if (ic->f_CreateFunction && !m_CreationPath.empty()) {
                    ic->f_CreateFunction(m_CreationPath);
                  }
                  Cherry::DeleteAppWindow(m_AppWindow);
                }
              }
            });
      }
    }
  }
}

} // namespace VortexEditor
