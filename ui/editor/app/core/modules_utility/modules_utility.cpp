#include "./modules_utility.hpp"
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

void ModulesUtility::ModulesRender() {
  // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
  Cherry::PushFont("ClashBold");
  CherryNextComponent.SetProperty("color_text", "#797979");
  CherryKit::TitleOne("QSf");
  Cherry::PopFont();
  CherryNextComponent.SetProperty("color", "#252525");
  CherryKit::Separator();
}

ModulesUtility::ModulesUtility(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetInternalPaddingX(0.0f);
  m_AppWindow->SetInternalPaddingY(0.0f);
  m_AppWindow->SetLeftMenubarCallback([this]() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");

    if (CherryKit::ButtonImageText(
            Cherry::GetLocale("loc.window.content.content_browser.add"),
            Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
            .GetDataAs<bool>("isClicked")) {
    }
  });

  m_SelectedChildName = "Project's modules";

  this->AddChild(ModulesUtilityChild(
      "Installed modules",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleThree("Project informations");
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

        for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size();
             i++) {
          ModuleCard(VortexMaker::GetCurrentContext()->IO.em[i],
                     VortexMaker::GetCurrentContext()->IO.em[i]->m_proper_name,
                     VortexMaker::GetCurrentContext()->IO.em[i]->m_path,
                     VortexMaker::GetCurrentContext()->IO.em[i]->m_name,
                     VortexMaker::GetCurrentContext()->IO.em[i]->m_version,
                     false,
                     VortexMaker::GetCurrentContext()->IO.em[i]->m_logo_path,
                     IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                     Cherry::HexToImU32("#B1FF31FF"), 100.0f, 5.0f);
        }
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")));

  this->AddChild(ModulesUtilityChild(
      "Add modules",
      [this]() {

      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_add.png")));

  this->AddChild(ModulesUtilityChild(
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

bool ModulesUtility::ModuleCard(const std::shared_ptr<ModuleInterface> &module,
                                const std::string &name,
                                const std::string &path,
                                const std::string &description,
                                const std::string &size, bool selected,
                                const std::string &logo, ImU32 bgColor,
                                ImU32 borderColor, ImU32 lineColor,
                                float maxTextWidth, float borderRadius) {
  bool pressed = false;

  float logoSize = 60.0f;
  float extraHeight = 80.0f;
  float padding = 10.0f;
  float separatorHeight = 2.0f;
  float textOffsetY = 5.0f;
  float versionBoxWidth = 10.0f;
  float versionBoxHeight = 20.0f;
  float thumbnailIconOffsetY = 30.0f;

  float oldfontsize = ImGui::GetFont()->Scale;
  ImFont *oldFont = ImGui::GetFont();

  if (selected) {
    bgColor = IM_COL32(80, 80, 240, 255);
    borderColor = IM_COL32(150, 150, 255, 255);
  }

  ImVec2 squareSize(logoSize, logoSize);

  const char *originalText = name.c_str();
  std::string truncatedText = name;

  if (ImGui::CalcTextSize(originalText).x > maxTextWidth) {
    truncatedText = name.substr(0, 20);
    if (ImGui::CalcTextSize(truncatedText.c_str()).x > maxTextWidth) {
      truncatedText = name.substr(0, 10) + "\n" + name.substr(10, 10);
    }
  } else {
    truncatedText = name + "\n";
  }

  std::string truncatedDesc = module->m_description;
  if (truncatedDesc.length() > 100) {
    truncatedDesc = truncatedDesc.substr(0, 97) + "...";
  }
  const char *originalDesc = truncatedDesc.c_str();

  if (ImGui::CalcTextSize(originalDesc).x > maxTextWidth) {
    truncatedDesc = module->m_description.substr(0, 90);
    if (ImGui::CalcTextSize(truncatedDesc.c_str()).x > maxTextWidth) {
      truncatedDesc = module->m_description.substr(0, 55) + "\n" +
                      module->m_description.substr(55, 55);
    }
  } else {
    truncatedDesc = module->m_description + "\n";
  }

  ImVec2 fixedSize(maxTextWidth + padding * 2 + 150.0f,
                   logoSize + extraHeight + padding * 2);

  ImVec2 cursorPos = ImGui::GetCursorScreenPos();

  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
  ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
  ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
  ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

  ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);
  ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

  static bool open_deletion_modal = false;

  static bool delete_single_file = false;
  static std::string delete_single_file_path = "";

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(2);

  ImDrawList *drawList = ImGui::GetWindowDrawList();

  drawList->AddRectFilled(
      cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
      bgColor, borderRadius);
  drawList->AddRectFilled(
      cursorPos,
      ImVec2(cursorPos.x + fixedSize.x,
             cursorPos.y + thumbnailIconOffsetY + squareSize.y),
      IM_COL32(26, 26, 26, 255), borderRadius, ImDrawFlags_RoundCornersTop);
  drawList->AddRect(
      cursorPos, ImVec2(cursorPos.x + fixedSize.x, cursorPos.y + fixedSize.y),
      borderColor, borderRadius, 0, 1.0f);

  ImVec2 logoPos = ImVec2(cursorPos.x + (fixedSize.x - squareSize.x) / 2,
                          cursorPos.y + padding);

  ImVec2 sizePos = ImVec2(cursorPos.x + padding, cursorPos.y + squareSize.y +
                                                     thumbnailIconOffsetY - 25 +
                                                     textOffsetY);
  ImGui::SetCursorScreenPos(sizePos);

  ImTextureID logotexture =
      Cherry::Application::GetCurrentRenderedWindow()->get_texture(logo);
  drawList->AddImage(
      logotexture, logoPos,
      ImVec2(logoPos.x + squareSize.x, logoPos.y + squareSize.y));

  ImGui::GetFont()->Scale = 0.7f;
  ImGui::PushFont(ImGui::GetFont());

  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
  ImGui::PushItemWidth(maxTextWidth);
  ImGui::TextWrapped(size.c_str());
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();

  ImGui::GetFont()->Scale = oldfontsize;
  ImGui::PopFont();

  ImVec2 lineStart =
      ImVec2(cursorPos.x, cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                              separatorHeight);
  ImVec2 lineEnd = ImVec2(cursorPos.x + fixedSize.x,
                          cursorPos.y + squareSize.y + thumbnailIconOffsetY +
                              separatorHeight);
  drawList->AddLine(lineStart, lineEnd, lineColor, separatorHeight);

  ImGui::GetFont()->Scale = 0.9f;
  ImGui::PushFont(ImGui::GetFont());

  ImVec2 textPos =
      ImVec2(cursorPos.x + padding,
             cursorPos.y + squareSize.y + thumbnailIconOffsetY + textOffsetY);
  ImGui::SetCursorScreenPos(textPos);
  ImGui::PushItemWidth(maxTextWidth);
  ImU32 textColor = IM_COL32(255, 255, 255, 255);
  ImU32 highlightColor = IM_COL32(255, 255, 0, 255);
  ImU32 highlightTextColor = IM_COL32(0, 0, 0, 255);
  DrawHighlightedText(drawList, textPos, truncatedText.c_str(), ModulesSearch,
                      highlightColor, textColor, highlightTextColor);

  ImGui::PopItemWidth();

  ImGui::GetFont()->Scale = oldfontsize;
  ImGui::PopFont();

  ImVec2 labelPos = ImVec2(cursorPos.x + padding,
                           cursorPos.y + squareSize.y - 20 +
                               thumbnailIconOffsetY + 35 + textOffsetY);
  ImGui::SetCursorScreenPos(labelPos);

  ImGui::GetFont()->Scale = 0.7f;
  ImGui::PushFont(ImGui::GetFont());

  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
  ImGui::PushItemWidth(maxTextWidth);
  ImGui::TextWrapped(description.c_str());
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();

  ImVec2 descriptionPos = ImVec2(cursorPos.x + padding,
                                 cursorPos.y + squareSize.y +
                                     thumbnailIconOffsetY + 35 + textOffsetY);
  ImGui::SetCursorScreenPos(descriptionPos);

  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
  ImGui::PushItemWidth(fixedSize.x);

  DrawDescription(drawList, descriptionPos, truncatedDesc.c_str(),
                  ModulesSearch, highlightColor,
                  Cherry::HexToImU32("#8A8A8AFF"), highlightTextColor,
                  oldfontsize);
  ImGui::PopItemWidth();
  ImGui::PopStyleColor();

  ImGui::GetFont()->Scale = oldfontsize;
  ImGui::PopFont();

  float buttonWidth = 20.0f;
  float buttonHeight = 20.0f;
  float buttonSpacing = 10.0f;

  ImVec2 firstButtonPos = ImVec2(
      cursorPos.x + fixedSize.x - padding - buttonWidth, cursorPos.y + padding);
  ImGui::SetCursorScreenPos(firstButtonPos);

  if (module->m_state == "failed") {

    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_retry.png"))
            .GetPropertyAs<bool>("isClicked")) {
      module->Start();
    }

  } else if (module->m_state == "unknow" || module->m_state == "stopped") {

    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_start.png"))
            .GetPropertyAs<bool>("isClicked")) {
      module->Start();
    }

  } else {
    if (CherryKit::ButtonImage(
            Cherry::GetPath("resources/imgs/icons/misc/icon_stop.png"))
            .GetPropertyAs<bool>("isClicked")) {
      module->Stop();
    }
  }

  ImVec2 secondButtonPos =
      ImVec2(firstButtonPos.x - buttonWidth - buttonSpacing, firstButtonPos.y);
  ImGui::SetCursorScreenPos(secondButtonPos);

  if (CherryKit::ButtonImage(
          Cherry::GetPath("resources/imgs/icons/misc/icon_trash.png"))
          .GetPropertyAs<bool>("isClicked")) {
    module->Stop();
    // TODO VortexMaker::DeleteProjectModule
  }

  ImVec2 thirdButtonPos = ImVec2(
      secondButtonPos.x - buttonWidth - buttonSpacing, secondButtonPos.y);
  ImGui::SetCursorScreenPos(thirdButtonPos);
  if (CherryKit::ButtonImage(
          Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
          .GetPropertyAs<bool>("isClicked")) {
    std::string label = "Details of " + module->m_proper_name + "####" + path;
    /*std::shared_ptr<ModuleDetails> window =
        ModuleDetails::Create(label, module);
    Cherry::AddAppWindow(window->GetAppWindow());*/
  }

  /*

     if (ctx->IO.em[i]->m_state == "failed")
      {
          if (ImGui::ImageButtonWithText(listIcon, "Retry to launch",
     ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
          {
              ctx->IO.em[i]->Start();
          }
          ImGui::SameLine();
          if (ImGui::ImageButtonWithText(trashIcon, "Delete",
     ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
          {
              // Behavior
          }
          ImGui::SameLine();
          if (ImGui::ImageButtonWithText(logIcon, "Details",
     ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
          {
              // Behavior
              std::shared_ptr<ModuleDetails> instance =
     std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
              this->factory->SpawnInstance(instance);
          }
      }
      if (ctx->IO.em[i]->m_state == "unknow" || ctx->IO.em[i]->m_state ==
     "stopped")
      {
          if (ImGui::ImageButtonWithText(startIcon, "Launch",
     ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
          {
              ctx->IO.em[i]->Start();
          }
          ImGui::SameLine();
          if (ImGui::ImageButtonWithText(trashIcon, "Delete",
     ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
          {
              // Behavior
          }
          ImGui::SameLine();
          if (ImGui::ImageButtonWithText(logIcon, "Details",
     ImVec2(this->m_RefreshIcon->GetWidth(), this->m_RefreshIcon->GetHeight())))
          {
              // Behavior
              std::shared_ptr<ModuleDetails> instance =
     std::make_shared<ModuleDetails>(this->ctx, ctx->IO.em[i]);
              this->factory->SpawnInstance(instance);
          }
      }
*/

  float windowVisibleX2 =
      ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
  if (cursorPos.x + fixedSize.x < windowVisibleX2)
    ImGui::SameLine();

  ImGui::SetCursorScreenPos(
      ImVec2(cursorPos.x, cursorPos.y + fixedSize.y + padding));

  ImGui::GetFont()->Scale = oldfontsize;

  return pressed;
}

std::vector<std::shared_ptr<EnvProject>> ModulesUtility::GetMostRecentProjects(
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

void ModulesUtility::AddChild(const ModulesUtilityChild &child) {
  m_Childs.push_back(child);
}

void ModulesUtility::RemoveChild(const std::string &child_name) {
  auto it = std::find_if(
      m_Childs.begin(), m_Childs.end(),
      [&child_name](const auto &child) { return child.Name == child_name; });
  if (it != m_Childs.end()) {
    m_Childs.erase(it);
  }
}

std::shared_ptr<Cherry::AppWindow> &ModulesUtility::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<ModulesUtility>
ModulesUtility::Create(const std::string &name) {
  auto instance = std::shared_ptr<ModulesUtility>(new ModulesUtility(name));
  instance->SetupRenderCallback();
  return instance;
}

void ModulesUtility::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

ModulesUtilityChild *ModulesUtility::GetChild(const std::string &child_name) {
  auto it = std::find_if(m_Childs.begin(), m_Childs.end(),
                         [&child_name](const ModulesUtilityChild &child) {
                           return child.Name == child_name;
                         });
  if (it != m_Childs.end()) {
    return &(*it);
  }
  return nullptr;
}

void ModulesUtility::Render() {
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

  for (const auto &child : m_Childs) {
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
