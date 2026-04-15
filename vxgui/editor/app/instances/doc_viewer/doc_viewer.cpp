#include "doc_viewer.hpp"

#include <iostream>

namespace VortexEditor {
DocViewerAppWindow::DocViewerAppWindow(const std::string &name) {
  std::cout << "DocViewerAppWindow" << std::endl;
  m_AppWindow = std::make_shared<AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_journal.png"));
  std::shared_ptr<AppWindow> win = m_AppWindow;
  m_AppWindow->m_Closable = true;
  m_AppWindow->SetCloseCallback(
      [this]() { Cherry::DeleteAppWindow(m_AppWindow); });

  m_AppWindow->SetRightMenubarCallback([this]() {
    CherryNextComponent.SetProperty("padding_y", "6.0f");
    CherryNextComponent.SetProperty("padding_x", "10.0f");
    CherryNextComponent.SetProperty("disable_callback", "true");
    if (CherryKit::ButtonImageTextDropdown(
            "Settings", GetPath("resources/imgs/icons/misc/icon_settings.png"))
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
      CherryKit::CheckboxText("Console font", &m_ConsoleFont);
      CherryGUI::EndPopup();
    }
  });

  m_AppWindow->SetLeftMenubarCallback([this]() {
    auto ctx = VortexMaker::GetCurrentContext();

    std::vector<std::pair<std::string, std::string>> combo_items;
    std::vector<std::string> topic_ids;
    int current_index = 0;
    int counter = 0;

    for (auto const &[topic_id, topic_data] : ctx->documentations) {
      std::string label = GetLabelForTopic(topic_id);

      std::string icon = GetPath("resources/imgs/icons/misc/icon_docs.png");
      if (topic_id == "vx")
        icon = GetPath("resources/imgs/icons/misc/icon_vortex.png");
      else if (topic_id.find("module:") == 0)
        icon = GetPath("resources/imgs/icons/misc/icon_module.png");
      else if (topic_id.find("plugin:") == 0)
        icon = GetPath("resources/imgs/icons/misc/icon_plugin.png");

      combo_items.push_back({label, icon});
      topic_ids.push_back(topic_id);

      if (m_selected_topic == topic_id) {
        current_index = counter;
      }
      counter++;
    }

    if (topic_ids.empty())
      return;

    CherryNextComponent.SetProperty("size_x", 180.0f);
    auto combo_result = CherryKit::ComboImageText("##DocTopicSelector",
                                                  combo_items, current_index);

    int selected_idx = combo_result.GetPropertyAs<int>("selected");
    if (selected_idx >= 0 && selected_idx < (int)topic_ids.size()) {
      std::string new_topic = topic_ids[selected_idx];
      if (new_topic != m_selected_topic) {
        m_selected_topic = new_topic;
        m_selected_section = "";
        m_selected_chapter = "";
        m_cached_markdown_content = "";
      }
    }
  });

  m_AppWindow->SetLeftBottombarCallback([this]() {

  });

  this->ctx = VortexMaker::GetCurrentContext();
}

std::shared_ptr<Cherry::AppWindow> &DocViewerAppWindow::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<DocViewerAppWindow>
DocViewerAppWindow::Create(const std::string &name) {
  auto instance =
      std::shared_ptr<DocViewerAppWindow>(new DocViewerAppWindow(name));
  instance->SetupRenderCallback();
  return instance;
}

void DocViewerAppWindow::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void DocViewerAppWindow::Render() {
  auto ctx = VortexMaker::GetCurrentContext();

  ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToImU32("#00000000"));
  ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToImU32("#00000000"));

  ImGui::BeginChild("Sidebar", ImVec2(m_sidebar_width, 0), true);
  ImGui::Spacing();

  if (ctx->documentations.count(m_selected_topic)) {
    auto &sections = ctx->documentations[m_selected_topic].sections;

    using SectionData = std::decay_t<decltype(sections.begin()->second)>;

    std::unordered_map<std::string,
                       std::vector<std::pair<
                           std::string, std::reference_wrapper<SectionData>>>>
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

        grouped_sections[parent].push_back({child, section_data});
      } else {
        if (grouped_sections.find(section_name) == grouped_sections.end()) {
          order.push_back(section_name);
        }

        grouped_sections[section_name].push_back({"", section_data});
      }
    }

    for (auto &parent : order) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

      std::string parent_id = parent + "##parent";
      bool open = ImGui::TreeNodeEx(parent_id.c_str(),
                                    ImGuiTreeNodeFlags_DefaultOpen |
                                        ImGuiTreeNodeFlags_SpanAvailWidth);

      ImGui::PopStyleColor();

      if (open) {
        for (auto &[child_name, section_ref] : grouped_sections[parent]) {
          auto &section_data = section_ref.get();

          if (child_name.empty()) {
            for (auto &[chapter_title, file_data] : section_data.chapters) {

              bool is_selected = (m_selected_chapter == chapter_title &&
                                  m_selected_section == parent);

              std::string unique_id = chapter_title + "##" + parent;

              if (ImGui::Selectable(unique_id.c_str(), is_selected)) {
                m_selected_section = parent;
                m_selected_chapter = chapter_title;
                LoadMarkdown(file_data.file_path);
              }
            }
          } else {
            std::string child_id = child_name + "##" + parent;

            bool sub_open = ImGui::TreeNodeEx(
                child_id.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

            if (sub_open) {
              for (auto &[chapter_title, file_data] : section_data.chapters) {

                bool is_selected =
                    (m_selected_chapter == chapter_title &&
                     m_selected_section == parent + ":" + child_name);

                std::string unique_id =
                    chapter_title + "##" + parent + ":" + child_name;

                if (ImGui::Selectable(unique_id.c_str(), is_selected)) {
                  m_selected_section = parent + ":" + child_name;
                  m_selected_chapter = chapter_title;
                  LoadMarkdown(file_data.file_path);
                }
              }
              ImGui::TreePop();
            }
          }
        }
        ImGui::TreePop();
      }

      ImGui::Spacing();
    }

  } else {
    ImGui::TextWrapped("No documentation found for topic: %s",
                       m_selected_topic.c_str());
  }

  ImGui::EndChild();
  ImGui::PopStyleColor(2);

  ImGui::SameLine();
  ImGui::Button("##splitter", ImVec2(5, -1));

  if (ImGui::IsItemActive()) {
    m_sidebar_width += ImGui::GetIO().MouseDelta.x;
    if (m_sidebar_width < 100)
      m_sidebar_width = 100;
  }

  ImGui::SameLine();

  ImGui::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToImU32("#00000000"));
  ImGui::PushStyleColor(ImGuiCol_Border, Cherry::HexToImU32("#00000000"));

  if (m_ConsoleFont) {
    Cherry::PushFont("JetBrainsMono");
    CherryStyle::PushFontSize(0.50f);
  }

  ImGui::BeginChild("MarkdownContent", ImVec2(0, 0), false);

  if (!m_cached_markdown_content.empty()) {
    ImGui::TextDisabled("%s > %s", m_selected_section.c_str(),
                        m_selected_chapter.c_str());

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::MarkdownView(m_cached_markdown_content);
  } else {
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2.0f);

    const char *hint = "Select a chapter in the sidebar";
    float text_width = ImGui::CalcTextSize(hint).x;

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) / 2.0f);

    ImGui::TextDisabled("%s", hint);
  }

  ImGui::EndChild();

  if (m_ConsoleFont) {
    CherryStyle::PopFontSize();
    Cherry::PopFont();
  }

  ImGui::PopStyleColor(2);
}

} // namespace VortexEditor