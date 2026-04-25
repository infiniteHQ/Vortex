#include "./modules_utility.hpp"

#include <cstdlib>  // std::system
#include <cstring>
#include <iostream>
#include <string>

#include "../../../../../lib/cherry/cherry.hpp"

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

namespace vxe {

  ModulesUtility::ModulesUtility(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [=]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(0.0f);
    app_window_->SetInternalPaddingY(0.0f);

    app_window_->SetLeftMenubarCallback([this]() {
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 3.0f);

      if (m_SelectedPannel != Pannels::Installed) {
        CherryNextComponent.SetProperty("color_border", "#00000000");
        CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
        CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      }
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");

      if (CherryKit::ButtonImageText("Installed", GetPath("resources/imgs/icons/misc/icon_folder.png"))
              .GetDataAs<bool>("isClicked")) {
        m_SelectedPannel = Pannels::Installed;
      }

      if (m_SelectedPannel != Pannels::Downloads) {
        CherryNextComponent.SetProperty("color_border", "#00000000");
        CherryNextComponent.SetProperty("color_border_hovered", "#00000000");
        CherryNextComponent.SetProperty("color_border_pressed", "#00000000");
      }
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      if (CherryKit::ButtonImageText("Download", GetPath("resources/imgs/icons/misc/icon_wadd.png"))
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
      if (CherryKit::ButtonImageText("Import", GetPath("resources/imgs/icons/misc/icon_import.png"))
              .GetDataAs<bool>("isClicked")) {
        m_SelectedPannel = Pannels::Import;
      }
    });

    app_window_->SetRightMenubarCallback([this]() {
      CherryNextComponent.SetProperty("padding_y", "6.0f");
      CherryNextComponent.SetProperty("padding_x", "10.0f");
      CherryNextComponent.SetProperty("disable_callback", "true");
      if (CherryKit::ButtonImageTextDropdown("Settings", GetPath("resources/imgs/icons/misc/icon_settings.png"))
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
        CherryKit::SeparatorText("View mode");

        int default_index = 0;

        if (m_SelectedShowMode == ShowModes::Thumbmails) {
          default_index = 0;
        } else if (m_SelectedShowMode == ShowModes::List) {
          default_index = 1;
        }

        switch (CherryNextComponent.SetProperty("size_x", 150.0f);
                CherryKit::ComboImageText(
                    "",
                    {
                        { "Thumbnails", GetPath("resources/imgs/icons/misc/icon_thumbnails.png") },
                        { "List", GetPath("resources/imgs/icons/misc/icon_lines.png") },
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
                "description_logo", Cherry::GetPath("resources/imgs/icons/misc/icon_magnifying_glass.png"));
            CherryNextComponent.SetProperty("description_logo_place", "r");
            CherryKit::InputString("", &ModulesSearch);

            CherryNextProp("color", "#252525");
            CherryKit::Separator();
          }
          CherryGUI::Spacing();

          if (vxe::GetCurrentContext()->IO.em.empty()) {
            CherryKit::TitleFour("No modules founded.");
          }

          std::vector<Cherry::Component> modules_blocks;
          if (m_SelectedShowMode == ShowModes::Thumbmails) {
            for (int i = 0; i < vxe::GetCurrentContext()->IO.em.size(); i++) {
              if (!vxe::GetCurrentContext()->IO.em[i]) {
                continue;
              }

              if (!m_SelectedCategory.empty() && m_SelectedCategory != "all") {
                if (m_SelectedCategory != vxe::GetCurrentContext()->IO.em[i]->m_group) {
                  continue;
                }
              }
              static std::string LatestChange;
              if (LatestChange != ModulesSearch) {
                LatestChange = ModulesSearch;
                m_SelectedCategoryChanged = true;
              }

              if (!ModulesSearch.empty()) {
                if (!HasCommonSubsequence(vxe::GetCurrentContext()->IO.em[i]->m_name, ModulesSearch) &&
                    !HasCommonSubsequence(vxe::GetCurrentContext()->IO.em[i]->m_proper_name, ModulesSearch) &&
                    !HasCommonSubsequence(vxe::GetCurrentContext()->IO.em[i]->m_description, ModulesSearch)) {
                  continue;
                }
              }

              CherryNextComponent.SetRenderMode(Cherry::RenderMode::CreateOnly);
              auto item = ModuleCard(
                  vxe::GetCurrentContext()->IO.em[i],
                  vxe::GetCurrentContext()->IO.em[i]->m_proper_name,
                  vxe::GetCurrentContext()->IO.em[i]->m_path,
                  vxe::GetCurrentContext()->IO.em[i]->m_name,
                  vxe::GetCurrentContext()->IO.em[i]->m_version,
                  false,
                  vxe::GetCurrentContext()->IO.em[i]->m_logo_path,
                  IM_COL32(56, 56, 56, 150),
                  IM_COL32(50, 50, 50, 255),
                  Cherry::HexToImU32("#B1FF31FF"),
                  100.0f,
                  5.0f,
                  [this](const std::shared_ptr<ModuleInterface> &mod) { setmoduletodelete(mod); },
                  &ModulesSearch);

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
    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::vector<std::shared_ptr<EnvProject>> ModulesUtility::GetMostRecentProjects(
      const std::vector<std::shared_ptr<EnvProject>> &projects,
      size_t maxCount) {
    auto sortedProjects = projects;
    std::sort(
        sortedProjects.begin(),
        sortedProjects.end(),
        [](const std::shared_ptr<EnvProject> &a, const std::shared_ptr<EnvProject> &b) {
          return a->lastOpened > b->lastOpened;
        });

    if (sortedProjects.size() > maxCount) {
      sortedProjects.resize(maxCount);
    }
    return sortedProjects;
  }

  void ModulesUtility::AddChild(const ModulesUtilityChild &child) {
    childs_.push_back(child);
  }

  void ModulesUtility::RemoveChild(const std::string &child_name) {
    auto it =
        std::find_if(childs_.begin(), childs_.end(), [&child_name](const auto &child) { return child.Name == child_name; });
    if (it != childs_.end()) {
      childs_.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &ModulesUtility::GetAppWindow() {
    return app_window_;
  }

  std::shared_ptr<ModulesUtility> ModulesUtility::Create(const std::string &name) {
    auto instance = std::shared_ptr<ModulesUtility>(new ModulesUtility(name));
    instance->SetupRenderCallback();
    return instance;
  }

  void ModulesUtility::SetupRenderCallback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->Render();
      }
    });
  }

  ModulesUtilityChild *ModulesUtility::GetChild(const std::string &child_name) {
    auto it = std::find_if(childs_.begin(), childs_.end(), [&child_name](const ModulesUtilityChild &child) {
      return child.Name == child_name;
    });
    if (it != childs_.end()) {
      return &(*it);
    }
    return nullptr;
  }

  void ModulesUtility::RefreshCategories() {
    m_AllCategories.clear();
    for (int i = 0; i < vxe::GetCurrentContext()->IO.em.size(); i++) {
      if (!vxe::GetCurrentContext()->IO.em[i]) {
        continue;
      }
      m_AllCategories[vxe::GetCurrentContext()->IO.em[i]->m_group]++;
    }
  }

  bool ModulesUtility::HasCommonSubsequence(const std::string &a, const std::string &b) {
    int j = 0;
    for (int i = 0; i < a.size() && j < b.size(); i++) {
      if (a[i] == b[j])
        j++;
    }
    return j == b.size();
  }

}  // namespace vxe
