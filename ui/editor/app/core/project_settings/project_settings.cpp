#include "./project_settings.hpp"
#include "../../../../../lib/cherry/cherry.hpp"

#include <cstdlib> // std::system
#include <cstring>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <shellapi.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#include <stdlib.h>
#elif defined(__linux__)
#include <stdlib.h>
#endif

namespace VortexEditor {

void PathListEditor(const std::string &type, std::vector<std::string> *list,
                    std::string *newPath) {
  if (!list || !newPath)
    return;

  CherryGUI::PushStyleColor(ImGuiCol_TableRowBg,
                            Cherry::HexToRGBA("#151515FF"));
  if (CherryGUI::BeginTable("##project_paths", 2)) {
    CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

    CherryGUI::TableSetupColumn("Path");
    CherryGUI::TableSetupColumn("Action");
    CherryGUI::TableHeadersRow();

    for (size_t i = 0; i < list->size(); ++i) {
      CherryGUI::TableNextRow();

      CherryGUI::TableSetColumnIndex(0);
      CherryGUI::PushStyleColor(ImGuiCol_TableRowBg,
                                ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

      CherryGUI::Text("%s", (*list)[i].c_str());

      CherryGUI::PopStyleColor();

      CherryGUI::TableSetColumnIndex(1);
      if (CherryKit::ButtonImageText(
              CherryID("delete_entry" + type + std::to_string(i)), "",
              Cherry::GetPath("resources/imgs/trash.png"))
              .GetData("isClicked") == "true") {
        list->erase(list->begin() + i);
        --i;
      }
    }

    CherryGUI::PopStyleVar();
    CherryGUI::EndTable();
  }

  CherryGUI::PopStyleColor();

  if (CherryKit::ButtonImageText(
          CherryID("add_entry" + type), "",
          Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
          .GetData("isClicked") == "true") {
    if (!newPath->empty()) {
      list->push_back(*newPath);
      newPath->clear();
    }
  }

  CherryGUI::SameLine();

  CherryKit::InputString("", newPath);
}

void ProjectSettings::ModulesRender() {
  // Cherry::SetNextComponentProperty("color_text", "#B1FF31"); // Todo remplace
  Cherry::PushFont("ClashBold");
  CherryNextComponent.SetProperty("color_text", "#797979");
  CherryKit::TitleOne("QSf");
  Cherry::PopFont();
  CherryNextComponent.SetProperty("color", "#252525");
  CherryKit::Separator();
}

std::string GetConfigFilePath() {
  std::string homeDir = VortexMaker::getHomeDirectory();
  std::string configPath;
  if (VortexMaker::IsWindows()) {
    configPath = homeDir + "\\.vx\\configs\\dists.json";
  } else {
    configPath = homeDir + "/.vx/configs/dists.json";
  }
  return configPath;
}

nlohmann::json LoadConfig(const std::string &filePath) {
  std::ifstream file(filePath);
  nlohmann::json config;
  if (file.is_open()) {
    file >> config;
  } else {
    config = {{"vortex_dists", {"def"}}, {"VortexEditor_dist", "def"}};
  }
  return config;
}

void SaveConfig(const std::string &filePath, const nlohmann::json &config) {
  std::ofstream file(filePath);
  if (file.is_open()) {
    file << config.dump(4);
  }
}

void ProjectSettings::Refresh() {
  std::string path = VortexMaker::GetCurrentContext()->projectPath.string();
  path += "/vortex.config";

  nlohmann::json projectData = VortexMaker::DumpJSON(path);

  std::shared_ptr<ProjectSettingsSave> newsave =
      std::make_shared<ProjectSettingsSave>();

  VortexMaker::GetCurrentContext()->name =
      projectData["project"]["name"].get<std::string>();
  strncpy(newsave->name, VortexMaker::GetCurrentContext()->name.c_str(),
          sizeof(newsave->name));
  newsave->name[sizeof(newsave->name) - 1] = '\0';

  VortexMaker::GetCurrentContext()->author =
      projectData["project"]["author"].get<std::string>();
  strncpy(newsave->author, VortexMaker::GetCurrentContext()->author.c_str(),
          sizeof(newsave->author));
  newsave->author[sizeof(newsave->author) - 1] = '\0';

  VortexMaker::GetCurrentContext()->description =
      projectData["project"]["description"].get<std::string>();
  strncpy(newsave->description,
          VortexMaker::GetCurrentContext()->description.c_str(),
          sizeof(newsave->description));
  newsave->description[sizeof(newsave->description) - 1] = '\0';

  VortexMaker::GetCurrentContext()->type =
      projectData["project"]["type"].get<std::string>();
  strncpy(newsave->type, VortexMaker::GetCurrentContext()->type.c_str(),
          sizeof(newsave->type));
  newsave->type[sizeof(newsave->type) - 1] = '\0';

  VortexMaker::GetCurrentContext()->project_version =
      projectData["project"]["version"].get<std::string>();
  strncpy(newsave->version, VortexMaker::GetCurrentContext()->version.c_str(),
          sizeof(newsave->version));
  newsave->version[sizeof(newsave->version) - 1] = '\0';

  VortexMaker::GetCurrentContext()->include_system_templates =
      projectData["project"]["include_system_templates"].get<bool>();
  newsave->include_system_templates =
      VortexMaker::GetCurrentContext()->include_system_templates;

  current_save = newsave;
}

void ProjectSettings::RefreshProjectInformations() {
  VortexMaker::RefreshProjectInformations();

  v_ProjectNameInitial = VortexMaker::GetCurrentContext()->name;
  v_ProjectAuthorInitial = VortexMaker::GetCurrentContext()->author;
  v_ProjectVersionInitial = VortexMaker::GetCurrentContext()->project_version;
  v_ProjectDescriptionInitial = VortexMaker::GetCurrentContext()->description;

  v_ProjectName = v_ProjectNameInitial;
  v_ProjectAuthor = v_ProjectAuthorInitial;
  v_ProjectVersion = v_ProjectVersionInitial;
  v_ProjectDescription = v_ProjectDescriptionInitial;
}

void ProjectSettings::UpdateProjectInformations() {
  VortexMaker::UpdateProjectName(v_ProjectName);
  VortexMaker::UpdateProjectDescription(v_ProjectDescription);
  VortexMaker::UpdateProjectVersion(v_ProjectVersion);
  VortexMaker::UpdateProjectAuthor(v_ProjectAuthor);

  v_ProjectNameInitial = v_ProjectName;
  v_ProjectDescriptionInitial = v_ProjectDescription;
  v_ProjectVersionInitial = v_ProjectVersion;
  v_ProjectAuthorInitial = v_ProjectAuthor;

  RefreshProjectInformations();
}

void ProjectSettings::Update() {
  std::string oldname = VortexMaker::GetCurrentContext()->name;

  std::string vortex_version = VORTEX_VERSION;

  nlohmann::json toolchainData;
  toolchainData["project"]["name"] = current_save->name;
  toolchainData["project"]["author"] = current_save->author;
  toolchainData["project"]["description"] = current_save->description;
  toolchainData["project"]["compatibleWith"] = vortex_version;
  toolchainData["project"]["type"] = current_save->type;
  toolchainData["project"]["version"] = current_save->version;
  toolchainData["project"]["include_system_templates"] =
      current_save->include_system_templates;

  std::string path = VortexMaker::GetCurrentContext()->projectPath.string();
  path += "/vortex.config";

  std::ofstream file(path);
  if (file.is_open()) {
    file << std::setw(4) << toolchainData << std::endl;
    VortexMaker::LogInfo("Core", "Object saved to " + path);
    file.close();
  } else {
    VortexMaker::LogError("Core",
                          "Unable to open file " + path + " for writing!");
  }

  Refresh();

  VortexMaker::UpdateEnvironmentProject(oldname);
}

ProjectSettings::ProjectSettings(const std::string &name) {
  m_AppWindow = std::make_shared<Cherry::AppWindow>(name, name);
  m_AppWindow->SetIcon(
      Cherry::GetPath("resources/imgs/icons/misc/icon_home.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetInternalPaddingX(0.0f);
  m_AppWindow->SetInternalPaddingY(0.0f);
  RefreshProjectInformations();

  m_SelectedChildName = "Project's modules";

  this->AddChild(ProjectSettingsChild(
      "Informations",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleFive("Project informations");
        Cherry::PopFont();
        CherryGUI::SameLine();
        CherryKit::TooltipTextCustom("(?)", []() {
          CherryKit::TitleFour("em : Editor Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
          CherryStyle::AddMarginY(10.0f);
          CherryKit::TitleFour("esm : Editor Script Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
        });

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Refresh",
                Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                .GetData("isClicked") == "true") {
          RefreshProjectInformations();
        }

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Save",
                Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                .GetData("isClicked") == "true") {
          UpdateProjectInformations();
        }

        CherryNextProp("color", "#252525");
        CherryKit::Separator();

        CherryKit::TableSimple(
            "Information table",
            {{
                CherryKit::KeyValParent(
                    "Main informations", true,
                    {

                        CherryKit::KeyValString("Name", &v_ProjectName),
                        CherryKit::KeyValString("Description",
                                                &v_ProjectDescription),
                        CherryKit::KeyValString("Version", &v_ProjectVersion),
                        CherryKit::KeyValString("Author", &v_ProjectAuthor),
                    }),
                CherryKit::KeyValParent(
                    "Shipping", false,
                    {

                        CherryKit::KeyValString("Projectg type",
                                                &v_ProjectName),
                    }),
            }});
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")));

  this->AddChild(ProjectSettingsChild(
      "Configurations",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleFive("Project informations");
        Cherry::PopFont();
        CherryGUI::SameLine();
        CherryKit::TooltipTextCustom("(?)", []() {
          CherryKit::TitleFour("em : Editor Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
          CherryStyle::AddMarginY(10.0f);
          CherryKit::TitleFour("esm : Editor Script Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
        });

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Refresh",
                Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                .GetData("isClicked") == "true") {
          RefreshProjectInformations();
        }

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Save",
                Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                .GetData("isClicked") == "true") {
          UpdateProjectInformations();
        }

        CherryNextProp("color", "#252525");
        CherryKit::Separator();

        CherryKit::TableSimple(
            "Information table",
            {{
                CherryKit::KeyValParent(
                    "Main informations", true,
                    {

                        CherryKit::KeyValString("Name", &v_ProjectName),
                        CherryKit::KeyValString("Description",
                                                &v_ProjectDescription),
                        CherryKit::KeyValString("Version", &v_ProjectVersion),
                        CherryKit::KeyValString("Author", &v_ProjectAuthor),
                    }),
                CherryKit::KeyValParent(
                    "Shipping", false,
                    {

                        CherryKit::KeyValString("Projectg type",
                                                &v_ProjectName),
                    }),
            }});
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png")));
  this->AddChild(ProjectSettingsChild(
      "Theme & Customizations",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleFive("Theme & Customizations");
        Cherry::PopFont();
        CherryGUI::SameLine();
        CherryKit::TooltipTextCustom("(?)", []() {
          CherryKit::TitleFour("em : Editor Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
          CherryStyle::AddMarginY(10.0f);
          CherryKit::TitleFour("esm : Editor Script Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
        });

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Refresh",
                Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                .GetData("isClicked") == "true") {
          RefreshProjectInformations();
        }

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Save",
                Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                .GetData("isClicked") == "true") {
          // UpdateProjectEditorSettings
        }

        static std::string color = "#B1FF31";
        static std::vector<std::string> theme_selector = {"Dark", "Light",
                                                          "Custom"};
        static int selected = 0;
        CherryNextProp("color", "#252525");
        CherryKit::Separator();
        CherryKit::TableSimple(
            "General",
            {
                CherryKit::KeyValCustom(
                    "Create new theme",
                    [this]() {
                      Cherry::SetNextComponentProperty("size_x", "200");
                      CherryKit::InputString("", &v_ProjectAuthor);
                      CherryGUI::SameLine();
                      Cherry::SetNextComponentProperty("size_x", "200");
                      CherryKit::ComboText("", &theme_selector, selected);
                      CherryGUI::SameLine();
                      Cherry::SetNextComponentProperty("padding_x", "9");
                      Cherry::SetNextComponentProperty("padding_y", "7");
                      if (CherryKit::ButtonImageText(
                              "Add",
                              Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_add.png"))
                              .GetData("isClicked") == "true") {
                        // UpdateProjectEditorSettings
                      }
                    }),
                CherryKit::KeyValComboString("Theme used", &theme_selector,
                                             selected),
            });
        CherryKit::Separator();

        CherryKit::TableSimple(
            "Customize theme",
            {
                CherryKit::KeyValParent("Theme Settings", {}),
                CherryKit::KeyValParent("Colors",
                                        {
                                            CherryKit::KeyValParent("Windows",
                                                                    {

                                                                    }),
                                            CherryKit::KeyValParent("Elements",
                                                                    {

                                                                    }),
                                        }),
                CherryKit::KeyValParent("Sizes & Scales", {}),
            });
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_human.png")));
  this->AddChild(ProjectSettingsChild(
      "Accessibility",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleFive("Editor");
        Cherry::PopFont();
        CherryGUI::SameLine();
        CherryKit::TooltipTextCustom("(?)", []() {
          CherryKit::TitleFour("em : Editor Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
          CherryStyle::AddMarginY(10.0f);
          CherryKit::TitleFour("esm : Editor Script Modules");
          CherryKit::TextWrapped("Lorem ipsum Lorem ipsumLorem ipsum");
        });

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Refresh",
                Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                .GetData("isClicked") == "true") {
          RefreshProjectInformations();
        }

        CherryGUI::SameLine();
        CherryStyle::AddMarginX(10.0f);
        Cherry::SetNextComponentProperty("padding_x", "8");
        Cherry::SetNextComponentProperty("padding_y", "4");
        if (CherryKit::ButtonImageText(
                "Save",
                Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                .GetData("isClicked") == "true") {
          // UpdateProjectEditorSettings
        }

        CherryNextProp("color", "#252525");
        CherryKit::Separator();
        CherryKit::TableSimple(
            "Text & Informations",
            {
                CherryKit::KeyValString("Language", &v_ProjectDescription),
                CherryKit::KeyValString("Text scale", &v_ProjectDescription),
            });
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")));

  this->AddChild(ProjectSettingsChild(
      "Help",
      [this]() {
        if (CherryKit::ButtonImageTextImage(
                "Learn and Documentation",
                Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
                Cherry::GetPath("resources/imgs/weblink.png"))
                .GetData("isClicked") == "true") {
          // VortexMaker::OpenURL("https://vortex.infinite.si/learn");
        }
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_help.png")));
  std::shared_ptr<Cherry::AppWindow> win = m_AppWindow;
}

std::vector<std::shared_ptr<EnvProject>> ProjectSettings::GetMostRecentProjects(
    const std::vector<std::shared_ptr<EnvProject>> &projects, size_t maxCount) {
  auto sortedProjects = projects;
  std::sort(sortedProjects.begin(), sortedProjects.end(),
            [](const std::shared_ptr<EnvProject> &a,
               const std::shared_ptr<EnvProject> &b) {
              return a->lastOpened > b->lastOpened;
            });

  if (sortedProjects.size() > maxCount) {
    sortedProjects.resize(maxCount);
  }
  return sortedProjects;
}

void ProjectSettings::AddChild(const ProjectSettingsChild &child) {
  m_Childs.push_back(child);
}

void ProjectSettings::RemoveChild(const std::string &child_name) {
  auto it = std::find_if(
      m_Childs.begin(), m_Childs.end(),
      [&child_name](const auto &child) { return child.Name == child_name; });
  if (it != m_Childs.end()) {
    m_Childs.erase(it);
  }
}

std::shared_ptr<Cherry::AppWindow> &ProjectSettings::GetAppWindow() {
  return m_AppWindow;
}

std::shared_ptr<ProjectSettings>
ProjectSettings::Create(const std::string &name) {
  auto instance = std::shared_ptr<ProjectSettings>(new ProjectSettings(name));
  instance->SetupRenderCallback();
  return instance;
}

void ProjectSettings::SetupRenderCallback() {
  auto self = shared_from_this();
  m_AppWindow->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

ProjectSettingsChild *ProjectSettings::GetChild(const std::string &child_name) {
  auto it = std::find_if(m_Childs.begin(), m_Childs.end(),
                         [&child_name](const ProjectSettingsChild &child) {
                           return child.Name == child_name;
                         });
  if (it != m_Childs.end()) {
    return &(*it);
  }
  return nullptr;
}

void ProjectSettings::Render() {
  const float minPaneWidth = 50.0f;
  const float splitterWidth = 1.5f;

  std::string label = "left_pane" + m_AppWindow->m_Name;
  CherryGUI::PushStyleColor(ImGuiCol_ChildBg, Cherry::HexToRGBA("#35353535"));
  CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToRGBA("#00000000"));
  CherryGUI::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
  CherryGUI::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  CherryGUI::BeginChild(label.c_str(), ImVec2(leftPaneWidth, 0), true, 0);

  CherryGUI::SetCursorPosY(CherryGUI::GetCursorPosY() + 5.0f);
  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 5.0f);
  CherryGUI::Image(
      Cherry::GetTexture(Cherry::GetPath("resources/imgs/settings_banner.png")),
      ImVec2(280, 142));

  // CherryStyle::SetPadding(7.0f);

  for (const auto &child : m_Childs) {
    if (child.Name == m_SelectedChildName) {
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
    if (CherryKit::ButtonImageText(CherryID(child_name), child_name.c_str(),
                                   child.LogoPath)
            .GetData("isClicked") == "true") {
      m_SelectedChildName = child.Name;
    }
  }

  CherryGUI::EndChild();
  CherryGUI::PopStyleVar(4);
  CherryGUI::PopStyleColor(2);

  CherryGUI::SameLine();
  CherryGUI::BeginGroup();

  CherryGUI::SetCursorPosX(CherryGUI::GetCursorPosX() + 20.0f);

  if (!m_SelectedChildName.empty()) {
    CherryGUI::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 20.0f));

    if (CherryGUI::BeginChild("ChildPanel", ImVec2(0, 0), false,
                              ImGuiWindowFlags_NoBackground |
                                  ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
      auto child = GetChild(m_SelectedChildName);

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
} // namespace VortexEditor
