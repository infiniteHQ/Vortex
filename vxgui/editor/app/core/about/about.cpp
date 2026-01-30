#include "./about.hpp"

namespace VortexEditor {
AboutVortex::AboutVortex(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetInternalPaddingX(8.0f);
  m_AppWindow->SetInternalPaddingY(8.0f);

#if defined(__linux__)
  system_desktop = " - " + VortexMaker::GetLinuxDesktopEnvAndDisplayServer();
#endif

  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

std::shared_ptr<Cherry::AppWindow> &AboutVortex::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<AboutVortex> AboutVortex::Create(const std::string &name) {
  auto instance = std::shared_ptr<AboutVortex>(new AboutVortex(name));
  instance->SetupRenderCallback();
  return instance;
}

void AboutVortex::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void AboutVortex::Render() {
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
