#pragma once
#include <vortex.h>

#ifndef VORTEX_DOCUMENTATION_HPP
#define VORTEX_DOCUMENTATION_HPP

namespace vxe {
  VORTEX_API std::vector<std::string> get_sections(const std::string &topic);
  VORTEX_API std::vector<std::string> get_chapters(const std::string &topic, const std::string &section);
  VORTEX_API std::vector<std::string> get_titles_from_topic(const std::string &topic);
  VORTEX_API std::vector<std::string> get_names_from_topic_and_title(const std::string &topic, const std::string &title);
  VORTEX_API void add_vortex_documentation();
  VORTEX_API std::string
  get_chapter_file_path(const std::string &topic, const std::string &section, const std::string &title);
  VORTEX_API void add_documentation(
      const std::string &topic,
      const std::string &section,
      const std::string &title,
      const std::string &md_file_path);

}  // namespace vxe

#endif  // VORTEX_DOCUMENTATION_HPP