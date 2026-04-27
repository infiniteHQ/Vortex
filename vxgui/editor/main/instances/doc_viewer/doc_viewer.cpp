//
//  doc_viewer.cpp
//  Sources for the documentation viewer, including rendering
//  and front end logics
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "doc_viewer.hpp"

namespace vxe {
  DocViewer::DocViewer(const std::string &name) {
    app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
    std::shared_ptr<Cherry::AppWindow> win = app_window_;

    app_window_->SetIcon(Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png"));
    app_window_->SetClosable(true);
    app_window_->SetCloseCallback([this]() { Cherry::DeleteAppWindow(app_window_); });
    app_window_->SetRightMenubarCallback([this]() { render_right_menubar(); });
    app_window_->SetLeftMenubarCallback([this]() { render_left_menubar(); });
  }

  std::shared_ptr<Cherry::AppWindow> &DocViewer::get_app_window() {
    return app_window_;
  }

  std::shared_ptr<DocViewer> DocViewer::create(const std::string &name) {
    auto instance = std::shared_ptr<DocViewer>(new DocViewer(name));
    instance->setup_render_callback();
    return instance;
  }

  void DocViewer::setup_render_callback() {
    auto self = shared_from_this();
    app_window_->SetRenderCallback([self]() {
      if (self) {
        self->render();
      }
    });
  }

  void DocViewer::render_right_menubar() {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("disable_callback", "true");
    if (CherryKit::ButtonImageTextDropdown("Settings", Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"))
            .GetDataAs<bool>("isClicked")) {
      ImVec2 mousePos = CherryGUI::GetMousePos();
      ImVec2 displaySize = CherryGUI::GetIO().DisplaySize;
      ImVec2 popupSize(150, 100);

      if (mousePos.x + popupSize.x > displaySize.x) {
        mousePos.x -= popupSize.x;
      }
      if (mousePos.y + popupSize.y > displaySize.y) {
        mousePos.y -= popupSize.y;
      }

      CherryGUI::SetNextWindowSize(ImVec2(150, 100), ImGuiCond_Appearing);
      CherryGUI::SetNextWindowPos(mousePos, ImGuiCond_Appearing);
      CherryGUI::OpenPopup("SettingsMenuPopup");
    }
    if (CherryGUI::BeginPopup("SettingsMenuPopup")) {
      CherryKit::CheckboxText("Console font", &console_font_);
      CherryGUI::EndPopup();
    }
  }

  void DocViewer::render_left_menubar() {
    auto ctx = vxe::get_current_context();

    std::vector<std::pair<std::string, std::string>> combo_items;
    std::vector<std::string> topic_ids;
    int current_index = 0;
    int counter = 0;

    for (auto const &[topic_id, topic_data] : ctx->documentations) {
      std::string label = get_label_for_topic(topic_id);

      std::string icon = Cherry::GetPath("resources/imgs/icons/misc/icon_docs.png");
      if (topic_id == "vx")
        icon = Cherry::GetPath("resources/imgs/icons/misc/icon_vortex.png");
      else if (topic_id.find("module:") == 0)
        icon = Cherry::GetPath("resources/imgs/icons/misc/icon_module.png");
      else if (topic_id.find("plugin:") == 0)
        icon = Cherry::GetPath("resources/imgs/icons/misc/icon_plugin.png");

      combo_items.push_back({ label, icon });
      topic_ids.push_back(topic_id);

      if (selected_topic_ == topic_id) {
        current_index = counter;
      }
      counter++;
    }

    if (topic_ids.empty())
      return;

    CherryNextComponent.SetProperty("size_x", 180.0f);
    auto combo_result = CherryKit::ComboImageText("##DocTopicSelector", combo_items, current_index);

    int selected_idx = combo_result.GetPropertyAs<int>("selected");
    if (selected_idx >= 0 && selected_idx < (int)topic_ids.size()) {
      std::string new_topic = topic_ids[selected_idx];
      if (new_topic != selected_topic_) {
        selected_topic_ = new_topic;
        selected_section_ = "";
        selected_chapter_ = "";
        cached_markdown_content_ = "";
      }
    }
  }

  void DocViewer::render() {
    auto ctx = vxe::get_current_context();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 221, 0, 255));
    CherryGUI::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    CherryGUI::BeginChild(
        "DevBanner", ImVec2(0, 30), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    CherryGUI::SetCursorPosX(
        (CherryGUI::GetWindowSize().x - CherryGUI::CalcTextSize(
                                            "This documentation utility is under "
                                            "development. Please visit vortex.infinite.si")
                                            .x) *
        0.5f);
    CherryGUI::Text(
        "This documentation utility is under development. Please "
        "visit vortex.infinite.si");

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToImU32("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToImU32("#00000000"));

    CherryGUI::BeginChild("Sidebar", ImVec2(sidebar_width_, 0), true);
    CherryGUI::Spacing();

    if (ctx->documentations.count(selected_topic_)) {
      auto &sections = ctx->documentations[selected_topic_].sections;

      using SectionData = std::decay_t<decltype(sections.begin()->second)>;

      std::unordered_map<std::string, std::vector<std::pair<std::string, std::reference_wrapper<SectionData>>>>
          grouped_sections;

      std::vector<std::string> order;

      for (auto &[section_name, section_data] : sections) {
        size_t pos = section_name.find(':');

        if (pos != std::string::npos) {
          std::string parent = section_name.substr(0, pos);
          std::string child = section_name.substr(pos + 1);

          if (grouped_sections.find(parent) == grouped_sections.end()) {
            order.push_back(parent);
          }

          grouped_sections[parent].push_back({ child, section_data });
        } else {
          if (grouped_sections.find(section_name) == grouped_sections.end()) {
            order.push_back(section_name);
          }

          grouped_sections[section_name].push_back({ "", section_data });
        }
      }

      for (auto &parent : order) {
        CherryGUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

        std::string parent_id = parent + "##parent";
        bool open =
            CherryGUI::TreeNodeEx(parent_id.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth);

        CherryGUI::PopStyleColor();

        if (open) {
          for (auto &[child_name, section_ref] : grouped_sections[parent]) {
            auto &section_data = section_ref.get();

            if (child_name.empty()) {
              for (auto &[chapter_title, file_data] : section_data.chapters) {
                bool is_selected = (selected_chapter_ == chapter_title && selected_section_ == parent);

                std::string unique_id = chapter_title + "##" + parent;

                if (CherryGUI::Selectable(unique_id.c_str(), is_selected)) {
                  selected_section_ = parent;
                  selected_chapter_ = chapter_title;
                  load_markdown(file_data.file_path);
                }
              }
            } else {
              std::string child_id = child_name + "##" + parent;

              bool sub_open = CherryGUI::TreeNodeEx(child_id.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

              if (sub_open) {
                for (auto &[chapter_title, file_data] : section_data.chapters) {
                  bool is_selected = (selected_chapter_ == chapter_title && selected_section_ == parent + ":" + child_name);

                  std::string unique_id = chapter_title + "##" + parent + ":" + child_name;

                  if (CherryGUI::Selectable(unique_id.c_str(), is_selected)) {
                    selected_section_ = parent + ":" + child_name;
                    selected_chapter_ = chapter_title;
                    load_markdown(file_data.file_path);
                  }
                }
                CherryGUI::TreePop();
              }
            }
          }
          CherryGUI::TreePop();
        }

        CherryGUI::Spacing();
      }

    } else {
      CherryGUI::TextWrapped("No documentation found for topic: %s", selected_topic_.c_str());
    }

    CherryGUI::EndChild();
    CherryGUI::PopStyleColor(2);

    CherryGUI::SameLine();
    CherryGUI::Button("##splitter", ImVec2(5, -1));

    if (CherryGUI::IsItemActive()) {
      sidebar_width_ += CherryGUI::GetIO().MouseDelta.x;
      if (sidebar_width_ < 100)
        sidebar_width_ = 100;
    }

    CherryGUI::SameLine();

    CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToImU32("#00000000"));
    CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToImU32("#00000000"));

    if (console_font_) {
      Cherry::PushFont("JetBrainsMono");
      CherryStyle::PushFontSize(0.50f);
    }

    CherryGUI::BeginChild("MarkdownContent", ImVec2(0, 0), false);

    if (!cached_markdown_content_.empty()) {
      CherryGUI::TextDisabled("%s > %s", selected_section_.c_str(), selected_chapter_.c_str());

      CherryGUI::Separator();
      CherryGUI::Spacing();

      CherryGUI::MarkdownView(cached_markdown_content_);
    } else {
      CherryGUI::SetCursorPosY(CherryGUI::GetWindowHeight() / 2.0f);

      const char *hint = "Select a chapter in the sidebar";
      float text_width = CherryGUI::CalcTextSize(hint).x;

      CherryGUI::SetCursorPosX((CherryGUI::GetWindowWidth() - text_width) / 2.0f);

      CherryGUI::TextDisabled("%s", hint);
    }

    CherryGUI::EndChild();

    if (console_font_) {
      CherryStyle::PopFontSize();
      Cherry::PopFont();
    }

    CherryGUI::PopStyleColor(2);
  }

  void DocViewer::load_markdown(const std::string &path) {
    std::ifstream t(path);
    if (t.is_open()) {
      std::stringstream buffer;
      buffer << t.rdbuf();
      cached_markdown_content_ = buffer.str();
    } else {
      cached_markdown_content_ = "> **Error**: Could not open file at `" + path + "`";
    }
  }

  std::string DocViewer::get_label_for_topic(const std::string &topic) {
    if (topic == "vx") {
      return "Vortex";
    }

    auto ctx = vxe::get_current_context();
    if (!ctx)
      return topic;

    size_t colon_pos = topic.find(':');
    if (colon_pos != std::string::npos) {
      std::string prefix = topic.substr(0, colon_pos);
      std::string name_to_find = topic.substr(colon_pos + 1);

      if (prefix == "module") {
        for (const auto &mod : ctx->IO.em) {
          if (mod && mod->name() == name_to_find) {
            return mod->proper_name();
          }
        }
      } else if (prefix == "plugin") {
        for (const auto &plug : ctx->IO.ep) {
          if (plug && plug->name() == name_to_find) {
            return plug->proper_name();
          }
        }
      }
    }

    return topic;
  }

}  // namespace vxe