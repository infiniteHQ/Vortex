//
//  modules_utility.cpp
//  Main sources of logic featurs for the module utility
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./modules_utility.hpp"

#include "../../../../../lib/cherry/cherry.hpp"

namespace vxe {

  ModulesUtility::ModulesUtility(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_bricksearch.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [=]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(0.0f);
    app_window_->SetInternalPaddingY(0.0f);

    app_window_->SetLeftMenubarCallback([this]() { render_left_menubar(); });

    app_window_->SetRightMenubarCallback([this]() { render_right_menubar(); });

    set_selected_category("all");
    selected_show_mode_ = ShowModes::Thumbmails;
    refresh_categories();

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  std::shared_ptr<Cherry::AppWindow> &ModulesUtility::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<ModulesUtility> ModulesUtility::create(const std::string &name) {
    auto instance = std::shared_ptr<ModulesUtility>(new ModulesUtility(name));
    instance->setup_render_callback();
    return instance;
  }

  void ModulesUtility::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }
  void ModulesUtility::refresh_categories() {
    all_categories.clear();
    for (int i = 0; i < vxe::GetCurrentContext()->IO.em.size(); i++) {
      if (!vxe::GetCurrentContext()->IO.em[i]) {
        continue;
      }
      all_categories[vxe::GetCurrentContext()->IO.em[i]->m_group]++;
    }
  }

  bool ModulesUtility::has_common_subsequence(const std::string &a, const std::string &b) {
    int j = 0;
    for (int i = 0; i < a.size() && j < b.size(); i++) {
      if (a[i] == b[j])
        j++;
    }
    return j == b.size();
  }

  void ModulesUtility::set_module_to_delete(const std::shared_ptr<ModuleInterface> &mod) {
    module_to_delete_name_ = mod->m_name;
    module_to_delete_proper_name_ = mod->m_proper_name;
    module_to_delete_description_ = mod->m_description;
    module_to_delete_version_ = mod->m_version;
    module_to_delete_logo_path_ = mod->m_logo_path;
    trigger_module_deletion_modal_ = true;
  }

  void ModulesUtility::set_selected_category(const std::string &c) {
    selected_category_ = c;
    selected_category_changed_ = true;
  }
  std::string ModulesUtility::get_selected_category() {
    return selected_category_;
  }
}  // namespace vxe
