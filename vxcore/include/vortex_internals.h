
#pragma once

#ifndef VORTEX_VERSION
#include "vortex.h"
#endif

#include "./modules/interface.h"
#include "./plugins/interface.h"
#include "./templates/install.h"
#include "./templates/interface.h"
#include "./utils/infos.hpp"
#include "./vortex/environment/environment.hpp"
#include "./vortex/events/events.hpp"
#include "./vortex/filesystem/filesystem.hpp"
#include "./vortex/json/json.hpp"
#include "./vortex/logger/logger.hpp"
#include "./vortex/project/project.hpp"
#include "./vortex/random/random.hpp"
#include "./vortex/scripting/scripting.hpp"
#include "./vortex/session/session.hpp"

struct VxContext;

// context pointer
#ifndef CVortexMaker
extern VORTEX_API VxContext *CVortexMaker;
#endif

#define Vx_STATIC_ASSERT(_COND)          static_assert(_COND, "")
#define Vx_BASE64ENCODE(_toencodebuffer) base64_encode(_toencodebuffer);
#define Vx_BASE64DECODE(_todecodebuffer) base64_decode(_todecodebuffer);

struct VortexMakerDebugAllocInfo {
  int TotalAllocCount;  // Number of call to MemAlloc().
  int TotalFreeCount;
  VortexMakerDebugAllocInfo() {
    memset(this, 0, sizeof(*this));
  }
};

struct VxSystemLog {
  spdlog::level::level_enum m_level;
  std::string m_filter;
  std::string m_message;
  std::string m_timestamp;

  VxSystemLog(spdlog::level::level_enum level, std::string filter, std::string message)
      : m_level(level),
        m_filter(filter),
        m_message(message) { };
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

class ContenBrowserItem {
 public:
  bool (*f_Detect)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 m_LineColor;
  std::string m_Description;

  ContenBrowserItem(
      bool (*detect_function)(const std::string &path),
      const std::string &name,
      const std::string &description,
      const ImVec4 &line_color)
      : m_Name(name),
        m_Description(description),
        f_Detect(detect_function),
        m_LineColor(line_color) { };
};

class ContenBrowserHandler {
 public:
  void (*f_Execute)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 m_LineColor;
  std::string m_Description;
};

// All custom pinned folder.
class ContenBrowserPinnedFolder {
 public:
  std::string m_Name;
  std::string m_Path;
  std::string m_Logo;
  ImVec4 m_Color;
};

// On a custom folder, we can change the logo, the color, if it is favorite.
class ContenBrowserCustomFolder {
 public:
  std::string m_Name;
  ImTextureID m_Logo;
  ImVec4 m_LineColor;
  bool m_IsFavorite;
};

class ModuleInterfaceUtility {
  virtual void render() { };

  std::string m_Name;
  ImTextureID m_Logo;
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
  bool (*f_Detect)(const std::string &path);

  std::string m_Name;

  std::string m_LogoPath;
  std::string m_BackgroundImagePath;
  std::string m_LineColor;
  std::string m_Description;

  ItemIdentifierInterface(
      bool (*detect_function)(const std::string &path),
      const std::string &name,
      const std::string &description,
      const std::string &line_color,
      const std::string &logo_path = "",
      const std::string &bg_image_path = "")
      : m_Name(name),
        m_Description(description),
        f_Detect(detect_function),
        m_LineColor(line_color),
        m_LogoPath(logo_path),
        m_BackgroundImagePath(bg_image_path) { };
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
  std::function<void(const std::string &path)> f_CreateFunction;
  std::string m_Name;

  std::string m_LogoPath;
  std::string m_LineColor;
  std::string m_Description;

  ItemCreatorInterface(
      std::function<void(const std::string &path)> function,
      const std::string &name,
      const std::string &description,
      const std::string &line_color = "#343434",
      const std::string &logo_path = "")
      : m_Name(name),
        m_Description(description),
        f_CreateFunction(function),
        m_LineColor(line_color),
        m_LogoPath(logo_path) { };
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
  std::string m_Color;
  bool m_IsFav;
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
  PlatformVendor m_PlatformVendor;

  // Components
  VxIO IO;
  SessionState state;
  VortexMakerDebugAllocInfo debugAllocInfo;
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
//-----------------------------------------------------------------------------

//_____________________________________________________________________________

//__________________________________________________________________________________________________________________
// [SECTION]: Internal API of VortexMaker
//__________________________________________________________________________________________________________________
// Internals functions of HypernetTK. Contain all basic stuff and low level
// features. All of theses functions are called by upper level api & user-api.
// Basicly, you don't need to care about this. For all user interactions of
// Hypernet & Vx, go on the main userapi on vortex.h
//__________________________________________________________________________________________________________________
namespace vxe {

  // Utils & Base
  VORTEX_API void DebugAllocHook(
      VortexMakerDebugAllocInfo *info,
      void *ptr,
      size_t size);  // size >= 0 : alloc, size = -1 : free
  VORTEX_API void AddGeneralUtility(const std::shared_ptr<ModuleInterfaceUtility> &utility);
  VORTEX_API void AddContentBrowserItem(const std::shared_ptr<ContenBrowserItem> &item);

  // Publish to ROM
  VORTEX_API void
  PublishContentBrowserCustomFolder(const std::string &path, const std::string &hex_color, const bool &isFav);
  VORTEX_API void PublishPool(const std::string &absolute_pool_path, const std::string &name);
  VORTEX_API void PostCustomFolderToJson();
  VORTEX_API void PostPoolsToJson();

  // Fetch from ROM
  VORTEX_API void FetchCustomFolders();
  VORTEX_API void FetchPools();

  // Cotnent browser IO manipulations
  VORTEX_API bool ContentBrowserFolderIsFav(const std::string &path);
  VORTEX_API bool GetContentBrowserFolderColor(const std::string &path, ImU32 *color);

  VORTEX_API void Copy(std::vector<std::string> selection, bool in_addition);
  VORTEX_API void Cut(std::vector<std::string> selection, bool in_addition);
  VORTEX_API void ClearCopySelection();
  VORTEX_API void ClearCutSelection();
  VORTEX_API void PasteAllSelections(const std::string &target_path);

  VORTEX_API void DeleteFolder(const std::string &target_path);
  VORTEX_API void DeleteFile(const std::string &target_path);
  VORTEX_API void DeletePath(const std::string &target_path);

  VORTEX_API void RenameFolder(const std::string &target_path, const std::string &new_name);
  VORTEX_API void RenameFile(const std::string &target_path, const std::string &new_name);

  VORTEX_API ImU32 HexToImU32(const std::string &hex);
  VORTEX_API std::string ImU32ToHex(ImU32 color);
}  // namespace vxe
