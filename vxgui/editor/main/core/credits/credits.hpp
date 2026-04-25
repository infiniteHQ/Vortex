#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef CREDITS_WINDOW_H
#define CREDITS_WINDOW_H

#define CHERRY_V1
#include "../../../../../lib/cherry/cherry.hpp"

namespace vxe {
  // This window can be a "subappwindow" of a parent if you use the constructor
  // with parent parameter.
  class Credits : public std::enable_shared_from_this<Credits> {
   public:
    Credits(const std::string &name);

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<Credits> Create(const std::string &name);
    void SetupRenderCallback();
    void Render();

    std::string m_SelectedCategory;

    bool m_SelectedCategoryChanged = false;
    bool m_AutoScroll = true;
    float m_ScrollY = 0.0f;
    float m_ScrollSpeed = 50.0f;
    bool m_UserScrolled = false;
    float m_UserScrollCooldown = 0.0f;
    const float USER_SCROLL_PAUSE = 3.0f;

    void SetSelectedCategory(const std::string &c) {
      m_SelectedCategory = c;
      m_SelectedCategoryChanged = true;
    }
    std::string GetSelectedCategory() {
      return m_SelectedCategory;
    }

    std::unordered_map<std::string, int> m_AllCategories;

    void RefreshCategories() {
      m_AllCategories.clear();
      for (int i = 0; i < vxe::GetCurrentContext()->IO.em.size(); i++) {
        if (!vxe::GetCurrentContext()->IO.em[i]) {
          continue;
        }
        m_AllCategories[vxe::GetCurrentContext()->IO.em[i]->m_group]++;
      }
    }

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // CREDITS_WINDOW_H