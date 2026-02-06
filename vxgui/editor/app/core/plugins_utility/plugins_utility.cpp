#include "./plugins_utility.hpp"
#include "../../../../../lib/cherry/cherry.hpp"

#include <cstdlib> // std::system
#include <cstring>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

namespace VortexEditor {

void PluginsUtility::PluginsRender() {
  // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
  Cherry::PushFont("ClashBold");
  CherryNextComponent.SetProperty("color_text", "#797979");
  CherryKit::TitleOne("QSf");
  Cherry::PopFont();
  CherryNextComponent.SetProperty("color", "#252525");
  CherryKit::Separator();
}

PluginsUtility::PluginsUtility(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetInternalPaddingX(0.0f);
  m_AppWindow->SetInternalPaddingY(0.0f);

  m_SelectedChildName = "main";

  this->AddChild(PluginsUtilityChild(
      "main",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleThree("Project installed modules");
        Cherry::PopFont();
        CherryGUI::SameLine();
        CherryKit::TooltipTextCustom("(?)", []() {
          CherryKit::TitleFour("em : Editor Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
          CherryStyle::AddMarginY(10.0f);
          CherryKit::TitleFour("esm : Editor Script Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
        });
        CherryNextProp("color", "#252525");
        CherryKit::Separator();
        static std::string ModulesSearch;

        if (true) {
          CherryNextComponent.SetProperty("size_x", "240");
          CherryNextComponent.SetProperty("padding_y", "6.0f");
          CherryNextComponent.SetProperty("description", "Search content...");
          CherryNextComponent.SetProperty(
              "description_logo",
              Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_magnifying_glass.png"));
          CherryNextComponent.SetProperty("description_logo_place", "r");
          CherryKit::InputString("", &ModulesSearch);

          CherryKit::Separator();
        }
        CherryGUI::Spacing();

        if (VortexMaker::GetCurrentContext()->IO.ep.empty()) {
          CherryKit::TitleFour("No plugins founded.");
        }

        std::vector<Cherry::Component> plugins_blocks;
        for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.ep.size();
             i++) {
          CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
          auto item = PluginCard(
              VortexMaker::GetCurrentContext()->IO.ep[i],
              VortexMaker::GetCurrentContext()->IO.ep[i]->m_proper_name,
              VortexMaker::GetCurrentContext()->IO.ep[i]->m_path,
              VortexMaker::GetCurrentContext()->IO.ep[i]->m_name,
              VortexMaker::GetCurrentContext()->IO.ep[i]->m_version, false,
              VortexMaker::GetCurrentContext()->IO.ep[i]->m_logo_path,
              IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
              Cherry::HexToImU32("#B1FF31FF"), 100.0f, 5.0f);

          plugins_blocks.push_back(item);
        }

        CherryKit::GridSimple(CherryID("plugins_grid"), 250.0f, 250.0f,
                              plugins_blocks);
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")));

  this->AddChild(PluginsUtilityChild(
      "Add modules",
      [this]() {

      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")));

  this->AddChild(PluginsUtilityChild(
      "Help",
      [this]() {
        if (CherryKit::ButtonImageTextImage(
                "Learn and Documentation",
                Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
                Cherry::GetPath("resources/imgs/weblink.png"))
                .GetData("isClicked") == "true") {
          // VortexMaker::OpenURL("https://vortex.infinite.si/learn");
        }
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_help.png")));
  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

std::vector<std::shared_ptr<EnvProject>> PluginsUtility::GetMostRecentProjects(
    const std::vector<std::shared_ptr<EnvProject>> &projects, size_t maxCount) {
  auto sortedProjects = projects;
  std::sort(sortedProjects.begin(), sortedProjects.end(),
            [](const std::shared_ptr<EnvProject> &a,
               const std::shared_ptr<EnvProject> &b) {
              return a->lastOpened > b->lastOpened;
            });

  if (sortedProjects.size() > maxCount) {
    sortedProjects.resize(maxCount);
  }
  return sortedProjects;
}

void PluginsUtility::AddChild(const PluginsUtilityChild &child) {
  m_Childs.push_back(child);
}

void PluginsUtility::RemoveChild(const std::string &child_name) {
  auto it = std::find_if(
      m_Childs.begin(), m_Childs.end(),
      [&child_name](const auto &child) { return child.Name == child_name; });
  if (it != m_Childs.end()) {
    m_Childs.erase(it);
  }
}

std::shared_ptr<Cherry::AppWindow> &PluginsUtility::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<PluginsUtility>
PluginsUtility::Create(const std::string &name) {
  auto instance = std::shared_ptr<PluginsUtility>(new PluginsUtility(name));
  instance->SetupRenderCallback();
  return instance;
}

void PluginsUtility::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

PluginsUtilityChild *PluginsUtility::GetChild(const std::string &child_name) {
  auto it = std::find_if(m_Childs.begin(), m_Childs.end(),
                         [&child_name](const PluginsUtilityChild &child) {
                           return child.Name == child_name;
                         });
  if (it != m_Childs.end()) {
    return &(*it);
  }
  return nullptr;
}

void PluginsUtility::Render() {
  const float minPaneWidth = 50.0f;
  const float splitterWidth = 1.5f;

  std::string label = "left_pane" + m_AppWindow->m_Name;
  CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
  CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
  CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, 0);

  CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
  CherryGUI::Image(
      Cherry::GetTexture(Cherry::GetPath("resources/imgs/banner_modules.png")),
      ImVec2(280, 142));

  // CherryStyle::SetPadding(7.0f);

  const float input_width = leftPaneWidth - 17.0f;
  const float header_width = leftPaneWidth - 27.0f;

  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));

  CherryKit::Space(3.0f);
  CherryStyle::AddMarginX(6.0f);
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                            Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                            Cherry::HexToRGBA("#454545"));

  std::string header_label =
      "All Modules (" +
      std::to_string(VortexMaker::GetCurrentContext()->IO.ep.size()) + ")";
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(
              Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
          header_width, header_label.c_str(), ImVec2(-FLT_MIN, 0.0f),
          ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0),
          ImVec4(1, 1, 1, 1))) {
  }

  CherryGUI::PopStyleColor(4);

  CherryKit::Space(5.0f);

  CherryStyle::AddMarginX(6.0f);
  CherryKit::SeparatorText("All types");

  static std::string SearchModulesString;
  CherryStyle::AddMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", input_width);
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  CherryNextComponent.SetProperty("description", "Search types...");
  CherryNextComponent.SetProperty(
      "description_logo",
      Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
  CherryNextComponent.SetProperty("description_logo_place", "r");
  CherryKit::InputString("", &SearchModulesString);

  CherryStyle::AddMarginX(6.0f);
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                            Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                            Cherry::HexToRGBA("#454545"));
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(Cherry::GetPath(
              "resources/imgs/icons/misc/icon_white_brick.png")),
          header_width, "Tools (32)", ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
          ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
  }
  CherryGUI::PopStyleColor(4);

  CherryStyle::AddMarginX(6.0f);
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                            Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                            Cherry::HexToRGBA("#454545"));
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(Cherry::GetPath(
              "resources/imgs/icons/misc/icon_white_brick.png")),
          header_width, "Miscelanous (3)", ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
          ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
  }
  CherryGUI::PopStyleColor(4);

  CherryStyle::AddMarginX(6.0f);
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                            Cherry::HexToRGBA("#343434"));
  CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                            Cherry::HexToRGBA("#454545"));
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(Cherry::GetPath(
              "resources/imgs/icons/misc/icon_white_brick.png")),
          header_width, "Systems (5)", ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
          ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
  }
  CherryGUI::PopStyleColor(4);
  CherryGUI::PopStyleVar();

  /*for (const auto &child : m_Childs) {
    if (child.Name == m_SelectedChildName) {
      // opt.hex_text_idle = "#FFFFFFFF";
    } else {
      // opt.hex_text_idle = "#A9A9A9FF";
    }
    std::string child_name;

    if (child.Name.rfind("?loc:", 0) == 0) {
      std::string localeName = child.Name.substr(5);
      child_name = Cherry::GetLocale(localeName) + "####" + localeName;
    } else {
      child_name = child.Name;
    }

    CherryNextComponent.SetProperty("color_bg", "#00000000");
    CherryNextComponent.SetProperty("color_border", "#00000000");
    CherryNextComponent.SetProperty("padding_x", "2");
    CherryNextComponent.SetProperty("padding_y", "2");
    CherryNextComponent.SetProperty("size_x", "20");
    CherryNextComponent.SetProperty("size_y", "20");
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
    if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(),
                                   child.LogoPath)
            .GetData("isClicked") == "true") {
      m_SelectedChildName = child.Name;
    }
  }
*/
  CherryGUI::EndChild();
  CherryGUI::PopStyleVar(4);
  CherryGUI::PopStyleColor(2);

  CherryGUI::SameLine();
  CherryGUI::BeginGroup();

  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

  if (!m_SelectedChildName.empty()) {
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

    if (CherryGUI::BeginChild("ChildPanel", ImVec2(0, 0), false,
                              ImGuiWindowFlags_NoBackground |
                                  ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
      auto child = GetChild(m_SelectedChildName);

      if (child) {
        std::function<void()> pannel_render = child->RenderCallback;
        if (pannel_render) {
          pannel_render();
        }
      }
    }
    CherryGUI::EndChild();

    CherryGUI::PopStyleVar(2);
  }

  CherryGUI::EndGroup();
}
} // namespace VortexEditor
