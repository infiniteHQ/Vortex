//
//  crash_report.cpp
//  Sources for the CrashReport utility, displayed on the Vortex Crash Handler
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "crash_report.hpp"

CrashReport::CrashReport(const std::string &name, const std::string &session_id) {
  app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
  app_window_->SetIcon(Cherry::GetPath("ch_resources/imgs/icon_crash.png"));
  app_window_->SetClosable(false);

  app_window_->m_TabMenuCallback = []() {
    ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
    ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    if (ImGui::BeginMenu("Edit")) {
      ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
      ImGui::Text("Main stuff");
      ImGui::PopStyleColor();

      ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
      ImGui::Separator();
      ImGui::PopStyleColor();

      if (ImGui::MenuItem("Logs Utility", "Overview of all logs")) {
      }

      if (ImGui::MenuItem("Logs2 Utility", "Overview of all logs")) {
      }

      ImGui::EndMenu();
    }
  };

  app_window_->SetInternalPaddingX(10.0f);
  app_window_->SetInternalPaddingY(10.0f);

  session_id_ = session_id;

  std::shared_ptr<Cherry::AppWindow> win = app_window_;
}

std::string CrashReport::get_home_directory() {
#if defined(__linux__) || defined(__APPLE__)
  const char *homePath = std::getenv("HOME");
  if (homePath == nullptr) {
    throw std::runtime_error("HOME environment variable not set");
  }
  return std::string(homePath);
#elif defined(_WIN32) || defined(_WIN64)
  const char *homePath = std::getenv("USERPROFILE");
  if (homePath == nullptr) {
    const char *homeDrive = std::getenv("HOMEDRIVE");
    const char *homePathEnv = std::getenv("HOMEPATH");
    if (homeDrive == nullptr || homePathEnv == nullptr) {
      throw std::runtime_error("HOMEPATH environment variables not set");
    }
    return std::string(homeDrive) + std::string(homePathEnv);
  }
  return std::string(homePath);
#endif
  throw std::runtime_error("Unknown platform: Unable to determine home directory");
}

// Updated loadFileToString function
bool loadFileToString(const std::string &filename, char *text, size_t maxSize) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return false;  // Unable to open file
  }

  std::string line;
  size_t index = 0;

  while (std::getline(file, line)) {
    if (index + line.size() < maxSize - 1) {    // Check boundary to avoid overflow
      std::strcpy(text + index, line.c_str());  // Copy line into text buffer
      index += line.size();
      text[index++] = '\n';  // Add newline character
    } else {
      break;  // Text buffer full
    }
  }

  text[index] = '\0';  // Null-terminate the string

  file.close();
  return true;
}

static std::vector<std::tuple<std::string, std::string, std::string>> modifiable_values;

std::shared_ptr<Cherry::AppWindow> &CrashReport::get_app_window() {
  return app_window_;
}

std::shared_ptr<CrashReport> CrashReport::create(const std::string &name, const std::string &id) {
  auto instance = std::shared_ptr<CrashReport>(new CrashReport(name, id));
  instance->setup_render_callback();
  return instance;
}

void CrashReport::setup_render_callback() {
  auto self = shared_from_this();
  app_window_->SetRenderCallback([self]() {
    if (self) {
      self->render();
    }
  });
}

void CrashReport::render() {
  float oldsize = ImGui::GetFont()->Scale;
  ImGui::GetFont()->Scale *= 1.5;
  ImGui::PushFont(ImGui::GetFont());

  ImGui::Text("Oups, Vortex crashed :(");

  ImGui::GetFont()->Scale = oldsize;
  ImGui::PopFont();

  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 105));
  ImGui::TextWrapped(
      "A Vortex process has encountered a fatal error or produced a bad "
      "output. This could be due to a faulty module, plugins, or a malfunction "
      "in the Vortex process itself. You can now review the last states and "
      "investigate what happened. However, it is safe to close this window.");
  ImGui::PopStyleColor(1);

  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 50));
  ImGui::TextWrapped("Error topic ID : ");
  ImGui::PopStyleColor(1);

  ImGui::SameLine();

  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 105));
  ImGui::TextWrapped(session_id_.c_str());
  ImGui::PopStyleColor(1);

  ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
    if (ImGui::BeginTabItem("Summary")) {
      float oldsize = ImGui::GetFont()->Scale;
      ImGui::GetFont()->Scale *= 1.3;
      ImGui::PushFont(ImGui::GetFont());
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 100));
      ImGui::TextWrapped("Latest informations");
      ImGui::PopStyleColor(1);
      ImGui::GetFont()->Scale = oldsize;
      ImGui::PopFont();

      static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                     ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

      if (ImGui::BeginTable("table_deps", 2, flags)) {
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        for (int i = 0; i < 4; i++) {
          ImGui::TableNextRow();
          for (int column = 0; column < 2; column++) {
            ImGui::TableSetColumnIndex(column);

            if (i == 0) {
              if (column == 0) {
                ImGui::Text("Latest fatal error");
              } else if (column == 1) {
                ImGui::Text("The current path doesn't exist !");
              }
            } else if (i == 1) {
              if (column == 0) {
                ImGui::Text("Latest error");
              } else if (column == 1) {
                ImGui::Text("My big project");
              }
            } else if (i == 2) {
              if (column == 0) {
                ImGui::Text("Latest warning");
              } else if (column == 1) {
                ImGui::Text("4qsfdsgsd");
              }
            } else if (i == 3) {
              if (column == 0) {
                ImGui::Text("Latest information");
              } else if (column == 1) {
                ImGui::Text("4fhdgfhdfgg");
              }
            }
          }
        }
        ImGui::EndTable();
      }

      ImGui::GetFont()->Scale *= 1.3;
      ImGui::PushFont(ImGui::GetFont());
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 100));
      ImGui::TextWrapped("Master context");
      ImGui::PopStyleColor(1);
      ImGui::GetFont()->Scale = oldsize;
      ImGui::PopFont();

      if (ImGui::BeginTable("table_deps", 2, flags)) {
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        std::string vortex_launcher_version = VORTEX_VERSION;
        std::string cherry_version = CHERRY_VERSION;
        std::string os_name = OS_NAME;
        std::string os_arch = ARCH_NAME;

        for (int i = 0; i < 8; i++) {
          ImGui::TableNextRow();
          for (int column = 0; column < 2; column++) {
            ImGui::TableSetColumnIndex(column);

            if (i == 0) {
              if (column == 0) {
                ImGui::Text("Vortex Version");
              } else if (column == 1) {
                const std::string version = VORTEX_VERSION;
                ImGui::Text(vortex_launcher_version.c_str());
              }
            } else if (i == 1) {
              if (column == 0) {
                ImGui::Text("Build");
              } else if (column == 1) {
                const std::string build = "Build: " + vxe::get_vortex_build_id() + " ; " + vxe::get_build_date() + " (" +
                                          vxe::get_vortex_editor_dist() + ")";
                ImGui::Text(build.c_str());
              }
            } else if (i == 2) {
              if (column == 0) {
                ImGui::Text("Hash");
              } else if (column == 1) {
                const std::string hash = " exe(" + vxe::get_vortex_editor_hash() + ") git(" + vxe::get_git_commit() + ")";
                ImGui::Text(hash.c_str());
              }
            } else if (i == 3) {
              if (column == 0) {
                ImGui::Text("System");
              } else if (column == 1) {
                const std::string system = os_name + " " + os_arch + system_desktop;
                ImGui::Text(system.c_str());
              }
            }
          }
        }
        ImGui::EndTable();
      }

      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Informations")) {
      static const int bufferSize = 1024 * 16;
      // Declare the text buffer
      static char text[bufferSize];

      // Load content from file into text variable
      static std::string log_file = get_home_directory() + "/.vx/sessions/" + session_id_ + "/logs/global.log";
      if (!loadFileToString(log_file, text, bufferSize)) {
        // Handle error opening or reading the file
        ImGui::Text("Failed to open or read file: %s", log_file.c_str());
      }

      Cherry::PushFont("JetBrainsMono");
      CherryStyle::PushFontSize(0.50f);
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToImU32("#00000000"));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
      static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly;
      ImGui::InputTextMultiline("##logs_source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, -FLT_MIN - 35.0f), flags);
      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor();
      CherryStyle::PopFontSize();
      Cherry::PopFont();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Processus report")) {
      static const int bufferSize = 1024 * 16;
      // Declare the text buffer
      static char text[bufferSize];

      // Load content from file into text variable
      static std::string log_file = get_home_directory() + "/.vx/sessions/" + session_id_ + "/crash/core_dumped.txt";
      if (!loadFileToString(log_file, text, bufferSize)) {
        // Handle error opening or reading the file
        ImGui::Text("Failed to open or read file: %s", log_file.c_str());
      }

      Cherry::PushFont("JetBrainsMono");
      CherryStyle::PushFontSize(0.50f);
      CherryGUI::PushStyleColor(ImGuiCol_Border, Cherry::HexToImU32("#00000000"));
      CherryGUI::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
      CherryGUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

      static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly;
      ImGui::InputTextMultiline("##processus_source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, -FLT_MIN - 35.0f), flags);

      CherryStyle::PopFontSize();
      CherryGUI::PopStyleVar(2);
      CherryGUI::PopStyleColor();
      Cherry::PopFont();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }

  std::string text = CherryApp.GetLocale("loc.continue") + CherryApp.GetLocale("loc.close");
  ImVec2 to_remove = CherryGUI::CalcTextSize(text.c_str());
  CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 50);
  CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 35.0f);
  CherryNextProp("color", "#222222");
  CherryKit::Separator();

  CherryNextProp("color_text", "#B1FF31");
  if (CherryKit::ButtonText("Close").GetData("isClicked") == "true") {
    Cherry::Application().Get().Close();
  }

  CherryGUI::SetCursorPosY(CherryGUI::GetContentRegionMax().y - 28.0f);
  CherryGUI::SetCursorPosX(CherryGUI::GetContentRegionMax().x - to_remove.x - 20);

  CherryNextProp("color_text", "#B1FF31");
  if (CherryKit::ButtonText("Restart project").GetData("isClicked") == "true") {
    Cherry::Application().Get().Close();
  }

  CherryGUI::SameLine();

  Cherry::SetNextComponentProperty("color_bg", "#B1FF31FF");
  Cherry::SetNextComponentProperty("color_bg_hovered", "#C3FF53FF");
  Cherry::SetNextComponentProperty("color_text", "#121212FF");
  if (CherryKit::ButtonText("Start Launcher").GetData("isClicked") == "true") {
  }
}
