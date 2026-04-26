#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

VORTEX_API std::vector<std::string> vxe::get_titles_from_topic(const std::string &topic) {
  auto ctx = vxe::get_current_context();

  auto it = ctx->credits.find(topic);
  if (it == ctx->credits.end())
    return {};

  return it->second.title_order;
}

VORTEX_API std::vector<std::string> vxe::get_names_from_topic_and_title(const std::string &topic, const std::string &title) {
  auto ctx = vxe::get_current_context();

  auto it = ctx->credits.find(topic);
  if (it == ctx->credits.end())
    return {};

  auto &tc = it->second;
  auto sit = tc.sections.find(title);
  if (sit == tc.sections.end())
    return {};

  std::vector<std::string> results;
  for (const auto &entry : sit->second) {
    std::string line = entry.name;
    if (!entry.contact.empty()) {
      bool is_email = entry.contact.find('@') != std::string::npos;
      if (is_email)
        line += " <" + entry.contact + ">";
      else
        line += " (" + entry.contact + ")";
    }
    results.push_back(line);
  }
  return results;
}

VORTEX_API void vxe::add_documentation(
    const std::string &topic,
    const std::string &section,
    const std::string &title,
    const std::string &md_file_path) {
  auto ctx = vxe::get_current_context();
  DocumentationFile doc;
  doc.title = title;
  doc.file_path = md_file_path;

  ctx->documentations[topic].sections[section].chapters[title] = doc;
}

VORTEX_API std::vector<std::string> vxe::get_sections(const std::string &topic) {
  auto ctx = vxe::get_current_context();
  std::vector<std::string> section_names;

  if (ctx->documentations.count(topic)) {
    for (auto const &[name, content] : ctx->documentations[topic].sections) {
      section_names.push_back(name);
    }
  }
  return section_names;
}

VORTEX_API std::vector<std::string> vxe::get_chapters(const std::string &topic, const std::string &section) {
  auto ctx = vxe::get_current_context();
  std::vector<std::string> chapter_titles;

  if (ctx->documentations.count(topic)) {
    auto &sections = ctx->documentations[topic].sections;
    if (sections.count(section)) {
      for (auto const &[title, file] : sections[section].chapters) {
        chapter_titles.push_back(title);
      }
    }
  }
  return chapter_titles;
}

VORTEX_API std::string
vxe::get_chapter_file_path(const std::string &topic, const std::string &section, const std::string &title) {
  auto ctx = vxe::get_current_context();

  auto it_topic = ctx->documentations.find(topic);
  if (it_topic == ctx->documentations.end())
    return "";

  auto &sections_map = it_topic->second.sections;
  auto it_section = sections_map.find(section);
  if (it_section == sections_map.end())
    return "";

  auto &chapters_map = it_section->second.chapters;
  auto it_chapter = chapters_map.find(title);
  if (it_chapter == chapters_map.end())
    return "";

  return it_chapter->second.file_path;
}

VORTEX_API void vxe::add_vortex_documentation() {
  vxe::add_documentation(
      "vx", "Introduction", "Introduction", Cherry::GetPath("docs/get_started/contents/introduction/introduction.md"));
  vxe::add_documentation(
      "vx", "Introduction", "What is Vortex ?", Cherry::GetPath("docs/get_started/contents/introduction/what_is_vortex.md"));
  vxe::add_documentation(
      "vx", "Introduction", "Install Vortex", Cherry::GetPath("docs/get_started/contents/introduction/install_vortex.md"));
  vxe::add_documentation(
      "vx", "Introduction", "Get started", Cherry::GetPath("docs/get_started/contents/introduction/get_started.md"));

  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand Vortex Launcher",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "understand_vortexlauncher.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Discover interface",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "discover_interface.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Update the launcher",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "update_launcher.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Manage projects",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_projects.md"));

  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand logical contents:Brief",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "understand_logical_contents.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand logical contents:Manage system modules",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_modules.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand logical contents:Manage system plugins",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_plugins.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand static contents:Brief",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "understand_static_contents.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand static contents:Manage system templates",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_templates.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Understand static contents:Manage system contents",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_system_contents.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Manage Vortex Editors",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "manage_vortex_editor_versions.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Subscribe to beta",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "subscribe_to_beta.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Launcher",
      "Uninstallation",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_launcher/"
          "uninstall.md"));

  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Understand Vortex Editor",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "understand_vortexeditor.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Discover interface",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "discover_interface.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Content Browser",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "content_browser.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Project settings",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "project_settings.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Console logs",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "console_logs.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Handle modules",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "handle_modules.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Handle plugins",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "handle_plugins.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Import contents",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "import_contents.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Create with templates",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "create_with_templates.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Brief",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "share_contents.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Export project/tool",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "export_project.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Share a content",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "share_a_content.md"));
  vxe::add_documentation(
      "vx",
      "Take the Vortex Editor",
      "Share Contents:Share a content",
      Cherry::GetPath(
          "docs/get_started/contents/take_vortex_editor/"
          "create_a_template.md"));
}
