#include "./add_window.hpp"

namespace VortexEditor {
static std::string AddWindowSearch;
ContentBrowserAddWindow::ContentBrowserAddWindow(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

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

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

std::shared_ptr<Cherry::AppWindow> &ContentBrowserAddWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<ContentBrowserAddWindow>
ContentBrowserAddWindow::Create(const std::string &name) {
  auto instance = std::shared_ptr<ContentBrowserAddWindow>(
      new ContentBrowserAddWindow(name));
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
  float header_width = window_width - 46.0f;
  float image_height = window_width / 4.720f;

  CherryGUI::Image(
      Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_add.png")),
      ImVec2(window_width, image_height));

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

  CherryStyle::AddMarginX(8.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Files",
      Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),
      [this]() {
        float window_width = CherryGUI::GetWindowSize().x;
        float header_width = window_width - 17.0f;
        // Little creation block
        CherryStyle::AddMarginX(8.0f);
        CherryNextComponent.SetProperty("size_x", header_width);
        CherryKit::BlockHorizontalCustom(
            [header_width]() {}, header_width, 25.0f,
            {
                [header_width]() {
                  std::cout << "header_width " << std::to_string(header_width)
                            << std::endl;
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

  CherryStyle::AddMarginX(8.0f);
  CherryNextComponent.SetProperty("size_x", header_width);
  CherryNextComponent.SetProperty("size_y", 4.0f);
  CherryNextComponent.SetProperty("color_border", "#343434");
  CherryNextComponent.SetProperty("color_bg", "#232323");
  CherryNextComponent.SetProperty("color_bg_hovered", "#343434");
  CherryNextComponent.SetProperty("color_bg_clicked", "#454545");
  CherryKit::HeaderImageText(
      "Text editor",
      Cherry::Application::CookPath("resources/imgs/icons/misc/icon_star.png"),
      [this]() {});
}

} // namespace VortexEditor
