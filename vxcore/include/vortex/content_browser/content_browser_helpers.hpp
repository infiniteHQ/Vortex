#pragma once
#include <vortex.h>

#include "./modules/interface.h"
#include "./plugins/interface.h"

#ifndef VORTEX_CONTENT_BROWSER_HELPERS_HPP
#define VORTEX_CONTENT_BROWSER_HELPERS_HPP

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

#endif  // VORTEX_CONTENT_BROWSER_HELPERS_HPP