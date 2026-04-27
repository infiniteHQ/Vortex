//
//  content_browser_helpers.hpp
//  Some classes and structures for content browser backends
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#include "./modules/interface.hpp"
#include "./plugins/interface.hpp"

#ifndef VORTEX_CONTENT_BROWSER_HELPERS_HPP
#define VORTEX_CONTENT_BROWSER_HELPERS_HPP

class ContenBrowserItem {
 public:
  bool (*detection_callback)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 line_color;
  std::string description;

  ContenBrowserItem(
      bool (*detect_function)(const std::string &path),
      const std::string &name,
      const std::string &description,
      const ImVec4 &line_color)
      : m_Name(name),
        description(description),
        detection_callback(detect_function),
        line_color(line_color) { };
};

class ContenBrowserHandler {
 public:
  void (*f_Execute)(const std::string &path);

  std::string m_Name;

  ImTextureID m_Logo;
  ImVec4 line_color;
  std::string description;
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
  ImVec4 line_color;
  bool m_IsFavorite;
};

class ModuleInterfaceUtility {
  virtual void render() { };

  std::string m_Name;
  ImTextureID m_Logo;
};

#endif  // VORTEX_CONTENT_BROWSER_HELPERS_HPP