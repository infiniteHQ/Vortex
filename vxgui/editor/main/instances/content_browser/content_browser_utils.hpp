#pragma once
#include "../../../../../lib/cherry/cherry.hpp"
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"
#include "./subwindows/add_window/add_window.hpp"

#ifndef VORTEX_EDITOR_CONTENT_BROWSER_UTILS_H
#define VORTEX_EDITOR_CONTENT_BROWSER_UTILS_H

using namespace Cherry;

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <stack>
#include <unordered_set>

namespace VortexEditor {
class ContenBrowserItem {
public:
  bool (*f_Detect)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 m_LineColor;
  std::string m_Description;

  ContenBrowserItem(bool (*detect_function)(const std::string &path),
                    const std::string &name, const std::string &description,
                    const ImVec4 &line_color)
      : m_Name(name), m_Description(description), f_Detect(detect_function),
        m_LineColor(line_color) {};
};
enum class ContentShowMode { Thumbmails, List, Objects };

enum class FileTypes {
  // Special,
  File_LICENSE,
  File_VORTEX,

  // Very low level
  File_ASM,
  File_BIN,

  // Programming languages
  File_C,
  File_H,
  File_CPP,
  File_HPP,
  File_INL,
  File_RUST,
  File_ZIG,
  File_GO,
  File_JAVA,
  File_JAVASCRIPT,
  File_TYPESCRIPT,
  File_COBOL,
  File_PASCAL,
  File_CARBON,
  File_PYTHON,
  File_RUBY,
  File_PHP,
  File_SWIFT,
  File_CSHARP,
  File_VB,
  File_DART,
  File_KOTLIN,
  File_SCALA,
  File_PERL,
  File_SHELL,
  File_BASH,
  File_BATCH,
  File_LUA,
  File_R,
  File_MATLAB,
  File_JULIA,
  File_SQL,
  File_GROOVY,
  File_FORTRAN,
  File_ERLANG,
  File_ELIXIR,
  File_OCAML,

  // Web and Markup
  File_HTML,
  File_CSS,
  File_XML,

  // Config
  File_CFG,
  File_JSON,
  File_YAML,
  File_INI,

  // Git
  File_GIT,

  // Documents
  File_TXT,
  File_MD,
  File_DOC,
  File_PDF,

  // Pictures
  File_PICTURE,

  // Videos
  File_VIDEO,

  // Audio
  File_AUDIO,

  // Archives
  File_ARCHIVE,

  // Miscellaneous
  File_LOG,
  File_BACKUP,
  File_TEMP,
  File_DATA,

  // Other
  File_UNKNOWN,
};

struct FileHandler {
  FileTypes for_type;
  std::string file_handler_name;
  std::string handler_module_name;
  std::string handler_plugin_name;
};

struct ContentBrowserChild {
  std::function<void()> m_Child;
  std::string m_Name;
  bool m_Disabled = true;
  float m_DefaultSize = 0.0f;
  float m_Size = 0.0f;
  ImVec4 m_BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

  void Enable() { m_Disabled = false; }

  void Disable() { m_Disabled = true; }

  ContentBrowserChild(const std::string &name,
                      const std::function<void()> &child,
                      float defaultSize = 0.0f)
      : m_Name(name), m_Child(child), m_DefaultSize(defaultSize),
        m_Size(defaultSize) {}
  ContentBrowserChild() {};
};

struct FileTypeInfo {
  const char *label;
  const char *icon;
  const std::string color; // ou IM_COL32
};

struct ItemCardStyle {
  ImU32 bg_color;
  ImU32 border_color;
  ImU32 line_color;
  float max_text_width;
  float border_radius;
};

struct ItemCardContent {
  const std::string &name;
  const std::string &path;
  const std::string &description;
  const std::string &size;
  const std::string &logo;
  const std::shared_ptr<ItemIdentifierInterface> &item_ident;
};

struct ItemCardParams {
  ItemCardContent &content;
  ItemCardStyle &style;
  bool selected;
  bool isFolder;
};

static const std::unordered_map<FileTypes, FileTypeInfo> FILE_TYPE_INFOS = {
    {FileTypes::File_VORTEX,
     {"Vortex File", "resources/imgs/icons/files/icon_vortex_file.png",
      "#B1FF31"}},
    {FileTypes::File_LICENSE,
     {"License File", "resources/imgs/icons/files/icon_license_file.png",
      "#454545"}},
    {FileTypes::File_PICTURE,
     {"Picture File", "resources/imgs/icons/files/icon_picture_file.png",
      "#454545"}},
    {FileTypes::File_VIDEO,
     {"Video File", "resources/imgs/icons/files/icon_video.png", "#454545"}},
    {FileTypes::File_AUDIO,
     {"Audio File", "resources/imgs/icons/files/icon_audio.png", "#454545"}},
    {FileTypes::File_HTML,
     {"HTML File", "resources/imgs/icons/files/icon_html.png", "#FF4646"}},
    {FileTypes::File_CSS,
     {"CSS File", "resources/imgs/icons/files/icon_css.png", "#FF4646"}},
    {FileTypes::File_XML,
     {"XML File", "resources/imgs/icons/files/icon_xml.png", "#46FF4C"}},
    {FileTypes::File_GIT,
     {"Git File", "resources/imgs/icons/files/icon_git_file.png", "#454545"}},
    {FileTypes::File_LUA,
     {"Lua File", "resources/imgs/icons/files/icon_lua.png", "#4C46FF"}},
    {FileTypes::File_PHP,
     {"PHP File", "resources/imgs/icons/files/icon_php.png", "#9346FF"}},
    {FileTypes::File_RUST,
     {"Rust File", "resources/imgs/icons/files/icon_rust_file.png", "#FF7E46"}},
    {FileTypes::File_ZIG,
     {"Zig File", "resources/imgs/icons/files/icon_zig.png", "#FFF946"}},
    {FileTypes::File_GO,
     {"Go File", "resources/imgs/icons/files/icon_go.png", "#46FFF4"}},
    {FileTypes::File_ZIG,
     {"Zig File", "resources/imgs/icons/files/icon_zig.png", "#46FFF4"}},
    {FileTypes::File_ASM,
     {"Assembly File", "resources/imgs/icons/files/icon_asm.png", "#CA3939"}},
    {FileTypes::File_JAVASCRIPT,
     {"Javascript File", "resources/imgs/icons/files/icon_js.png", "#FFF946"}},
    {FileTypes::File_TYPESCRIPT,
     {"Typescript File", "resources/imgs/icons/files/icon_ts.png", "#0077FF"}},
    {FileTypes::File_PYTHON,
     {"Python File", "resources/imgs/icons/files/icon_py.png", "#FFC446"}},
    {FileTypes::File_JAVA,
     {"Java File", "resources/imgs/icons/files/icon_java.png", "#FF4646"}},
    {FileTypes::File_JSON,
     {"JSON File", "resources/imgs/icons/files/icon_json.png", "#454545"}},
    {FileTypes::File_H,
     {"C Header File", "resources/imgs/icons/files/icon_c_h_file.png",
      "#C183FF"}},
    {FileTypes::File_C,
     {"C Source File", "resources/imgs/icons/files/icon_c_file.png",
      "#83C3FF"}},
    {FileTypes::File_CSHARP,
     {"C# Source File", "resources/imgs/icons/files/icon_cs.png", "#AE46FF"}},
    {FileTypes::File_HPP,
     {"C++ Header File", "resources/imgs/icons/files/icon_cpp_h_file.png",
      "#B946FF"}},
    {FileTypes::File_CPP,
     {"C++ Source File", "resources/imgs/icons/files/icon_cpp_file.png",
      "#4690FF"}},
    {FileTypes::File_INI,
     {"Init File", "resources/imgs/icons/files/icon_ini_file.png", "#454545"}},
    {FileTypes::File_TXT,
     {"Text File", "resources/imgs/icons/files/icon_text_file.png", "#454545"}},
    {FileTypes::File_CFG,
     {"Config File", "resources/imgs/icons/files/icon_text_cfg.png",
      "#454545"}},
    {FileTypes::File_BIN,
     {"Binary File", "resources/imgs/icons/files/icon_bin_file.png",
      "#454545"}},
};

static const FileTypeInfo DEFAULT_TYPE_INFO = {
    "File", "resources/imgs/icons/files/icon_unknow_file.png", "#676767"};

} // namespace VortexEditor

#endif // VORTEX_EDITOR_CONTENT_BROWSER_UTILS_H