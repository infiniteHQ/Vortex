#include "./credits.hpp"

namespace VortexEditor {
Credits::Credits(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [this]() {
    m_AppWindow->SetVisibility(false);
  };
  m_AppWindow->SetLeftMenubarCallback([this]() {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("disable_callback", "true");
    if (CherryKit::ButtonImageTextDropdown(
            "View", Cherry::GetPath("resources/imgs/icons/misc/icon_eye.png"))
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
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);          // TODO : Props
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f); // TODO : Props
    ImVec4 darkBackgroundColor =
        ImVec4(0.15f, 0.15f, 0.15f, 1.0f);                    // TODO : Props
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // TODO : Props

    CherryGUI::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
    CherryGUI::PushStyleColor(ImGuiCol_Border, lightBorderColor);
    CherryGUI::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

    if (CherryGUI::BeginPopup("ViewMenuPopup")) {
      CherryKit::SeparatorText("Pannels");
      CherryKit::CheckboxText("Auto scroll", &m_AutoScroll);

      int default_index = 0;

      switch (
          CherryNextComponent.SetProperty("size_x", 150.0f);
          CherryKit::ComboImageText(
              "",
              {{"All", Cherry::GetPath(
                           "resources/imgs/icons/misc/icon_thumbnails.png")},
               {"Modules",
                Cherry::GetPath("resources/imgs/icons/misc/icon_lines.png")},
               {"Objects",
                Cherry::GetPath("resources/imgs/icons/misc/icon_objects.png")}},
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
  });

  m_AppWindow->SetInternalPaddingX(8.0f);
  m_AppWindow->SetInternalPaddingY(8.0f);

#if defined(__linux__)
  system_desktop = " - " + VortexMaker::GetLinuxDesktopEnvAndDisplayServer();
#endif

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

std::shared_ptr<Cherry::AppWindow> &Credits::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<Credits> Credits::Create(const std::string &name) {
  auto instance = std::shared_ptr<Credits>(new Credits(name));
  instance->SetupRenderCallback();
  return instance;
}

void Credits::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void Credits::Render() {
  float window_width = CherryGUI::GetWindowSize().x;
  float image_height = window_width / 3.435f;

  CherryGUI::Image(
      Cherry::GetTexture(Cherry::GetPath("resources/imgs/vortex_banner.png")),
      ImVec2(window_width, image_height));

  CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding,
                          ImVec2(2, 2)); // CherryStyle::Padding
  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding,
                          ImVec2(2, 2)); // CherryStyle::Padding
  CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#00000000"));
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
  CherryStyle::AddMarginX(20.0f);
  CherryStyle::AddMarginY(5.0f);
  CherryGUI::BeginChild("aboutchild", ImVec2(430, 0)); // cherry api

  float maxScroll = ImGui::GetScrollMaxY();
  float dt = ImGui::GetIO().DeltaTime;

  if (m_AutoScroll) {
    if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0.0f) {
      m_UserScrolled = true;
      m_UserScrollCooldown = USER_SCROLL_PAUSE;
    }

    if (m_UserScrolled) {
      m_UserScrollCooldown -= dt;
      if (m_UserScrollCooldown <= 0.0f) {
        m_UserScrolled = false;
        m_ScrollY = ImGui::GetScrollY();
      }
    } else {
      m_ScrollY += m_ScrollSpeed * dt;

      if (m_ScrollY >= maxScroll)
        m_ScrollY = 0.0f;

      ImGui::SetScrollY(m_ScrollY);
    }
  }

  CherryStyle::AddMarginY(20.0f);
  std::string vortex_launcher_version = VORTEX_VERSION;
  Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
  CherryKit::TitleTwo("Vortex Editor (" + vortex_launcher_version + ")");

  Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
  CherryKit::TitleSix("Core");
  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Name Name");
  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Name Name");

  CherryStyle::AddMarginY(20.0f);
  Cherry::SetNextComponentProperty("color_text", "#FFFFFF");
  CherryKit::TitleThree("Example module");

  Cherry::SetNextComponentProperty("color_text", "#BBBBBB");
  CherryKit::TextSimple("Area");
  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Name Name");
  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Name Name");

  CherryGUI::EndChild();

  CherryGUI::PopStyleColor(2);
  CherryGUI::PopStyleVar(2); // CherryStyle::Padding
}

} // namespace VortexEditor
