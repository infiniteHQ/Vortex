//
//  crash_handler.hpp
//  Headers, main classes for the crash handler GUI window
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <memory>
#include <thread>

#include "../../lib/cherry/cherry.hpp"
#include "src/static/crash_report/crash_report.hpp"

static std::string global_session_id = "unknow";

namespace vxe {
  int VortexCrash(int argc, char **argv);
}

class CrashHandler {
 public:
  CrashHandler() {
    crash_handler = CrashReport::create("Crash report", global_session_id);
    Cherry::AddAppWindow(crash_handler->get_app_window());
  };

 private:
  std::shared_ptr<CrashReport> crash_handler;
};

static std::shared_ptr<CrashHandler> c_CrashHandler;

Cherry::Application *CreateCrash(int argc, char **argv) {
  Cherry::ApplicationSpecification spec;
  std::string name = "Vortex Crash Reporter";
  spec.Name = name;
  spec.MinHeight = 500;
  spec.MinWidth = 500;
  spec.Height = 650;
  spec.Width = 800;
  spec.CustomTitlebar = true;
  spec.DisableWindowManagerTitleBar = true;
  spec.WindowOnlyClosable = true;
  spec.DisableResize = true;
  spec.RenderMode = Cherry::WindowRenderingMethod::DockingWindows;

  spec.DisableTitle = true;
  spec.WindowSaves = false;
  spec.IconPath = Cherry::GetPath("ch_resources/imgs/icon_crash.png");

  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::GetPath("ch_resources/imgs/icon_crash.png"));
  app->AddFont("Consola", Cherry::GetPath("ch_resources/fonts/consola.ttf"), 17.0f);
  app->AddFont("JetBrainsMono", Cherry::Application::CookPath("resources/fonts/JetBrainsMono-Regular.ttf"), 40.0f);

  app->AddLocale("fr", Cherry::GetPath("ch_resources/locales/fr.json"));
  app->AddLocale("en", Cherry::GetPath("ch_resources/locales/en.json"));
  app->SetLocale("en");
  app->SetDefaultLocale("en");

  c_CrashHandler = std::make_shared<CrashHandler>();
  return app;
}

int vxe::VortexCrash(int argc, char **argv) {
  return Cherry::ThirdMain(argc, argv, CreateCrash);
}