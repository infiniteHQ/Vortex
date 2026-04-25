
#pragma once
#include "../../../../../lib/cherry/cherry.hpp"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#ifndef VORTEX_EDITOR_DOC_VIEWER_APPWINDOW_H
#define VORTEX_EDITOR_DOC_VIEWER_APPWINDOW_H

using namespace Cherry;

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stack>
#include <unordered_set>

namespace vxe {

  class DocViewerAppWindow : public std::enable_shared_from_this<DocViewerAppWindow> {
   public:
    DocViewerAppWindow(const std::string &name);

    void OnImGuiRender();
    void menubar();

    std::shared_ptr<Cherry::AppWindow> &GetAppWindow();
    static std::shared_ptr<DocViewerAppWindow> Create(const std::string &name);
    void SetupRenderCallback();

    void Render();

    void LoadMarkdown(const std::string &path) {
      std::ifstream t(path);
      if (t.is_open()) {
        std::stringstream buffer;
        buffer << t.rdbuf();
        m_cached_markdown_content = buffer.str();
      } else {
        m_cached_markdown_content = "> **Error**: Could not open file at `" + path + "`";
      }
    }

    std::string GetLabelForTopic(const std::string &topic) {
      if (topic == "vx") {
        return "Vortex";
      }

      auto ctx = vxe::GetCurrentContext();
      if (!ctx)
        return topic;

      size_t colon_pos = topic.find(':');
      if (colon_pos != std::string::npos) {
        std::string prefix = topic.substr(0, colon_pos);
        std::string name_to_find = topic.substr(colon_pos + 1);

        if (prefix == "module") {
          for (const auto &mod : ctx->IO.em) {
            if (mod && mod->m_name == name_to_find) {
              return mod->m_proper_name;
            }
          }
        } else if (prefix == "plugin") {
          for (const auto &plug : ctx->IO.ep) {
            if (plug && plug->m_name == name_to_find) {
              return plug->m_proper_name;
            }
          }
        }
      }

      return topic;
    }

    std::string m_selected_topic = "vx";  // TODO: For modules : "module:module_id"
                                          // (for proper names implementation)
    std::string m_selected_section = "";
    std::string m_selected_chapter = "";
    std::string m_cached_markdown_content = "";
    float m_sidebar_width = 250.0f;
    bool m_ConsoleFont = false;

   private:
    VxContext *ctx;
    bool opened;
    std::shared_ptr<AppWindow> app_window_;
  };
}  // namespace vxe

#endif  // LOGUTILITY_H
