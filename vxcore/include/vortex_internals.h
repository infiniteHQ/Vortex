//
//  vortex_internals.h
//  Includes, structures and context pointer for the internal API of vortex.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once

#ifndef VORTEX_VERSION
#include "vortex.h"
#endif

#include "./modules/interface.hpp"
#include "./plugins/interface.hpp"
#include "./templates/install.hpp"
#include "./templates/interface.hpp"
#include "./utils/infos.hpp"
#include "./vortex/content_browser/content_browser.hpp"
#include "./vortex/credits/credits.hpp"
#include "./vortex/customizations/customizations.hpp"
#include "./vortex/documentation/documentation.hpp"
#include "./vortex/environment/environment.hpp"
#include "./vortex/events/events.hpp"
#include "./vortex/filesystem/filesystem.hpp"
#include "./vortex/json/json.hpp"
#include "./vortex/logger/logger.hpp"
#include "./vortex/project/project.hpp"
#include "./vortex/random/random.hpp"
#include "./vortex/scripting/scripting.hpp"
#include "./vortex/session/session.hpp"
#include "./vortex/theme/theme.hpp"
#include "./vortex/utilities/utilities.hpp"

struct VxContext;

// context pointer
#ifndef CVortexMaker
extern VORTEX_API std::weak_ptr<VxContext> CVortexMaker;
#endif

struct VxSystemLog {
  spdlog::level::level_enum level;
  std::string filter;
  std::string message;
  std::string timestamp;

  VxSystemLog(spdlog::level::level_enum level, std::string filter, std::string message)
      : level(level),
        filter(filter),
        message(message) { };
};

struct EnvProject {
  std::string name;
  std::string path;
  std::string version;
  std::string compatibleWith;
  std::string description;
  std::string logoPath;
  std::string author;
  std::string lastOpened;
};

enum class PlatformVendor { Linux, Windows, Macos };

struct SessionState {
  //
  std::string session_id;

  // Master
  std::string session_started_at;
  std::string session_started_on_os;
  std::string session_started_on_arch;

  // Last module
  std::shared_ptr<ModuleInterface> last_used_module;

  bool master_initialized = false;
  bool logs_modified = false;
  bool last_used_module_modified = false;
  bool last_used_plugin_modified = false;
  bool last_used_module_input_event_modified = false;
  bool last_used_module_output_event_modified = false;
};

struct ItemIdentifierInterface {
 public:
  bool (*detection_callback)(const std::string &path);

  std::string name;

  std::string logo_path;
  std::string background_image_path;
  std::string line_color;
  std::string description;

  ItemIdentifierInterface(
      bool (*detect_function)(const std::string &path),
      const std::string &name,
      const std::string &description,
      const std::string &line_color,
      const std::string &logo_path = "",
      const std::string &bg_image_path = "")
      : name(name),
        description(description),
        detection_callback(detect_function),
        line_color(line_color),
        logo_path(logo_path),
        background_image_path(bg_image_path) { };
};

struct ItemHandlerInterface {
  std::function<void(const std::string &)> handler;
  std::string title;
  std::string type;
  std::string description;
  std::string logo;

  ItemHandlerInterface(
      const std::string &ty,
      std::function<void(const std::string &)> h,
      const std::string &ti,
      const std::string &d = "",
      const std::string &l = "")
      : handler(std::move(h)),
        type(ty),
        title(ti),
        description(d),
        logo(l) {
  }
};

// Todo : Creation configurations (names, variantes, etc)
struct ItemCreatorInterface {
  std::function<void(const std::string &path)> create_function;
  std::string name;

  std::string logo_path;
  std::string line_color;
  std::string description;

  ItemCreatorInterface(
      std::function<void(const std::string &path)> function,
      const std::string &name,
      const std::string &description,
      const std::string &line_color = "#343434",
      const std::string &logo_path = "")
      : name(name),
        description(description),
        create_function(function),
        line_color(line_color),
        logo_path(logo_path) { };
};

enum class DevFlag {
  DEPRECIATED,
  READY,
};

enum class HappeningState {
  INFO,
  WARNING,
  ERR,
  FATAL,
};

enum class HandlerItemType { File, Item, Folder };

struct ContentBrowserCustomFolder {
  std::string path;
  std::string color;
  bool is_fav;
};

struct Theme {
  std::string label;
  std::string name;
  std::string description;
  std::string authors;
  std::unordered_map<std::string, std::string> theme;
};

struct VxIO {
  int MetricsActiveAllocations;

  // EM / Editor Modules
  std::vector<void *> em_handles;
  std::vector<std::shared_ptr<ModuleInterface>> em;
  std::vector<std::shared_ptr<ModuleInterface>> sys_em;

  // EP / Editor Plugins
  std::vector<void *> ep_handles;
  std::vector<std::shared_ptr<PluginInterface>> ep;
  std::vector<std::shared_ptr<PluginInterface>> sys_ep;

  // Know projects in system
  std::vector<std::shared_ptr<EnvProject>> sys_projects;

  // Templates
  std::vector<std::shared_ptr<TemplateInterface>> templates;
  std::vector<std::shared_ptr<TemplateInterface>> sys_templates;

  // Pools
  std::vector<std::string> sys_projects_pools;
  std::vector<std::string> sys_vortex_versions_pools;
  std::vector<std::string> sys_modules_pools;
  std::vector<std::string> sys_templates_pools;
  std::vector<std::string> sys_plugins_pools;

  // Themes
  std::vector<std::shared_ptr<Theme>> themes;
  std::string used_theme;
  std::vector<std::string> override_themes;
  bool theme_changed = false;  // Flag, when the user change the theme this flag
                               // will be true and the UI will be rebuilded wich
                               // changed values, whatever if refresh or test

  // Content browser items
  std::vector<std::shared_ptr<ContenBrowserItem>> contentbrowser_items;
  std::vector<std::shared_ptr<ContentBrowserCustomFolder>> contentbrowser_customfolders;
  std::string contentbrowser_mainpool;
  std::string contentbrowser_absolute_mainpool;
  std::vector<std::pair<std::string, std::string>> contentbrowser_pools;
  std::vector<std::string> copy_selection;
  std::vector<std::string> cut_selection;
  float past_state;  // from 0.0f (0%) to 1.0f (100%)

  // Main utilities
  std::vector<std::shared_ptr<ModuleInterfaceUtility>> em_utilities;
};

struct VxPaths {
  std::string toolchainDistFolder;
  std::string hostDistFolder;
};

struct CreditEntry {
  std::string name;
  std::string contact;
};

struct DocumentationFile {
  std::string title;
  std::string file_path;
};

struct DocumentationSection {
  std::map<std::string, DocumentationFile> chapters;
};

struct DocumentationTopic {
  std::map<std::string, DocumentationSection> sections;
};
struct TopicCredits {
  std::vector<std::string> title_order;
  std::map<std::string, std::vector<CreditEntry>> sections;
};

struct EditMenuItem {
  std::string title;
  std::function<void()> action;
  std::string logo;
  std::string section;
};

struct CustomMenu {
  std::string title;
  std::function<void()> render;
};

//-----------------------------------------------------------------------------
// (Context) VortexMakerContext => Main VortexMaker context.
//-----------------------------------------------------------------------------
// This context contain all user data about VortexMaker functionnal interfaces &
// all instances of custom contents.
//-----------------------------------------------------------------------------
struct VxContext {
  // Master flags
  bool initialized;

  // Loger
  bool logger;
  bool logger_registering = true;
  std::shared_ptr<spdlog::logger> global_logger;
  std::shared_ptr<spdlog::logger> console_logger;
  std::vector<std::pair<std::string, std::shared_ptr<spdlog::logger>>> pool_loggers;

  // Vendor
  PlatformVendor platform_vendor;

  // Components
  VxIO IO;
  SessionState state;
  std::vector<std::shared_ptr<VxSystemLog>> registered_logs;
  fs::path projectPath;
  fs::path projectDataPath;
  fs::path logoPath;
  VxPaths paths;
  std::string configFilePath;
  std::string author;
  std::string compatibleWith;
  std::string description;
  std::string label;
  std::string name;
  std::string type;
  std::string version;
  std::string tags;
  std::string logo_path;
  std::string banner_path;
  std::string website;
  std::string support_contact;
  std::string copyright_file;
  std::string license_file;
  std::string readme_file;
  std::string requirements_file;
  std::string code_of_conduct_file;
  std::string security_file;
  std::string project_version;
  std::string root_content_path;

  std::string startup_script;

  std::string toolchainsPath;
  std::string gposPath;
  std::string packagesPath;
  std::string scriptsPath;
  std::string hostsPath;
  bool include_system_templates;

  std::string platform;
  std::string arch;

  // Credits
  std::map<std::string, TopicCredits> credits;

  // Embedded documentation
  std::map<std::string, DocumentationTopic> documentations;

  // Edit callbacks
  std::vector<EditMenuItem> editMenuItems;

  // Custom menus callabcks
  std::vector<CustomMenu> customMenus;
};