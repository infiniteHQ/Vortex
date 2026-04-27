#include "modules_details.hpp"

using namespace Cherry;

ModuleDetails::ModuleDetails(const std::string &name, const std::shared_ptr<ModuleInterface> &module) {
  this->m_module = module;

  app_window_ = std::make_shared<Cherry::AppWindow>(name, name);
  app_window_->SetIcon("/usr/local/include/Vortex/imgs/vortex.png");
  app_window_->SetClosable(false);

  app_window_->SetInternalPaddingX(10.0f);
  app_window_->SetInternalPaddingY(10.0f);
};

std::shared_ptr<Cherry::AppWindow> &ModuleDetails::GetAppWindow() {
  return app_window_;
}

std::shared_ptr<ModuleDetails> ModuleDetails::Create(
    const std::string &name,
    const std::shared_ptr<ModuleInterface> &module) {
  auto instance = std::shared_ptr<ModuleDetails>(new ModuleDetails(name, module));
  instance->SetupRenderCallback();
  return instance;
}

void ModuleDetails::SetupRenderCallback() {
  auto self = shared_from_this();
  app_window_->SetRenderCallback([self]() {
    if (self) {
      self->Render();
    }
  });
}

void ModuleDetails::close() {
  this->opened = false;
}

void ModuleDetails::Render() {
  float oldsize = CherryGUI::GetFont()->Scale;
  CherryGUI::GetFont()->Scale *= 1.3;
  CherryGUI::PushFont(CherryGUI::GetFont());

  CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module :");
  CherryGUI::SameLine();
  CherryGUI::Text(this->m_module->proper_name().c_str());

  CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module State :");
  CherryGUI::SameLine();
  if (this->m_module->state() == "running") {
    CherryGUI::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 0.8f), "Running");
  }
  if (this->m_module->state() == "failed") {
    CherryGUI::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 0.8f), "Failed");
  }
  if (this->m_module->state() == "unknow" || this->m_module->state() == "stopped") {
    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Not running");
  }

  CherryGUI::GetFont()->Scale = oldsize;
  CherryGUI::PopFont();

  CherryGUI::Separator();

  CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module name : ");
  CherryGUI::SameLine();
  CherryGUI::Text(this->m_module->name().c_str());

  CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Module version : ");
  CherryGUI::SameLine();
  CherryGUI::Text(this->m_module->version().c_str());

  CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Compatible with Vortex version(s) : ");
  CherryGUI::SameLine();
  static std::string version;
  version.clear();
  for (auto ver : this->m_module->supported_versions()) {
    version += "[" + ver + "]";
  }
  CherryGUI::Text(version.c_str());

  CherryGUI::Separator();
  if (CherryGUI::CollapsingHeader("Dependancies")) {
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    if (CherryGUI::BeginTable("table_deps", 4, flags)) {
      CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("Version", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableHeadersRow();
      for (int i = 0; i < this->m_module->dependencies().size(); i++) {
        CherryGUI::TableNextRow();
        for (int column = 0; column < 4; column++) {
          CherryGUI::TableSetColumnIndex(column);

          if (column == 0) {
            CherryGUI::Text(this->m_module->dependencies()[i]->name.c_str());
          } else if (column == 1) {
            CherryGUI::Text(this->m_module->dependencies()[i]->type.c_str());
          } else if (column == 2) {
            static std::string versions;
            versions.clear();
            for (auto ver : this->m_module->dependencies()[i]->supported_versions) {
              version += "[" + ver + "] ";
            }

            CherryGUI::Text(versions.c_str());
          } else if (column == 3) {
            if (this->m_module->dependencies()[i]->satisfied) {
              CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Satisfied");
            } else {
              CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not satisfied");
            }
          }
        }
      }
      CherryGUI::EndTable();
    }
  }
  if (CherryGUI::CollapsingHeader("Instances")) {
    // Behavior
  }
  if (CherryGUI::CollapsingHeader("Input events")) {
    for (int row = 0; row < this->m_module->input_events().size(); row++) {
      if (CherryGUI::TreeNode(this->m_module->input_events()[row]->name_.c_str())) {
        CherryGUI::BeginChildFrame(CherryGUI::GetID(this->m_module->input_events()[row]->name_.c_str()), ImVec2(0, 400));
        CherryGUI::Text("Name : ");
        CherryGUI::SameLine();
        CherryGUI::Text(this->m_module->input_events()[row]->name_.c_str());
        CherryGUI::Text("Description : ");
        CherryGUI::SameLine();
        CherryGUI::Text(this->m_module->input_events()[row]->description_.c_str());

        if (this->m_module->input_events()[row]->m_devflag == DevFlag::READY) {
          CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
        } else if (this->m_module->input_events()[row]->m_devflag == DevFlag::DEPRECIATED) {
          CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Depreciated");
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (CherryGUI::BeginTable("table_input_event_args", 3, flags)) {
          CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("C++ type", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();
          for (int param = 0; param < this->m_module->input_events()[row]->m_params.size(); param++) {
            CherryGUI::TableNextRow();
            for (int column = 0; column < 3; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                CherryGUI::Text(std::get<0>(this->m_module->input_events()[row]->m_params[param]).c_str());
              }
              if (column == 1) {
                CherryGUI::Text(std::get<1>(this->m_module->input_events()[row]->m_params[param]).c_str());
              }
              if (column == 2) {
                CherryGUI::Text(std::get<2>(this->m_module->input_events()[row]->m_params[param]).c_str());
              }
            }
          }
          CherryGUI::EndTable();
        }
        CherryGUI::EndChildFrame();
        CherryGUI::TreePop();
      }
    }
  }
  if (CherryGUI::CollapsingHeader("Functions")) {
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    for (int row = 0; row < this->m_module->functions().size(); row++) {
      if (CherryGUI::TreeNode(this->m_module->functions()[row]->name_.c_str())) {
        CherryGUI::BeginChildFrame(CherryGUI::GetID(this->m_module->functions()[row]->name_.c_str()), ImVec2(0, 400));
        CherryGUI::Text("Name : ");
        CherryGUI::SameLine();
        CherryGUI::Text(this->m_module->functions()[row]->name_.c_str());
        CherryGUI::Text("Description : ");
        CherryGUI::SameLine();
        CherryGUI::Text(this->m_module->functions()[row]->description_.c_str());

        /*if (this->m_module->functions()[row]->m_devflag == DevFlag::READY)
        {
            CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ready to use");
        }
        else if (this->m_module->functions()[row]->m_devflag == DevFlag::DEPRECIATED)
        {
            CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Depreciated");
        }*/

        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (CherryGUI::BeginTable("table_input_event_args", 3, flags)) {
          CherryGUI::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("C++ type", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
          CherryGUI::TableHeadersRow();
          for (int param = 0; param < this->m_module->functions()[row]->m_params_def.size(); param++) {
            CherryGUI::TableNextRow();
            for (int column = 0; column < 3; column++) {
              CherryGUI::TableSetColumnIndex(column);

              if (column == 0) {
                CherryGUI::Text(std::get<0>(this->m_module->functions()[row]->m_params_def[param]).c_str());
              }
              if (column == 1) {
                CherryGUI::Text(std::get<1>(this->m_module->functions()[row]->m_params_def[param]).c_str());
              }
              if (column == 2) {
                CherryGUI::Text(std::get<2>(this->m_module->functions()[row]->m_params_def[param]).c_str());
              }
            }
          }
          CherryGUI::EndTable();
        }
        CherryGUI::EndChildFrame();
        CherryGUI::TreePop();
      }
    }
  }
  if (CherryGUI::CollapsingHeader("History")) {
    // Events boostrapped, with informations, args, return ,etc...
    // Behavior
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    if (CherryGUI::BeginTable("table_", 4, flags)) {
      CherryGUI::TableSetupColumn("Trigger", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableHeadersRow();
      for (int i = 0; i < this->m_module->input_events().size(); i++) {
        for (int row = 0; row < this->m_module->input_events()[i]->m_happenings.size(); row++) {
          CherryGUI::TableNextRow();
          for (int column = 0; column < 4; column++) {
            CherryGUI::TableSetColumnIndex(column);

            if (column == 0) {
              CherryGUI::Text(this->m_module->input_events()[i]->m_happenings[row]->trigger_name.c_str());
            } else if (column == 1) {
              if (this->m_module->input_events()[i]->m_happenings[row]->state == HappeningState::FATAL) {
                CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
              } else if (this->m_module->input_events()[i]->m_happenings[row]->state == HappeningState::ERR) {
                CherryGUI::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
              } else if (this->m_module->input_events()[i]->m_happenings[row]->state == HappeningState::WARNING) {
                CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
              } else if (this->m_module->input_events()[i]->m_happenings[row]->state == HappeningState::INFO) {
                CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
              }
            } else if (column == 2) {
              CherryGUI::Text(this->m_module->input_events()[i]->m_happenings[row]->timestamp.c_str());
            } else if (column == 2) {
              CherryGUI::Text(this->m_module->input_events()[i]->m_happenings[row]->log.c_str());
            }
          }
        }
      }
      CherryGUI::EndTable();
    }
  }
  if (CherryGUI::CollapsingHeader("Logs")) {
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                   ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    static std::vector<std::shared_ptr<VxSystemLog>> logs;

    logs.clear();
    for (int i = 0; i < this->m_ctx->registered_logs.size(); i++) {
      if (this->m_ctx->registered_logs[i]->filter == this->m_module->name()) {
        logs.push_back(this->m_ctx->registered_logs[i]);
      }
    }

    if (CherryGUI::BeginTable("table_", 4, flags)) {
      CherryGUI::TableSetupColumn("Trigger", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableSetupColumn("Log", ImGuiTableColumnFlags_WidthFixed);
      CherryGUI::TableHeadersRow();
      for (int i = 0; i < logs.size(); i++) {
        CherryGUI::TableNextRow();
        for (int column = 0; column < 4; column++) {
          CherryGUI::TableSetColumnIndex(column);
          if (column == 0) {
            if (logs[i]->level == spdlog::level::critical) {
              CherryGUI::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Fatal");
            } else if (logs[i]->level == spdlog::level::err) {
              CherryGUI::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error");
            } else if (logs[i]->level == spdlog::level::warn) {
              CherryGUI::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Warning");
            } else if (logs[i]->level == spdlog::level::info) {
              CherryGUI::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Information");
            }
          } else if (column == 1) {
            CherryGUI::Text(logs[i]->timestamp.c_str());
          } else if (column == 2) {
            CherryGUI::Text(logs[i]->filter.c_str());
          } else if (column == 3) {
            CherryGUI::TextWrapped(logs[i]->message.c_str());
          }
        }
      }
      CherryGUI::EndTable();
    }
  }
}

void ModuleDetails::menubar() {
  if (CherryGUI::BeginMenuBar()) {
    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Details of module :\"");
    CherryGUI::Text(this->m_module->proper_name().c_str());
    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\" with name : \"");
    CherryGUI::Text(this->m_module->name().c_str());
    CherryGUI::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "\"");

    CherryGUI::EndMenuBar();
  }
}
