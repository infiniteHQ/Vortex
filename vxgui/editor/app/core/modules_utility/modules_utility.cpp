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

using namespace Cherry;

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
      Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetInternalPaddingX(0.0f);
  m_AppWindow->SetInternalPaddingY(0.0f);

  m_AppWindow->SetLeftMenubarCallback([this]() {
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);

    if (m_SelectedPannel != Pannels::Installed) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    }
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");

    if (CherryKit::ButtonImageText(
            "Installed", GetPath("resources/imgs/icons/misc/icon_download.png"))
            .GetDataAs<bool>("isClicked")) {
      m_SelectedPannel = Pannels::Installed;
    }

    if (m_SelectedPannel != Pannels::Downloads) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    }
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    if (CherryKit::ButtonImageText(
            "Download", GetPath("resources/imgs/icons/misc/icon_import.png"))
            .GetDataAs<bool>("isClicked")) {
      m_SelectedPannel = Pannels::Downloads;
    }

    if (m_SelectedPannel != Pannels::Import) {
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
      CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
    }
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    if (CherryKit::ButtonImageText(
            "Import", GetPath("resources/imgs/icons/misc/icon_import.png"))
            .GetDataAs<bool>("isClicked")) {
      m_SelectedPannel = Pannels::Import;
    }
  });

  m_AppWindow->SetRightMenubarCallback([this]() {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("disable_callback", "true");
    if (CherryKit::ButtonImageTextDropdown(
            "Settings", GetPath("resources/imgs/icons/misc/icon_settings.png"))
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
      CherryKit::SeparatorText("View mode");

      int default_index = 0;

      if (m_SelectedShowMode == ShowModes::Thumbmails) {
        default_index = 0;
      } else if (m_SelectedShowMode == ShowModes::List) {
        default_index = 1;
      }

      switch (
          CherryNextComponent.SetProperty("size_x", 150.0f);
          CherryKit::ComboImageText(
              "",
              {
                  {"Thumbnails",
                   GetPath("resources/imgs/icons/misc/icon_thumbnails.png")},
                  {"List", GetPath("resources/imgs/icons/misc/icon_lines.png")},
              },
              default_index)
              .GetPropertyAs<int>("selected")) {
      case 0: {
        m_SelectedShowMode = ShowModes::Thumbmails;
        break;
      }
      case 1: {
        m_SelectedShowMode = ShowModes::List;
        break;
      }
      default: {
        m_SelectedShowMode = ShowModes::Thumbmails;
        break;
      }
      }

      CherryGUI::EndPopup();
    }

    CherryGUI::PopStyleVar();
    CherryGUI::PopStyleColor(2);
  });

  SetSelectedCategory("all");
  m_SelectedShowMode = ShowModes::Thumbmails;
  RefreshCategories();

  m_SelectedChildName = "main";

  this->AddChild(ModulesUtilityChild(
      "main",
      [this]() {
        CherryGUI::SameLine();

        static std::string ModulesSearch;

        if (true) {
          CherryNextComponent.SetProperty("size_x", "240");
          CherryNextComponent.SetProperty("padding_y", "6.0f");
          CherryNextComponent.SetProperty("description", "Search module...");
          CherryNextComponent.SetProperty(
              "description_logo",
              Cherry::GetPath(
                  "resources/imgs/icons/misc/icon_magnifying_glass.png"));
          CherryNextComponent.SetProperty("description_logo_place", "r");
          CherryKit::InputString("", &ModulesSearch);

          CherryNextProp("color", "#252525");
          CherryKit::Separator();
        }
        CherryGUI::Spacing();

        if (VortexMaker::GetCurrentContext()->IO.em.empty()) {
          CherryKit::TitleFour("No modules founded.");
        }

        std::vector<Cherry::Component> modules_blocks;
        if (m_SelectedShowMode == ShowModes::Thumbmails) {
          for (int i = 0; i < VortexMaker::GetCurrentContext()->IO.em.size();
               i++) {

            if (!VortexMaker::GetCurrentContext()->IO.em[i]) {
              continue;
            }

            if (!m_SelectedCategory.empty() && m_SelectedCategory != "all") {
              if (m_SelectedCategory !=
                  VortexMaker::GetCurrentContext()->IO.em[i]->m_group) {
                continue;
              }
            }
            static std::string LatestChange;
            if (LatestChange != ModulesSearch) {
              LatestChange = ModulesSearch;
              m_SelectedCategoryChanged = true;
            }

            if (!ModulesSearch.empty()) {
              if (!HasCommonSubsequence(
                      VortexMaker::GetCurrentContext()->IO.em[i]->m_name,
                      ModulesSearch) &&
                  !HasCommonSubsequence(
                      VortexMaker::GetCurrentContext()->IO.em[i]->m_proper_name,
                      ModulesSearch) &&
                  !HasCommonSubsequence(
                      VortexMaker::GetCurrentContext()->IO.em[i]->m_description,
                      ModulesSearch)) {
                continue;
              }
            }

            CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
            auto item = ModuleCard(
                VortexMaker::GetCurrentContext()->IO.em[i],
                VortexMaker::GetCurrentContext()->IO.em[i]->m_proper_name,
                VortexMaker::GetCurrentContext()->IO.em[i]->m_path,
                VortexMaker::GetCurrentContext()->IO.em[i]->m_name,
                VortexMaker::GetCurrentContext()->IO.em[i]->m_version, false,
                VortexMaker::GetCurrentContext()->IO.em[i]->m_logo_path,
                IM_COL32(56, 56, 56, 150), IM_COL32(50, 50, 50, 255),
                Cherry::HexToImU32("#B1FF31FF"), 100.0f, 5.0f);

            modules_blocks.push_back(item);
          }
          if (!m_SelectedCategoryChanged) {
            CherryKit::GridSimple(250.0f, 250.0f, modules_blocks);
          } else {
            m_SelectedCategoryChanged = false;
          }
        } else if (m_SelectedShowMode == ShowModes::List) {
        }
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")));
  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
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
  RenderModuleDeletionModal();
  if (m_SelectedPannel == Pannels::Installed) {
    RenderInstalled();
  } else if (m_SelectedPannel == Pannels::Import) {
    RenderImport();
  } else if (m_SelectedPannel == Pannels::Downloads) {
    RenderDownload();
  }
}

void ModulesUtility::RenderModuleDeletionModal() {
  if (g_TriggerModuleDeletionModal) {
    ImGui::OpenPopup("##delete_module_modal");
    g_TriggerModuleDeletionModal = false;
  }

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(520, 0), ImGuiCond_Appearing);
  ImGui::SetNextWindowBgAlpha(1.0f);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
  ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.11f, 0.11f, 0.12f, 1.00f));
  ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg,
                        ImVec4(0.00f, 0.00f, 0.00f, 0.65f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.22f, 0.24f, 1.00f));

  bool open = true;
  if (ImGui::BeginPopupModal("##delete_module_modal", &open,
                             ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoTitleBar)) {
    const float MODAL_W = 520.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.62f, 0.10f, 0.10f, 1.0f));
    ImGui::BeginChild("##topbar", ImVec2(MODAL_W, 48.0f), false,
                      ImGuiWindowFlags_NoScrollbar);
    {
      ImGui::SetCursorPos(ImVec2(16.0f, 13.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      ImGui::TextUnformatted("Delete module");
      ImGui::PopStyleColor();
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SetCursorPosX(0.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::BeginChild("##body", ImVec2(MODAL_W, 200.0f), false,
                      ImGuiWindowFlags_NoScrollbar |
                          ImGuiWindowFlags_AlwaysAutoResize);
    {
      ImGui::SetCursorPos(ImVec2(20.0f, 16.0f));

      ImGui::PushStyleColor(ImGuiCol_ChildBg,
                            ImVec4(0.16f, 0.16f, 0.18f, 1.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
      ImGui::SetCursorPosX(20.0f);
      ImGui::BeginChild("##modulecard", ImVec2(MODAL_W - 40.0f, 82.0f), false,
                        ImGuiWindowFlags_NoScrollbar);
      {
        const float LOGO_SIZE = 52.0f;
        ImGui::SetCursorPos(ImVec2(14.0f, 15.0f));

        ImTextureID logo = Cherry::GetTexture(g_ModuleToDeleteLogoPath);
        if (logo) {
          ImVec2 logoSize = Cherry::GetTextureSize(g_ModuleToDeleteLogoPath);
          float scale = LOGO_SIZE / std::max(logoSize.x, logoSize.y);
          ImVec2 drawSize(logoSize.x * scale, logoSize.y * scale);
          ImVec2 imgPos = ImGui::GetCursorPos();
          imgPos.x += (LOGO_SIZE - drawSize.x) * 0.5f;
          imgPos.y += (LOGO_SIZE - drawSize.y) * 0.5f;
          ImGui::SetCursorPos(imgPos);
          ImGui::Image(logo, drawSize);
        } else {
          ImVec2 p = ImGui::GetCursorScreenPos();
          ImDrawList *dl = ImGui::GetWindowDrawList();
          dl->AddRectFilled(p, ImVec2(p.x + LOGO_SIZE, p.y + LOGO_SIZE),
                            IM_COL32(80, 40, 40, 255), 8.0f);
          std::string initials = g_ModuleToDeleteProperName.size() >= 2
                                     ? g_ModuleToDeleteProperName.substr(0, 2)
                                     : "??";
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));
          ImVec2 ts = ImGui::CalcTextSize(initials.c_str());
          ImGui::SetCursorScreenPos(ImVec2(p.x + (LOGO_SIZE - ts.x) * 0.5f,
                                           p.y + (LOGO_SIZE - ts.y) * 0.5f));
          ImGui::TextUnformatted(initials.c_str());
          ImGui::PopStyleColor();
          ImGui::SetCursorPos(ImVec2(14.0f + LOGO_SIZE + 14.0f, 15.0f));
        }

        float textX = 14.0f + LOGO_SIZE + 14.0f;
        ImGui::SetCursorPos(ImVec2(textX, 15.0f));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.96f, 1.0f));
        ImGui::TextUnformatted(g_ModuleToDeleteProperName.c_str());
        ImGui::PopStyleColor();

        ImGui::SetCursorPos(ImVec2(textX, 36.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.58f, 1.0f));
        ImGui::Text("v%s", g_ModuleToDeleteVersion.c_str());
        ImGui::PopStyleColor();

        if (!g_ModuleToDeleteDescription.empty()) {
          ImGui::SetCursorPos(ImVec2(textX, 54.0f));
          ImGui::PushStyleColor(ImGuiCol_Text,
                                ImVec4(0.45f, 0.45f, 0.48f, 1.0f));
          // Clamp to single line with ellipsis
          std::string desc = g_ModuleToDeleteDescription;
          const float maxW = MODAL_W - 40.0f - textX - 14.0f;
          while (!desc.empty() &&
                 ImGui::CalcTextSize((desc + "...").c_str()).x > maxW)
            desc.pop_back();
          if (desc.size() < g_ModuleToDeleteDescription.size())
            desc += "...";
          ImGui::TextUnformatted(desc.c_str());
          ImGui::PopStyleColor();
        }
      }
      ImGui::EndChild();
      ImGui::PopStyleVar();
      ImGui::PopStyleColor();

      ImGui::SetCursorPos(ImVec2(20.0f, ImGui::GetCursorPosY() + 8.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.62f, 0.62f, 0.65f, 1.0f));
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + MODAL_W - 40.0f);
      ImGui::TextUnformatted("This action is permanent. The module files "
                             "will be removed from disk "
                             "and cannot be recovered.");
      ImGui::PopTextWrapPos();
      ImGui::PopStyleColor();

      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 14.0f);
      ImDrawList *dl = ImGui::GetWindowDrawList();
      ImVec2 sepA = ImGui::GetCursorScreenPos();
      dl->AddLine(sepA, ImVec2(sepA.x + MODAL_W, sepA.y),
                  IM_COL32(50, 50, 55, 255), 1.0f);
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);

      const float BTN_H = 36.0f;
      const float BTN_W = 140.0f;
      const float PAD = 16.0f;

      ImGui::SetCursorPos(ImVec2(MODAL_W - (BTN_W * 2.0f + 8.0f + PAD),
                                 ImGui::GetCursorPosY() + 12.0f));

      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            ImVec4(0.26f, 0.26f, 0.28f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            ImVec4(0.17f, 0.17f, 0.19f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.78f, 0.78f, 0.80f, 1.0f));

      if (ImGui::Button("Cancel", ImVec2(BTN_W, BTN_H))) {
        g_ModuleToDeleteName.clear();
        g_ModuleToDeleteProperName.clear();
        g_ModuleToDeleteDescription.clear();
        g_ModuleToDeleteVersion.clear();
        g_ModuleToDeleteLogoPath.clear();
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor(4);

      ImGui::SameLine(0.0f, 8.0f);

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.62f, 0.10f, 0.10f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            ImVec4(0.75f, 0.14f, 0.14f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            ImVec4(0.50f, 0.08f, 0.08f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

      if (ImGui::Button("Delete module", ImVec2(BTN_W, BTN_H))) {
        VortexMaker::DeleteProjectModule(g_ModuleToDeleteName,
                                         g_ModuleToDeleteVersion);
        VortexMaker::LoadEditorModules(
            VortexMaker::GetCurrentContext()->projectPath.string(),
            VortexMaker::GetCurrentContext()->IO.em_handles,
            VortexMaker::GetCurrentContext()->IO.em);
        g_ModuleToDeleteName.clear();
        g_ModuleToDeleteProperName.clear();
        g_ModuleToDeleteDescription.clear();
        g_ModuleToDeleteVersion.clear();
        g_ModuleToDeleteLogoPath.clear();
        m_SelectedCategoryChanged = true;
        RefreshCategories();
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar();

      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 14.0f);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::EndPopup();
  }

  ImGui::PopStyleColor(3);
  ImGui::PopStyleVar(3);
}

void ModulesUtility::RenderDownload() {}

void ModulesUtility::RenderImport() {}

void ModulesUtility::RenderInstalled() {
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

  Cherry::PushFont("Clash");
  CherryStyle::RemoveMarginY(40.0f);
  CherryStyle::AddMarginX(15.0f);
  CherryStyle::PushFontSize(0.28f);
  CherryKit::TitleOne("Modules");
  CherryStyle::PopFontSize();
  Cherry::PopFont();

  // CherryStyle::SetPadding(7.0f);

  const float input_width = leftPaneWidth - 17.0f;
  const float header_width = leftPaneWidth - 27.0f;

  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 7));

  CherryKit::Space(3.0f);
  CherryStyle::AddMarginX(6.0f);
  if (m_SelectedCategory == "all") {
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#787878"));
    CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#454545"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                              Cherry::HexToRGBA("#565656"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                              Cherry::HexToRGBA("#454545"));
  } else {
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                              Cherry::HexToRGBA("#343434"));
    CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                              Cherry::HexToRGBA("#454545"));
  }

  std::string header_label =
      "All Modules (" +
      std::to_string(VortexMaker::GetCurrentContext()->IO.em.size()) + ")";
  if (CherryGUI::ImageSizeButtonWithText(
          Cherry::GetTexture(
              Cherry::GetPath("resources/imgs/icons/misc/icon_stack.png")),
          header_width, header_label.c_str(), ImVec2(-FLT_MIN, 0.0f),
          ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0),
          ImVec4(1, 1, 1, 1))) {
    SetSelectedCategory("all");
  }

  CherryGUI::PopStyleColor(4);

  CherryKit::Space(5.0f);

  CherryStyle::AddMarginX(6.0f);
  CherryKit::SeparatorText("All types");

  static std::string SearchModulesString;
  CherryStyle::AddMarginX(6.0f);
  CherryNextComponent.SetProperty("size_x", input_width);
  CherryNextComponent.SetProperty("padding_y", "6.0f");
  CherryNextComponent.SetProperty("description", "Search categories...");
  CherryNextComponent.SetProperty(
      "description_logo",
      Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
  CherryNextComponent.SetProperty("description_logo_place", "r");
  CherryKit::InputString("", &SearchModulesString);

  CherryGUI::BeginChild("####ModulesCategoryLists");
  for (const auto &[category, count] : m_AllCategories) {

    if (!SearchModulesString.empty()) {
      if (!HasCommonSubsequence(category, SearchModulesString)) {
        continue;
      }
    }

    std::string label = category + " (" + std::to_string(count) + ")";
    CherryStyle::AddMarginX(6.0f);
    if (category == m_SelectedCategory) {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#787878"));
      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#454545"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                                Cherry::HexToRGBA("#565656"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                                Cherry::HexToRGBA("#454545"));
    } else {
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#343434"));
      CherryGUI::PushStyleColor(ImGuiCol_Button, Cherry::HexToRGBA("#232323"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonHovered,
                                Cherry::HexToRGBA("#343434"));
      CherryGUI::PushStyleColor(ImGuiCol_ButtonActive,
                                Cherry::HexToRGBA("#454545"));
    }
    if (CherryGUI::ImageSizeButtonWithText(
            Cherry::GetTexture(Cherry::GetPath(
                "resources/imgs/icons/misc/icon_white_brick.png")),
            header_width, label.c_str(), ImVec2(-FLT_MIN, 0.0f), ImVec2(0, 0),
            ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) {
      SetSelectedCategory(category);
    }
    CherryGUI::PopStyleColor(4);
  }
  CherryGUI::EndChild();

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
