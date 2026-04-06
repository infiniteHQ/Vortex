#include "./credits.hpp"

namespace VortexEditor {
Credits::Credits(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };
  m_AppWindow->SetLeftMenubarCallback(
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      CherryNextComponent.SetProperty("disable_callback", "true");
      if (CherryKit::ButtonImageTextDropdown(
              "View", GetPath("resources/imgs/icons/misc/icon_eye.png"))
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
      } ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // TODO : Props
      ImVec4 graySeparatorColor =
          ImVec4(0.4f, 0.4f, 0.4f, 0.5f); // TODO : Props
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

        if (m_ShowMode == ContentShowMode::Thumbmails) {
          default_index = 0;
        } else if (m_ShowMode == ContentShowMode::List) {
          default_index = 1;
        } else if (m_ShowMode == ContentShowMode::Objects) {
          default_index = 2;
        }

        switch (
            CherryNextComponent.SetProperty("size_x", 150.0f);
            CherryKit::ComboImageText(
                "",
                {{"Thumbnails",
                  GetPath("resources/imgs/icons/misc/icon_thumbnails.png")},
                 {"List", GetPath("resources/imgs/icons/misc/icon_lines.png")},
                 {"Objects",
                  GetPath("resources/imgs/icons/misc/icon_objects.png")}},
                default_index)
                .GetPropertyAs<int>("selected")) {
        case 0: {
          m_ShowMode = ContentShowMode::Thumbmails;
          break;
        }
        case 1: {
          m_ShowMode = ContentShowMode::List;
          break;
        }
        case 2: {
          m_ShowMode = ContentShowMode::Objects;
          break;
        }
        default: {
          m_ShowMode = ContentShowMode::Thumbmails;
          break;
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
  CherryKit::TextSimple(VortexMaker::GetVortexBuildName());

  CherryKit::Space(12.0f);

  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Cherry version: " + cherry_version);
  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Build: " + VortexMaker::GetVortexBuildID() + " ; " +
                        VortexMaker::GetBuildDate() + " (" +
                        VortexMaker::GetVortexEditorDist() + ")");
  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("Hash: "
                        " exe(" +
                        VortexMaker::GetVortexEditorHash() + ") git(" +
                        VortexMaker::GetGitCommit() + ")");

  Cherry::SetNextComponentProperty("color_text", "#878787");
  CherryKit::TextSimple("System: " + os_name + " " + os_arch + system_desktop);

  CherryGUI::EndChild();
  CherryGUI::SameLine();

  CherryGUI::BeginChild("link"); // cherry api

  CherryNextProp("color_bg", "#00000000");
  CherryNextProp("color_border", "#00000000");
  CherryNextProp("padding_x", "2");
  CherryNextProp("padding_y", "2");
  CherryNextProp("size_x", "20");
  CherryNextProp("size_y", "20");

  if (CherryKit::ButtonImageTextImage(
          "Support Us",
          Cherry::GetPath("resources/imgs/icons/launcher/heart.png"),
          Cherry::GetPath("resources/imgs/weblink.png"))
          .GetData("isClicked") == "true") {
    VortexMaker::OpenURL("https://fund.infinite.si/");
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
    VortexMaker::OpenURL(
        "https://github.com/infiniteHQ/Vortex/graphs/contributors");
  }

  CherryNextProp("color_bg", "#00000000");
  CherryNextProp("color_border", "#00000000");
  CherryNextProp("padding_x", "2");
  CherryNextProp("padding_y", "2");
  CherryNextProp("size_x", "20");
  CherryNextProp("size_y", "20");
  if (CherryKit::ButtonImageTextImage(
          "Learn and Documentation",
          Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
          Cherry::GetPath("resources/imgs/weblink.png"))
          .GetData("isClicked") == "true") {
    VortexMaker::OpenURL("https://vortex.infinite.si/learn");
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
  CherryGUI::PopStyleVar(2); // CherryStyle::Padding
}

} // namespace VortexEditor
