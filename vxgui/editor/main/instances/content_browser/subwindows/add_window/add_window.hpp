#pragma once
#include "../../../../../../../vxcore/include/vortex.h"
#include "../../../../../../../vxcore/include/vortex_internals.h"

#ifndef CONTENT_BROWSER_ADD_WINDOW_H
#define CONTENT_BROWSER_ADD_WINDOW_H

#define CHERRY_V1
#include "../../../../../../../lib/cherry/cherry.hpp"

namespace vxe {
  // This window can be a "subappwindow" of a parent if you use the constructor
  // with parent parameter.
  class ContentBrowserAddWindow : public std::enable_shared_from_this<ContentBrowserAddWindow> {
   public:
    ContentBrowserAddWindow(const std::string &name, const std::string &path);

    std::shared_ptr<Cherry::AppWindow> &get_app_window();
    static std::shared_ptr<ContentBrowserAddWindow> create(const std::string &name, const std::string &path);
    void setup_render_callback();
    void render();

    void Setcreate_fileCallback(const std::function<void()> &callback) {
      m_create_fileCallback = callback;
    }

    void Setcreate_folderCallback(const std::function<void()> &callback) {
      m_create_folderCallback = callback;
    }

    void SetImportContentCallback(const std::function<void()> &callback) {
      m_ImportContentCallback = callback;
    }

    std::function<void()> m_create_fileCallback;
    std::function<void()> m_create_folderCallback;
    std::function<void()> m_ImportContentCallback;
    std::string m_CreationPath;

    std::shared_ptr<Cherry::AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // CONTENT_BROWSER_ADD_WINDOW_H