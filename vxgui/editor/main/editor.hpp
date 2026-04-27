
#pragma once
#include "../../../vxcore/include/vortex.h"

// Static windows
#include "./core/about/about.hpp"
#include "./core/about_project/about_project.hpp"
#include "./core/credits/credits.hpp"
#include "./core/modules_utility/modules_utility.hpp"
#include "./core/plugins_utility/plugins_utility.hpp"
#include "./core/project_settings/project_settings.hpp"
#include "./core/welcome/welcome.hpp"

// Instanciable windows
#include "./instances/content_browser/content_browser.hpp"
#include "./instances/doc_viewer/doc_viewer.hpp"
#include "./instances/logs_utility/logs_utility.hpp"

#ifndef EDITOR_H
#define EDITOR_H

static std::vector<std::shared_ptr<vxe::ContentBrowser>> content_browser_instances_;
static std::vector<std::shared_ptr<vxe::LogsUtility>> logs_utility_instances_;
static std::vector<std::shared_ptr<vxe::DocViewer>> doc_viewer_instances_;

namespace vxe {

  class Editor {
   public:
    Editor();

    // windows
    bool get_credits_visibility();
    bool get_about_visibility();
    bool get_about_project_visibility();
    bool get_templates_utility_visibility();
    bool get_project_settings_visibility();
    bool get_modules_utility_visibility();
    bool get_plugins_utility_visibility();
    bool get_welcome_visibility();

    void set_credits_visibility(const bool visibility);
    void set_about_visibility(const bool visibility);
    void set_about_project_visibility(const bool visibility);
    void set_templates_utility_visibility(const bool &visibility);
    void set_project_settings_visibility(const bool &visibility, const std::string &tab);
    void set_modules_utility_visibility(const bool &visibility);
    void set_plugins_utility_visibility(const bool &visibility);
    void set_welcome_visibility(const bool &visibility);

    std::string spawn_content_browser();
    std::string spawn_doc_viewer();
    void spawn_content_browser_on_bottom();
    void spawn_logs_utility();
    void toggle_project_settings();

    void render_menubar();
    void render_framebar();

   private:
    // ui static windows
    std::shared_ptr<vxe::Welcome> welcome_window_;
    std::shared_ptr<vxe::ModulesUtility> modules_utility_window_;
    std::shared_ptr<vxe::PluginsUtility> plugins_utility_window_;
    std::shared_ptr<vxe::ProjectSettings> project_settings_window_;
    std::shared_ptr<vxe::AboutVortex> about_window_;
    std::shared_ptr<vxe::AboutProject> about_project_window_;
    std::shared_ptr<vxe::Credits> credits_window_;

    // ui instances
  };

  static std::shared_ptr<Editor> c_Editor;

  VORTEX_API int VortexEditor(int argc, char **argv);
}  // namespace vxe

#endif