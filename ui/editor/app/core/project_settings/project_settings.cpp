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

struct GroupNode {
  std::unordered_map<std::string, GroupNode> children;
  std::vector<Cherry::Component> components;
};

void insertIntoTree(GroupNode &node, const std::vector<std::string> &pathParts,
                    size_t index,
                    const std::vector<Cherry::Component> &components) {
  if (index >= pathParts.size())
    return;
  auto &child = node.children[pathParts[index]];
  if (index == pathParts.size() - 1) {
    child.components = components;
  } else {
    insertIntoTree(child, pathParts, index + 1, components);
  }
}

Cherry::Component buildComponentTree(const std::string &name,
                                     const GroupNode &node) {
  std::vector<Cherry::Component> children;

  if (!node.components.empty()) {
    children.insert(children.end(), node.components.begin(),
                    node.components.end());
  }

  for (const auto &[childName, childNode] : node.children) {
    children.push_back(buildComponentTree(childName, childNode));
  }

  return CherryKit::KeyValParent(name, true, children);
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

  auto *ctx = VortexMaker::GetCurrentContext();

  v_ProjectNameInitial = ctx->name;
  v_ProjectAuthorInitial = ctx->author;
  v_ProjectVersionInitial = ctx->project_version;
  v_ProjectDescriptionInitial = ctx->description;
  v_ProjectTagsInitial = ctx->tags;
  v_ProjectTypeInitial = ctx->type;
  v_ProjectThumbnailPathInitial = ctx->logo_path;
  v_ProjectWebsiteInitial = ctx->website;
  v_ProjectSupportContactInitial = ctx->support_contact;
  v_ProjectCopyrightFileInitial = ctx->copyright_file;
  v_ProjectLicenseFileInitial = ctx->license_file;
  v_ProjectReadmeFileInitial = ctx->readme_file;
  v_ProjectRequirementsFileInitial = ctx->requirements_file;
  v_ProjectCodeOfConductFileInitial = ctx->code_of_conduct_file;
  v_ProjectSecurityFileInitial = ctx->security_file;

  v_ProjectName = v_ProjectNameInitial;
  v_ProjectAuthor = v_ProjectAuthorInitial;
  v_ProjectVersion = v_ProjectVersionInitial;
  v_ProjectDescription = v_ProjectDescriptionInitial;
  v_ProjectTags = v_ProjectTagsInitial;
  v_ProjectType = v_ProjectTypeInitial;
  v_ProjectThumbnailPath = v_ProjectThumbnailPathInitial;
  v_ProjectWebsite = v_ProjectWebsiteInitial;
  v_ProjectSupportContact = v_ProjectSupportContactInitial;
  v_ProjectCopyrightFile = v_ProjectCopyrightFileInitial;
  v_ProjectLicenseFile = v_ProjectLicenseFileInitial;
  v_ProjectReadmeFile = v_ProjectReadmeFileInitial;
  v_ProjectRequirementsFile = v_ProjectRequirementsFileInitial;
  v_ProjectCodeOfConductFile = v_ProjectCodeOfConductFileInitial;
  v_ProjectSecurityFile = v_ProjectSecurityFileInitial;
  v_ProjectRootContentPath = v_ProjectRootContentPathInitial;
}

void ProjectSettings::UpdateProjectInformations() {
  VortexMaker::UpdateProjectName(v_ProjectName);
  VortexMaker::UpdateProjectDescription(v_ProjectDescription);
  VortexMaker::UpdateProjectVersion(v_ProjectVersion);
  VortexMaker::UpdateProjectAuthor(v_ProjectAuthor);
  VortexMaker::UpdateProjectTags(v_ProjectTags);
  VortexMaker::UpdateProjectType(v_ProjectType);
  VortexMaker::UpdateProjectLogoPath(v_ProjectThumbnailPath);
  VortexMaker::UpdateProjectWebsite(v_ProjectWebsite);
  VortexMaker::UpdateProjectSupportContact(v_ProjectSupportContact);
  VortexMaker::UpdateProjectCopyrightFile(v_ProjectCopyrightFile);
  VortexMaker::UpdateProjectLicenseFile(v_ProjectLicenseFile);
  VortexMaker::UpdateProjectReadmeFile(v_ProjectReadmeFile);
  VortexMaker::UpdateProjectRequirementsFile(v_ProjectRequirementsFile);
  VortexMaker::UpdateProjectCodeOfConductFile(v_ProjectCodeOfConductFile);
  VortexMaker::UpdateProjectSecurityFile(v_ProjectSecurityFile);
  VortexMaker::UpdateProjectRootContentPath(v_ProjectRootContentPath);

  // Sync initial values
  v_ProjectNameInitial = v_ProjectName;
  v_ProjectDescriptionInitial = v_ProjectDescription;
  v_ProjectVersionInitial = v_ProjectVersion;
  v_ProjectAuthorInitial = v_ProjectAuthor;
  v_ProjectTagsInitial = v_ProjectTags;
  v_ProjectTypeInitial = v_ProjectType;
  v_ProjectThumbnailPathInitial = v_ProjectThumbnailPath;
  v_ProjectWebsiteInitial = v_ProjectWebsite;
  v_ProjectSupportContactInitial = v_ProjectSupportContact;
  v_ProjectCopyrightFileInitial = v_ProjectCopyrightFile;
  v_ProjectLicenseFileInitial = v_ProjectLicenseFile;
  v_ProjectReadmeFileInitial = v_ProjectReadmeFile;
  v_ProjectRequirementsFileInitial = v_ProjectRequirementsFile;
  v_ProjectCodeOfConductFileInitial = v_ProjectCodeOfConductFile;
  v_ProjectSecurityFileInitial = v_ProjectSecurityFile;
  v_ProjectRootContentPathInitial = v_ProjectRootContentPath;

  RefreshProjectInformations();
}

void ProjectSettings::Update() {
  std::string oldname = VortexMaker::GetCurrentContext()->name;

  std::string vortex_version = VORTEX_VERSION;

  nlohmann::json jsonData;
  jsonData["project"]["name"] = current_save->name;
  jsonData["project"]["author"] = current_save->author;
  jsonData["project"]["description"] = current_save->description;
  jsonData["project"]["compatibleWith"] = vortex_version;
  jsonData["project"]["type"] = current_save->type;
  jsonData["project"]["version"] = current_save->version;
  jsonData["project"]["include_system_templates"] =
      current_save->include_system_templates;

  std::string path = VortexMaker::GetCurrentContext()->projectPath.string();
  path += "/vortex.config";

  std::ofstream file(path);
  if (file.is_open()) {
    file << std::setw(4) << jsonData << std::endl;
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
      Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png"));

  m_AppWindow->SetClosable(true);
  m_AppWindow->m_CloseCallback = [=]() { m_AppWindow->SetVisibility(false); };

  m_AppWindow->SetInternalPaddingX(0.0f);
  m_AppWindow->SetInternalPaddingY(0.0f);
  RefreshProjectInformations();
  RefreshProjectThemes();

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

        static std::vector<std::string> project_types = {"Project", "Tool"};
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
                        CherryKit::KeyValString("Thumbnail", &v_ProjectName),
                    }),
                CherryKit::KeyValParent(
                    "Shipping & Distribution", true,
                    {
                        CherryKit::KeyValComboString(
                            CherryID("project_type_combo"), "Project type",
                            &project_types),
                        CherryKit::KeyValString("Tags (separe with commas)",
                                                &v_ProjectTags),
                    }),
                CherryKit::KeyValParent(
                    "Publisher", true,
                    {
                        CherryKit::KeyValString("Author", &v_ProjectAuthor),
                        CherryKit::KeyValString("Website link",
                                                &v_ProjectWebsite),
                        CherryKit::KeyValString("Support or Contact",
                                                &v_ProjectSupportContact),
                    }),
                CherryKit::KeyValParent(
                    "Legal", true,
                    {
                        CherryKit::KeyValString("Copyright file",
                                                &v_ProjectCopyrightFile),
                        CherryKit::KeyValString(
                            "License file",
                            &v_ProjectLicenseFile), // Custom with upper texte
                                                    // preview, input and button
                                                    // path / default path if
                                                    // .vx/data/LICENSE.md
                    }),
                CherryKit::KeyValParent(
                    "Statements", true,
                    {
                        CherryKit::KeyValString("Readme file",
                                                &v_ProjectReadmeFile),
                        CherryKit::KeyValString(
                            "Conventions & Requirements file",
                            &v_ProjectRequirementsFile),
                        CherryKit::KeyValString("Code of conduct file",
                                                &v_ProjectCodeOfConductFile),
                        CherryKit::KeyValString("Safety and security file",
                                                &v_ProjectSecurityFile),
                    }),
            }});

        int type_selected =
            CherryApp.GetComponent(CherryID("project_type_combo"))
                .GetPropertyAs<int>("selected_index");

        switch (type_selected) {
        case 0: {
          v_ProjectType = "project";
          break;
        }
        case 1: {
          v_ProjectType = "tool";
          break;
        }
        default: {
          v_ProjectType = "project";
          break;
        }
        }
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

  this->AddChild(ProjectSettingsChild("Contents", [this]() {
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
            "Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
            .GetData("isClicked") == "true") {
      UpdateProjectInformations();
    }

    CherryNextProp("color", "#252525");
    CherryKit::Separator();

    CherryKit::TableSimple(
        "Content settings",
        {
            CherryKit::KeyValString("Root content path",
                                    &v_ProjectRootContentPath),
        });
  }));

  // TODO : Start a little the editor window, with dedicated save/refresh
  // button. And display a list of themes to modify/delete. Remove Delete from
  // project in the used
  this->AddChild(ProjectSettingsChild(
      "Themes",
      [this]() {
        Cherry::PushFont("ClashBold");
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleTwo("Themes");
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
        static int selected = 0;
        static bool theme_changed = false;
        CherryNextProp("color", "#252525");
        CherryKit::Separator();
        if (!theme_changed) {
          CherryKit::Space(15.0f);
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Project editor theme");
          CherryNextProp("color", "#252525");
          CherryKit::Separator();
          CherryKit::TableSimple(
              "General",
              {

                  CherryKit::KeyValCustom(
                      "Active theme ",
                      [this]() {
                        auto theme_used = VortexMaker::GetSelectedTheme();
                        CherryNextComponent.SetProperty("size_x", 200.0f);
                        CherryKit::ComboText(CherryID("theme_selector"), "",
                                             &m_AvailableThemes, selected);

                        static bool modal = false;

                        CherryKit::ModalSimple(
                            "Are you sure ?", &modal, [this]() {
                              if (CherryKit::ButtonImageText(
                                      "Close", Cherry::GetPath(
                                                   "resources/imgs/icons/misc/"
                                                   "icon_trash.png"))
                                      .GetData("isClicked") == "true") {
                                modal = false;
                              }
                            });
                        if (theme_used)
                          if (theme_used->label != "dark" &&
                              theme_used->label != "white") {
                            CherryGUI::SameLine();
                            if (CherryKit::ButtonImageText(
                                    "Delete from project",
                                    Cherry::GetPath("resources/imgs/icons/misc/"
                                                    "icon_trash.png"))
                                    .GetData("isClicked") == "true") {
                              modal = true;
                            }
                          }

                        selected =
                            CherryApp.GetComponent(CherryID("theme_selector"))
                                .GetPropertyAs<int>("selected");
                      }),
                  CherryKit::KeyValParent(
                      "Theme overrides", true,
                      {
                          // List of themes with up/down buttons
                      }),
              });
        }

        CherryKit::Space(15.0f);
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleFive("Create themes");
        CherryNextProp("color", "#252525");
        CherryKit::Separator();
        CherryKit::TableSimple(
            "Create themes",
            {
                CherryKit::KeyValCustom(
                    "Create new theme",
                    [this]() {
                      CherryNextComponent.SetProperty("size_x", "200");
                      CherryNextComponent.SetProperty("padding_y", "6.0f");
                      CherryNextComponent.SetProperty("description",
                                                      "Enter theme name...");
                      CherryNextComponent.SetProperty("description_logo_place",
                                                      "r");
                      CherryKit::InputString("based on", &v_NewThemeName);

                      CherryGUI::SameLine();
                      Cherry::SetNextComponentProperty("size_x", "150");
                      CherryKit::ComboText("", &m_AvailableThemes, selected);
                      CherryGUI::SameLine();
                      Cherry::SetNextComponentProperty("padding_x", "9");
                      Cherry::SetNextComponentProperty("padding_y", "7");
                      if (CherryKit::ButtonImageText(
                              "Create theme",
                              Cherry::GetPath(
                                  "resources/imgs/icons/misc/icon_add.png"))
                              .GetData("isClicked") == "true") {
                        auto theme =
                            VortexMaker::GetTheme(m_AvailableThemes[selected]);
                        if (theme) {
                          VortexMaker::CreateNewTheme(theme, v_NewThemeName);
                          VortexMaker::RefreshProjectThemes();
                          RefreshProjectThemes();
                        }
                      }
                    }),
            });

        auto selected_theme = VortexMaker::GetSelectedTheme();
        auto theme = VortexMaker::GetTheme(m_AvailableThemes[selected]);

        if (theme && selected_theme) {
          if (theme->label != selected_theme->label) {
            theme_changed = true;
            VortexMaker::GetCurrentContext()->IO.used_theme = theme->label;
            VortexMaker::UpdateProjectThemesComfig();
            VortexMaker::RebuildTheme();
          }
        }

        CherryKit::Space(15.0f);
        CherryNextProp("color_text", "#797979");
        CherryKit::TitleFive("Customize themes");
        CherryNextProp("color", "#252525");
        CherryKit::Separator();

        if (selected_theme) {

          if (selected_theme->label != "dark" &&
              selected_theme->label != "white") {
          }
        }

        if (selected_theme->label != "dark" && selected_theme->label != "white")
          if (!theme_changed) {
            static std::unordered_map<std::string, std::string> themeLabels = {
                {"button_color_border", "Button Border Color"},
                {"button_color_border_hovered", "Button Border Hovered Color"},
                {"button_color_border_clicked", "Button Border Clicked Color"},
                {"button_color_border_pressed", "Button Border Pressed Color"},
                {"button_color_bg", "Button Background Color"},
                {"button_color_bg_hovered", "Button Background Hovered Color"},
                {"button_color_bg_pressed", "Button Background Pressed Color"},
                {"button_color_bg_clicked", "Button Background Clicked Color"},
                {"button_color_text", "Button Text Color"},
                {"button_color_text_hovered", "Button Text Hovered Color"},
                {"button_color_text_pressed", "Button Text Pressed Color"},
                {"button_color_underline", "Button Underline Color"},
                {"button_color_underline_hovered",
                 "Button Underline Hovered Color"},
                {"button_color_underline_pressed",
                 "Button Underline Pressed Color"},
                {"button_size_x", "Button Default Width"},
                {"button_size_y", "Button Default Height"},
                {"button_padding_x", "Button Padding X"},
                {"button_padding_y", "Button Padding Y"},
                {"button_scale", "Button Scale"},
            };

            static std::unordered_map<std::string, std::vector<std::string>>
                themeGroups = {
                    {"Components/Buttons",
                     {"button_color_border", "button_color_border_hovered",
                      "button_color_border_clicked",
                      "button_color_border_pressed", "button_color_bg",
                      "button_color_bg_hovered", "button_color_bg_pressed",
                      "button_color_bg_clicked", "button_color_text",
                      "button_color_text_hovered", "button_color_text_pressed",
                      "button_color_underline",
                      "button_color_underline_hovered",
                      "button_color_underline_pressed", "button_size_x",
                      "button_size_y", "button_padding_x", "button_padding_y",
                      "button_scale"}},
                    {"Components/Checkboxes",
                     {"checkbox_color_border", "checkbox_color_border_hovered",
                      "checkbox_color_border_clicked",
                      "checkbox_color_border_pressed", "checkbox_color_bg",
                      "checkbox_color_bg_hovered", "checkbox_color_bg_pressed",
                      "checkbox_color_bg_clicked", "checkbox_size_x",
                      "checkbox_size_y"}},
                    {"Components/Combos",
                     {"combo_color_border", "combo_color_border_hovered",
                      "combo_color_border_clicked",
                      "combo_color_border_pressed", "combo_color_bg",
                      "combo_color_bg_hovered", "combo_color_bg_pressed",
                      "combo_color_bg_clicked", "combo_size_x",
                      "combo_size_y"}},
                    {"Components/Headers",
                     {"header_color_border",
                      "header_color_border_hovered",
                      "header_color_border_clicked",
                      "header_color_border_pressed",
                      "header_color_bg",
                      "header_color_bg_hovered",
                      "header_color_bg_pressed",
                      "header_color_bg_clicked",
                      "header_size_x",
                      "header_size_y",
                      "header_button_color_border",
                      "header_button_color_border_hovered",
                      "header_button_color_border_clicked",
                      "header_button_color_border_pressed",
                      "header_button_color_bg",
                      "header_button_color_bg_hovered",
                      "header_button_color_bg_pressed",
                      "header_button_color_bg_clicked",
                      "header_button_color_text",
                      "header_button_color_text_hovered",
                      "header_button_color_text_pressed"}},
                    {"Components/Images",
                     {"image_color_border", "image_color_border_hovered",
                      "image_color_border_clicked",
                      "image_color_border_pressed", "image_color_bg",
                      "image_color_bg_hovered", "image_color_bg_pressed",
                      "image_color_bg_clicked", "image_size_x",
                      "image_size_y"}},
                    {"Components/Modals",
                     {"modal_padding_x", "modal_padding_y"}},
                    {"Components/Notifications",
                     {"notification_color_border",
                      "notification_color_border_hovered",
                      "notification_color_bg",
                      "notification_color_bg_hovered"}},
                    {"Components/Separators",
                     {"separator_color", "separator_color_text"}},
                    {"Components/KeyVals", {"keyval_color_text"}},
                    {"Components/Tables",
                     {"table_color_border", "table_cell_padding_x_header",
                      "table_cell_padding_y_header", "table_cell_padding_x_row",
                      "table_cell_padding_y_row"}},
                    {"Components/Text",
                     {"text_color_text", "text_area_color_text"}},
                    {"Components/Titles", {"title_color_text"}},
                    {"Components/Tooltips",
                     {"tooltip_color_border", "tooltip_color_border_hovered",
                      "tooltip_color_border_clicked", "tooltip_color_bg",
                      "tooltip_color_bg_hovered", "tooltip_color_bg_clicked"}},
                    {"Components/Blocks",
                     {"block_color", "block_color_hovered",
                      "block_color_pressed", "block_border_color",
                      "block_border_color_hovered",
                      "block_border_color_pressed", "block_border_radius",
                      "block_border_size"}},
                };

            if (selected_theme) {
              std::unordered_set<std::string> displayedKeys;
              GroupNode root;

              for (const auto &[groupPath, keys] : themeGroups) {
                std::vector<std::string> pathParts;
                std::stringstream ss(groupPath);
                std::string item;
                while (std::getline(ss, item, '/')) {
                  pathParts.push_back(item);
                }

                std::vector<Cherry::Component> groupComponents;
                for (const auto &key : keys) {
                  auto it = selected_theme->theme.find(key);
                  if (it != selected_theme->theme.end()) {
                    displayedKeys.insert(key);
                    std::string label =
                        themeLabels.count(key) ? themeLabels.at(key) : key;
                    groupComponents.push_back(CherryKit::KeyValString(
                        label, &selected_theme->theme[key]));
                  }
                }

                if (!groupComponents.empty()) {
                  insertIntoTree(root, pathParts, 0, groupComponents);
                }
              }

              std::vector<Cherry::Component> mainComponents;
              GroupNode *otherParamsNode = nullptr;

              for (auto &[topName, topNode] : root.children) {
                if (topName == "Other parameters") {
                  otherParamsNode = &topNode;
                } else {
                  mainComponents.push_back(
                      buildComponentTree(topName, topNode));
                }
              }

              std::vector<Cherry::Component> otherComponents;
              for (const auto &[key, value] : selected_theme->theme) {
                if (!displayedKeys.count(key)) {
                  std::string label =
                      themeLabels.count(key) ? themeLabels.at(key) : key;
                  otherComponents.push_back(CherryKit::KeyValString(
                      label, &selected_theme->theme[key]));
                }
              }

              if (!otherComponents.empty()) {
                Cherry::Component otherGroup = CherryKit::KeyValParent(
                    "Other parameters", true, otherComponents);
                mainComponents.push_back(otherGroup);
              }

              std::string label = "table" + selected_theme->label;
              CherryKit::TableSimple(CherryID(label), "Customize theme",
                                     mainComponents);
            }
          } else {
            theme_changed = false;
          }
      },
      Cherry::GetPath("resources/imgs/icons/misc/icon_human.png")));

  this->AddChild(ProjectSettingsChild("Interface", [this]() {
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
            "Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
            .GetData("isClicked") == "true") {
      UpdateProjectInformations();
    }

    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }));

  this->AddChild(ProjectSettingsChild("Modules", [this]() {
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
            "Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
            .GetData("isClicked") == "true") {
      UpdateProjectInformations();
    }

    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }));
  this->AddChild(ProjectSettingsChild("Startup", [this]() {
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
            "Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
            .GetData("isClicked") == "true") {
      UpdateProjectInformations();
    }

    CherryNextProp("color", "#252525");
    CherryKit::Separator();
  }));
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

void ProjectSettings::RefreshProjectThemes() {
  m_AvailableThemes.clear();
  for (auto t : VortexMaker::GetCurrentContext()->IO.themes) {
    m_AvailableThemes.push_back(t->label);
  }
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
