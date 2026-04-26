//
//  project_settings.cpp
//  Sources of logics and rendering of the project settings utility.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./project_settings.hpp"

namespace vxe {

  ProjectSettings::ProjectSettings(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"));

    app_window_->SetClosable(true);
    app_window_->m_CloseCallback = [=]() { app_window_->SetVisibility(false); };

    app_window_->SetInternalPaddingX(0.0f);
    app_window_->SetInternalPaddingY(0.0f);
    refresh_project_informations();
    refresh_project_themes();

    add_all_childs();

    selected_child_name_ = "Informations";

    std::shared_ptr<Cherry::AppWindow> win = app_window_;
  }

  void ProjectSettings::load_tab_user_want(const std::string &tab) {
    if (tab == "theme") {
      selected_child_name_ = "Appearance";
    } else if ("language") {
      selected_child_name_ = "Accessibility";
    }
  }

  void ProjectSettings::add_child(const ProjectSettingsChild &child) {
    childs_.push_back(child);
  }

  void ProjectSettings::remove_child(const std::string &child_name) {
    auto it =
        std::find_if(childs_.begin(), childs_.end(), [&child_name](const auto &child) { return child.Name == child_name; });
    if (it != childs_.end()) {
      childs_.erase(it);
    }
  }

  std::shared_ptr<Cherry::AppWindow> &ProjectSettings::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<ProjectSettings> ProjectSettings::create(const std::string &name) {
    auto instance = std::shared_ptr<ProjectSettings>(new ProjectSettings(name));
    instance->setup_render_callback();
    return instance;
  }

  void ProjectSettings::refresh_project_themes() {
    available_themes_.clear();
    for (auto t : vxe::get_current_context()->IO.themes) {
      available_themes_.push_back(t->label);
    }
  }

  void ProjectSettings::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  ProjectSettingsChild *ProjectSettings::get_child(const std::string &child_name) {
    auto it = std::find_if(childs_.begin(), childs_.end(), [&child_name](const ProjectSettingsChild &child) {
      return child.Name == child_name;
    });
    if (it != childs_.end()) {
      return &(*it);
    }
    return nullptr;
  }

  void ProjectSettings::render() {
    const float minPaneWidth = 50.0f;
    const float splitterWidth = 1.5f;

    std::string label = "left_pane" + app_window_->m_Name;
    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
    CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    CherryGUI::BeginChild(label.c_str(), ImVec2(left_panel_width_, 0), true, 0);

    CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
    CherryGUI::Image(Cherry::GetTexture(Cherry::GetPath("resources/imgs/settings_banner.png")), ImVec2(280, 142));

    // CherryStyle::SetPadding(7.0f);

    for (const auto &child : childs_) {
      if (child.Name == selected_child_name_) {
        // opt.hex_text_idle = "#FFFFFFFF";
      } else {
        // opt.hex_text_idle = "#A9A9A9FF";
      }
      std::string child_name;

      if (child.Name.rfind("?loc:", 0) == 0) {
        std::string localeName = child.Name.substr(5);
        child_name = Cherry::GetLocale(localeName) + "####" + localeName;
      } else {
        child_name = child.Name;
      }

      CherryNextComponent.SetProperty("color_bg", "#00000000");
      CherryNextComponent.SetProperty("color_border", "#00000000");
      CherryNextComponent.SetProperty("padding_x", "2");
      CherryNextComponent.SetProperty("padding_y", "2");
      CherryNextComponent.SetProperty("size_x", "20");
      CherryNextComponent.SetProperty("size_y", "20");
      CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 7.5f);
      if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(), child.LogoPath).GetData("isClicked") ==
          "true") {
        selected_child_name_ = child.Name;
      }
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleVar(4);
    CherryGUI::PopStyleColor(2);

    CherryGUI::SameLine();
    CherryGUI::BeginGroup();

    CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

    if (!selected_child_name_.empty()) {
      CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

      if (CherryGUI::BeginChild(
              "ChildPanel", ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
        auto child = get_child(selected_child_name_);

        if (child) {
          std::function<void()> pannel_render = child->RenderCallback;
          if (pannel_render) {
            pannel_render();
          }
        }
      }
      CherryGUI::EndChild();

      CherryGUI::PopStyleVar(2);
    }

    CherryGUI::EndGroup();
  }

  void ProjectSettings::project_info_reset() {
    current_project_info_ = initial_project_info_;
  }
  void ProjectSettings::insert_into_tree(
      GroupNode &node,
      const std::vector<std::string> &pathParts,
      size_t index,
      const std::vector<Cherry::Component> &components) {
    if (index >= pathParts.size())
      return;
    auto &child = node.children[pathParts[index]];
    if (index == pathParts.size() - 1) {
      child.components = components;
    } else {
      insert_into_tree(child, pathParts, index + 1, components);
    }
  }

  Cherry::Component ProjectSettings::build_component_tree(const std::string &name, const GroupNode &node) {
    std::vector<Cherry::Component> children;

    if (!node.components.empty()) {
      children.insert(children.end(), node.components.begin(), node.components.end());
    }

    for (const auto &[childName, childNode] : node.children) {
      children.push_back(build_component_tree(childName, childNode));
    }

    return CherryKit::KeyValParent(name, true, children);
  }

  void ProjectSettings::refresh() {
    std::string path = vxe::get_current_context()->projectPath.string();
    path += "/vortex.config";

    nlohmann::json projectData = vxe::dump_json(path);

    std::shared_ptr<ProjectSettingsSave> newsave = std::make_shared<ProjectSettingsSave>();

    vxe::get_current_context()->name = projectData["project"]["name"].get<std::string>();
    strncpy(newsave->name, vxe::get_current_context()->name.c_str(), sizeof(newsave->name));
    newsave->name[sizeof(newsave->name) - 1] = '\0';

    vxe::get_current_context()->author = projectData["project"]["author"].get<std::string>();
    strncpy(newsave->author, vxe::get_current_context()->author.c_str(), sizeof(newsave->author));
    newsave->author[sizeof(newsave->author) - 1] = '\0';

    vxe::get_current_context()->description = projectData["project"]["description"].get<std::string>();
    strncpy(newsave->description, vxe::get_current_context()->description.c_str(), sizeof(newsave->description));
    newsave->description[sizeof(newsave->description) - 1] = '\0';

    vxe::get_current_context()->type = projectData["project"]["type"].get<std::string>();
    strncpy(newsave->type, vxe::get_current_context()->type.c_str(), sizeof(newsave->type));
    newsave->type[sizeof(newsave->type) - 1] = '\0';

    vxe::get_current_context()->project_version = projectData["project"]["version"].get<std::string>();
    strncpy(newsave->version, vxe::get_current_context()->version.c_str(), sizeof(newsave->version));
    newsave->version[sizeof(newsave->version) - 1] = '\0';

    vxe::get_current_context()->include_system_templates = projectData["project"]["include_system_templates"].get<bool>();
    newsave->include_system_templates = vxe::get_current_context()->include_system_templates;

    current_save_ = newsave;
  }

  void ProjectSettings::refresh_project_informations() {
    vxe::refresh_project_informations();
    auto *ctx = vxe::get_current_context();

    initial_project_info_ = {
      .name = ctx->name,
      .description = ctx->description,
      .version = ctx->project_version,
      .thumbnail_path = ctx->logo_path,
      .banner_path = ctx->banner_path,
      .type = ctx->type,
      .tags = ctx->tags,
      .author = ctx->author,
      .website = ctx->website,
      .support_contact = ctx->support_contact,
      .copyright_file = ctx->copyright_file,
      .license_file = ctx->license_file,
      .startup_script = ctx->startup_script,
      .readme_file = ctx->readme_file,
      .requirements_file = ctx->requirements_file,
      .code_of_conduct_file = ctx->code_of_conduct_file,
      .security_file = ctx->security_file,
      .root_content_path = ctx->root_content_path,
    };

    current_project_info_ = initial_project_info_;  // copie propre
  }

  void ProjectSettings::update_project_informations() {
    vxe::update_project_name(current_project_info_.name);
    vxe::update_project_description(current_project_info_.description);
    vxe::update_project_version(current_project_info_.version);
    vxe::update_project_author(current_project_info_.author);
    vxe::update_project_tags(current_project_info_.tags);
    vxe::update_project_type(current_project_info_.type);
    vxe::update_project_logo_path(current_project_info_.thumbnail_path);
    vxe::update_project_website(current_project_info_.website);
    vxe::update_project_support_contact(current_project_info_.support_contact);
    vxe::update_project_copyright_flle(current_project_info_.copyright_file);
    vxe::update_project_license_flle(current_project_info_.license_file);
    vxe::update_project_readme_flle(current_project_info_.readme_file);
    vxe::update_project_requirements_flle(current_project_info_.requirements_file);
    vxe::update_project_code_of_conduct_flle(current_project_info_.code_of_conduct_file);
    vxe::update_project_security_flle(current_project_info_.security_file);
    vxe::update_project_root_content_path(current_project_info_.root_content_path);
    vxe::update_project_startup_script(current_project_info_.startup_script);

    initial_project_info_ = current_project_info_;
    refresh_project_informations();
  }

  void ProjectSettings::update() {
    std::string oldname = vxe::get_current_context()->name;

    std::string vortex_version = VORTEX_VERSION;

    nlohmann::json jsonData;
    jsonData["project"]["name"] = current_save_->name;
    jsonData["project"]["author"] = current_save_->author;
    jsonData["project"]["description"] = current_save_->description;
    jsonData["project"]["compatibleWith"] = vortex_version;
    jsonData["project"]["type"] = current_save_->type;
    jsonData["project"]["version"] = current_save_->version;
    jsonData["project"]["include_system_templates"] = current_save_->include_system_templates;

    std::string path = vxe::get_current_context()->projectPath.string();
    path += "/vortex.config";

    std::ofstream file(path);
    if (file.is_open()) {
      file << std::setw(4) << jsonData << std::endl;
      vxe::log_info("Core", "Object saved to " + path);
      file.close();
    } else {
      vxe::log_error("Core", "Unable to open file " + path + " for writing!");
    }

    refresh();

    vxe::update_environment_project(oldname);
  }

}  // namespace vxe
