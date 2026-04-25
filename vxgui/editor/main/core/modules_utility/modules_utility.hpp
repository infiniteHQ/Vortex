#pragma once

#include <unordered_set>

#include "../../../../../vxcore/include/modules/delete.h"
#include "../../../../../vxcore/include/modules/load.h"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "../../instances/modules_details/modules_details.hpp"
#include "../utils.hpp"
#include "./ui/module_card_item.hpp"
#include "modules_utility_helpers.hpp"

#ifndef MODULES_UTILITY_WINDOW_H
#define MODULES_UTILITY_WINDOW_H

namespace vxe {

  class ModulesUtility : public std::enable_shared_from_this<ModulesUtility> {
   public:
    ModulesUtility(const std::string &name);

    // window
    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<ModulesUtility> Create(const std::string &name);
    void SetupRenderCallback();
    void AddChild(const ModulesUtilityChild &child);
    void RemoveChild(const std::string &child_name);
    ModulesUtilityChild *GetChild(const std::string &child_name);

    // rendering
    void Render();
    void RenderInstalled();
    void ModulesRender();
    void RenderImport();
    void RenderDownload();
    void RenderModuleDeletionModal();

    // logic
    void RefreshCategories();

    // utils
    bool HasCommonSubsequence(const std::string &a, const std::string &b);

    void setmoduletodelete(const std::shared_ptr<ModuleInterface> &mod) {
      g_ModuleToDeleteName = mod->m_name;
      g_ModuleToDeleteProperName = mod->m_proper_name;
      g_ModuleToDeleteDescription = mod->m_description;
      g_ModuleToDeleteVersion = mod->m_version;
      g_ModuleToDeleteLogoPath = mod->m_logo_path;
      g_TriggerModuleDeletionModal = true;
    }

   private:
    std::vector<ModulesUtilityChild> childs_;

    std::string m_ModulesSearch;
    bool g_TriggerModuleDeletionModal;
    std::string g_ModuleToDeleteName;
    std::string g_ModuleToDeleteProperName;
    std::string g_ModuleToDeleteDescription;
    std::string g_ModuleToDeleteVersion;
    std::string g_ModuleToDeleteLogoPath;

    std::function<void()> m_CreateProjectCallback;
    std::function<void()> m_OpenProjectCallback;
    std::function<void()> m_SettingsCallback;
    std::function<void(const std::shared_ptr<EnvProject> &)> m_ProjectCallback;

    enum class ShowModes { Thumbmails, List };
    enum class Pannels { Installed, Downloads, Import };
    ShowModes m_SelectedShowMode = ShowModes::Thumbmails;

    Pannels m_SelectedPannel = Pannels::Installed;

    char ModulesSearch[512];
    std::vector<std::shared_ptr<EnvProject>> GetMostRecentProjects(
        const std::vector<std::shared_ptr<EnvProject>> &projects,
        size_t maxCount);
    std::vector<std::shared_ptr<EnvProject>> m_RecentProjects;
    std::string m_SelectedChildName;

    std::string m_SelectedCategory;
    bool m_SelectedCategoryChanged = false;
    void SetSelectedCategory(const std::string &c) {
      m_SelectedCategory = c;
      m_SelectedCategoryChanged = true;
    }
    std::string GetSelectedCategory() {
      return m_SelectedCategory;
    }
    std::unordered_map<std::string, int> m_AllCategories;

    std::vector<std::string> vortexDists;
    std::string VortexEditorDist;
    std::string newDist;

    std::shared_ptr<Cherry::AppWindow> app_window_;
    int selected;
    float leftPaneWidth = 290.0f;
  };
}  // namespace vxe
#endif  // MODULEs_UTILITY_WINDOW_H