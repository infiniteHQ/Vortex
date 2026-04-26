//
//  project_settings_childs.cpp
//  Sources of childs tabs for the project settings utility.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "./project_settings.hpp"

namespace vxe {

  void ProjectSettings::add_informations_child() {
    this->add_child(ProjectSettingsChild(
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();

          static std::vector<std::string> project_types = { "Project", "Tool" };

          if (current_project_info_.type == "tool") {
            CherryApp.GetComponent(CherryID("project_type_combo")).SetProperty("selected_index", "1");
          } else if (current_project_info_.type == "project") {
            CherryApp.GetComponent(CherryID("project_type_combo")).SetProperty("selected_index", "0");
          }

          CherryKit::TableSimple(
              "Information table",
              { {
                  CherryKit::KeyValParent(
                      "Main informations",
                      true,
                      {

                          CherryKit::KeyValString("Name", &current_project_info_.name),
                          CherryKit::KeyValString("Description", &current_project_info_.description),
                          CherryKit::KeyValString("Version", &current_project_info_.version),
                          CherryKit::KeyValString("Logo path", &current_project_info_.thumbnail_path),
                          CherryKit::KeyValString("Banner path", &current_project_info_.banner_path),
                      }),
                  CherryKit::KeyValParent(
                      "Shipping & Distribution",
                      true,
                      {
                          CherryKit::KeyValComboString(CherryID("project_type_combo"), "Project type", &project_types),
                          CherryKit::KeyValString("Tags (separe with commas)", &current_project_info_.tags),
                      }),
                  CherryKit::KeyValParent(
                      "Publisher",
                      true,
                      {
                          CherryKit::KeyValString("Author", &current_project_info_.author),
                          CherryKit::KeyValString("Website link", &current_project_info_.website),
                          CherryKit::KeyValString("Support or Contact", &current_project_info_.support_contact),
                      }),
                  CherryKit::KeyValParent(
                      "Legal",
                      true,
                      {
                          CherryKit::KeyValString("Copyright file", &current_project_info_.copyright_file),
                          CherryKit::KeyValString(
                              "License file",
                              &current_project_info_.license_file),  // Custom with upper texte
                                                                     // preview, input and button
                                                                     // path / default path if
                                                                     // .vx/data/LICENSE.md
                      }),
                  CherryKit::KeyValParent(
                      "Statements",
                      true,
                      {
                          CherryKit::KeyValString("Readme file", &current_project_info_.readme_file),
                          CherryKit::KeyValString(
                              "Conventions & Requirements file", &current_project_info_.requirements_file),
                          CherryKit::KeyValString("Code of conduct file", &current_project_info_.code_of_conduct_file),
                          CherryKit::KeyValString("Safety and security file", &current_project_info_.security_file),
                      }),
              } });

          int type_selected = CherryApp.GetComponent(CherryID("project_type_combo")).GetPropertyAs<int>("selected_index");

          switch (type_selected) {
            case 0: {
              current_project_info_.type = "project";
              break;
            }
            case 1: {
              current_project_info_.type = "tool";
              break;
            }
            default: {
              current_project_info_.type = "project";
              break;
            }
          }
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_info.png")));
  };
  void ProjectSettings::add_configurations_child() {
    this->add_child(ProjectSettingsChild(
        "Configurations",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Project configurations");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();

          CherryKit::TableSimple(
              "Information table",
              { {
                  CherryKit::KeyValParent(
                      "Content Browser",
                      true,
                      {
                          CherryKit::KeyValString("Root content path", &current_project_info_.root_content_path),
                      }),
                  CherryKit::KeyValParent(
                      "Startup",
                      true,
                      {
                          CherryKit::KeyValString("Startup script (lua)", &current_project_info_.startup_script),
                      }),
              } });
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_settings.png")));
  };
  void ProjectSettings::add_appearance_child() {  // TODO : Start a little the editor window, with dedicated save/refresh
    // button. And display a list of themes to modify/delete. Remove Delete from
    // project in the used
    this->add_child(ProjectSettingsChild(
        "Appearance",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleTwo("Appearance of project and editor");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            // UpdateProjectEditorSettings
          }

          static std::string color = "#B1FF31";
          static int selected = 1;
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
                          auto theme_used = vxe::GetSelectedTheme();
                          CherryNextComponent.SetProperty("size_x", 200.0f);
                          CherryKit::ComboText(CherryID("theme_selector"), "", &available_themes_, selected);

                          static bool modal = false;

                          CherryKit::ModalSimple("Are you sure ?", &modal, [this]() {
                            if (CherryKit::ButtonImageText(
                                    "Close",
                                    Cherry::GetPath(
                                        "resources/imgs/icons/misc/"
                                        "icon_trash.png"))
                                    .GetData("isClicked") == "true") {
                              modal = false;
                            }
                          });
                          if (theme_used)
                            if (theme_used->label != "dark" && theme_used->label != "clear") {
                              CherryGUI::SameLine();
                              if (CherryKit::ButtonImageText(
                                      "Delete from project",
                                      Cherry::GetPath(
                                          "resources/imgs/icons/misc/"
                                          "icon_trash.png"))
                                      .GetData("isClicked") == "true") {
                                modal = true;
                              }
                            }

                          selected = CherryApp.GetComponent(CherryID("theme_selector")).GetPropertyAs<int>("selected");
                        }),
                    CherryKit::KeyValParent(
                        "Theme overrides",
                        true,
                        {
                            // List of themes with up/down buttons
                        }),
                });
          }

          CherryKit::Space(15.0f);
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("create themes");
          CherryNextProp("color", "#252525");
          CherryKit::Separator();
          CherryKit::TableSimple(
              "create themes",
              {
                  CherryKit::KeyValCustom(
                      "create new theme",
                      [this]() {
                        CherryNextComponent.SetProperty("size_x", "200");
                        CherryNextComponent.SetProperty("padding_y", "6.0f");
                        CherryNextComponent.SetProperty("description", "Enter theme name...");
                        CherryNextComponent.SetProperty("description_logo_place", "r");
                        CherryKit::InputString("based on", &new_theme_name_);

                        CherryGUI::SameLine();
                        Cherry::SetNextComponentProperty("size_x", "150");
                        CherryKit::ComboText("", &available_themes_, selected);
                        CherryGUI::SameLine();
                        Cherry::SetNextComponentProperty("padding_x", "9");
                        Cherry::SetNextComponentProperty("padding_y", "7");
                        if (CherryKit::ButtonImageText(
                                "create theme", Cherry::GetPath("resources/imgs/icons/misc/icon_add.png"))
                                .GetData("isClicked") == "true") {
                          auto theme = vxe::GetTheme(available_themes_[selected]);
                          if (theme) {
                            vxe::CreateNewTheme(theme, new_theme_name_);
                            vxe::RefreshProjectThemes();
                            refresh_project_themes();
                          }
                        }
                      }),
              });

          auto selected_theme = vxe::GetSelectedTheme();
          auto theme = vxe::GetTheme(available_themes_[selected]);

          if (theme && selected_theme) {
            if (theme->label != selected_theme->label) {
              theme_changed = true;
              vxe::get_current_context()->IO.used_theme = theme->label;
              vxe::UpdateProjectThemesComfig();
              vxe::RebuildTheme();
            }
          }

          CherryKit::Space(15.0f);
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Customize themes");
          CherryNextProp("color", "#252525");
          CherryKit::Separator();

          if (selected_theme) {
            if (selected_theme->label != "dark" && selected_theme->label != "clear") {
              if (!theme_changed) {
                static std::unordered_map<std::string, std::string> themeLabels = {
                  { "button_color_border", "Button Border Color" },
                  { "button_color_border_hovered", "Button Border Hovered Color" },
                  { "button_color_border_clicked", "Button Border Clicked Color" },
                  { "button_color_border_pressed", "Button Border Pressed Color" },
                  { "button_color_bg", "Button Background Color" },
                  { "button_color_bg_hovered", "Button Background Hovered Color" },
                  { "button_color_bg_pressed", "Button Background Pressed Color" },
                  { "button_color_bg_clicked", "Button Background Clicked Color" },
                  { "button_color_text", "Button Text Color" },
                  { "button_color_text_hovered", "Button Text Hovered Color" },
                  { "button_color_text_pressed", "Button Text Pressed Color" },
                  { "button_color_underline", "Button Underline Color" },
                  { "button_color_underline_hovered", "Button Underline Hovered Color" },
                  { "button_color_underline_pressed", "Button Underline Pressed Color" },
                  { "button_size_x", "Button Default Width" },
                  { "button_size_y", "Button Default Height" },
                  { "button_padding_x", "Button Padding X" },
                  { "button_padding_y", "Button Padding Y" },
                  { "button_scale", "Button Scale" },
                };

                static std::unordered_map<std::string, std::vector<std::string>> themeGroups = {
                  { "Components/Buttons",
                    { "button_color_border",
                      "button_color_border_hovered",
                      "button_color_border_clicked",
                      "button_color_border_pressed",
                      "button_color_bg",
                      "button_color_bg_hovered",
                      "button_color_bg_pressed",
                      "button_color_bg_clicked",
                      "button_color_text",
                      "button_color_text_hovered",
                      "button_color_text_pressed",
                      "button_color_underline",
                      "button_color_underline_hovered",
                      "button_color_underline_pressed",
                      "button_size_x",
                      "button_size_y",
                      "button_padding_x",
                      "button_padding_y",
                      "button_scale" } },
                  { "Components/Checkboxes",
                    { "checkbox_color_border",
                      "checkbox_color_border_hovered",
                      "checkbox_color_border_clicked",
                      "checkbox_color_border_pressed",
                      "checkbox_color_bg",
                      "checkbox_color_bg_hovered",
                      "checkbox_color_bg_pressed",
                      "checkbox_color_bg_clicked",
                      "checkbox_size_x",
                      "checkbox_size_y" } },
                  { "Components/Combos",
                    { "combo_color_border",
                      "combo_color_border_hovered",
                      "combo_color_border_clicked",
                      "combo_color_border_pressed",
                      "combo_color_bg",
                      "combo_color_bg_hovered",
                      "combo_color_bg_pressed",
                      "combo_color_bg_clicked",
                      "combo_size_x",
                      "combo_size_y" } },
                  { "Components/Headers",
                    { "header_color_border",
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
                      "header_button_color_text_pressed" } },
                  { "Components/Images",
                    { "image_color_border",
                      "image_color_border_hovered",
                      "image_color_border_clicked",
                      "image_color_border_pressed",
                      "image_color_bg",
                      "image_color_bg_hovered",
                      "image_color_bg_pressed",
                      "image_color_bg_clicked",
                      "image_size_x",
                      "image_size_y" } },
                  { "Components/Modals", { "modal_padding_x", "modal_padding_y" } },
                  { "Components/Notifications",
                    { "notification_color_border",
                      "notification_color_border_hovered",
                      "notification_color_bg",
                      "notification_color_bg_hovered" } },
                  { "Components/Separators", { "separator_color", "separator_color_text" } },
                  { "Components/KeyVals", { "keyval_color_text" } },
                  { "Components/Tables",
                    { "table_color_border",
                      "table_cell_padding_x_header",
                      "table_cell_padding_y_header",
                      "table_cell_padding_x_row",
                      "table_cell_padding_y_row" } },
                  { "Components/Text", { "text_color_text", "text_area_color_text" } },
                  { "Components/Titles", { "title_color_text" } },
                  { "Components/Tooltips",
                    { "tooltip_color_border",
                      "tooltip_color_border_hovered",
                      "tooltip_color_border_clicked",
                      "tooltip_color_bg",
                      "tooltip_color_bg_hovered",
                      "tooltip_color_bg_clicked" } },
                  { "Components/Blocks",
                    { "block_color",
                      "block_color_hovered",
                      "block_color_pressed",
                      "block_border_color",
                      "block_border_color_hovered",
                      "block_border_color_pressed",
                      "block_border_radius",
                      "block_border_size" } },
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
                        std::string label = themeLabels.count(key) ? themeLabels.at(key) : key;
                        groupComponents.push_back(CherryKit::KeyValString(label, &selected_theme->theme[key]));
                      }
                    }

                    if (!groupComponents.empty()) {
                      insert_into_tree(root, pathParts, 0, groupComponents);
                    }
                  }

                  std::vector<Cherry::Component> mainComponents;
                  GroupNode *otherParamsNode = nullptr;

                  for (auto &[topName, topNode] : root.children) {
                    if (topName == "Other parameters") {
                      otherParamsNode = &topNode;
                    } else {
                      mainComponents.push_back(build_component_tree(topName, topNode));
                    }
                  }

                  std::vector<Cherry::Component> otherComponents;
                  for (const auto &[key, value] : selected_theme->theme) {
                    if (!displayedKeys.count(key)) {
                      std::string label = themeLabels.count(key) ? themeLabels.at(key) : key;
                      otherComponents.push_back(CherryKit::KeyValString(label, &selected_theme->theme[key]));
                    }
                  }

                  if (!otherComponents.empty()) {
                    Cherry::Component otherGroup = CherryKit::KeyValParent("Other parameters", true, otherComponents);
                    mainComponents.push_back(otherGroup);
                  }

                  std::string label = "table" + selected_theme->label;
                  CherryKit::TableSimple(CherryID(label), "Customize theme", mainComponents);
                }
              }
            } else {
              CherryNextProp("color_text", "#797979");
              CherryKit::TitleFive(
                  "Cannot customize a default theme, please "
                  "select a custom theme or create a new one.");
            }
          }

          if (theme_changed) {
            theme_changed = false;
          }
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_white_paint.png")));
  };
  void ProjectSettings::add_interface_child() {
    this->add_child(ProjectSettingsChild(
        "Interface",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Interface configurations");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_white_screen.png")));
  };
  void ProjectSettings::add_contents_templates_child() {
    this->add_child(ProjectSettingsChild(
        "Contents & Templates",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Contents & Templates configurations");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();

          CherryKit::TableSimple("Content settings", {});
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_white_box.png")));
  };
  void ProjectSettings::add_modules_child() {
    this->add_child(ProjectSettingsChild(
        "Modules",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Modules settings");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_white_brick.png")));
  };
  void ProjectSettings::add_plugins_child() {
    this->add_child(ProjectSettingsChild(
        "Plugins",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Plugins settings");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_white_plug.png")));
  };
  void ProjectSettings::add_startup_child() {
    this->add_child(ProjectSettingsChild(
        "Startup",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Project startup settings");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            update_project_informations();
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_white_rocket.png")));
  };
  void ProjectSettings::add_accessibility_child() {
    this->add_child(ProjectSettingsChild(
        "Accessibility",
        [this]() {
          Cherry::PushFont("ClashBold");
          CherryNextProp("color_text", "#797979");
          CherryKit::TitleFive("Accessibility configurations");
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
          if (CherryKit::ButtonImageText("refresh", Cherry::GetPath("resources/imgs/icons/misc/icon_save.png"))
                  .GetData("isClicked") == "true") {
            refresh_project_informations();
          }

          CherryGUI::SameLine();
          CherryStyle::AddMarginX(10.0f);
          Cherry::SetNextComponentProperty("padding_x", "8");
          Cherry::SetNextComponentProperty("padding_y", "4");
          if (CherryKit::ButtonImageText("Save", Cherry::GetPath("resources/imgs/icons/misc/icon_return.png"))
                  .GetData("isClicked") == "true") {
            // UpdateProjectEditorSettings
          }

          CherryNextProp("color", "#252525");
          CherryKit::Separator();
          CherryKit::TableSimple(
              "Text & Informations",
              {
                  CherryKit::KeyValString("Language", &current_project_info_.description),
                  CherryKit::KeyValString("Text scale", &current_project_info_.description),
              });
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_copy.png")));
  };
  void ProjectSettings::add_help_child() {
    this->add_child(ProjectSettingsChild(
        "Help",
        [this]() {
          if (CherryKit::ButtonImageTextImage(
                  "Learn and Documentation",
                  Cherry::GetPath("resources/imgs/icons/launcher/docs.png"),
                  Cherry::GetPath("resources/imgs/weblink.png"))
                  .GetData("isClicked") == "true") {
            // vxe::OpenURL("https://vortex.infinite.si/learn");
          }
        },
        Cherry::GetPath("resources/imgs/icons/misc/icon_help.png")));
  };

  void ProjectSettings::add_all_childs() {
    add_informations_child();
    add_configurations_child();
    add_appearance_child();
    add_interface_child();
    add_contents_templates_child();
    add_modules_child();
    add_plugins_child();
    add_startup_child();
    add_accessibility_child();
    add_help_child();
  };
}  // namespace vxe
