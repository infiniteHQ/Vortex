#pragma once
#define CHERRY_V1
#include "../../lib/cherry/cherry.hpp"

#include "src/static/crash_report/crash_report.hpp"
#include <thread>
#include <memory>

namespace VortexMaker
{
   int VortexCrash(int argc, char **argv);
}

class CrashHandlerLayer : public Cherry::Layer
{
public:
  CrashHandlerLayer() {};
};

class CrashHandler
{
public:
  CrashHandler()
  {
    // Render static windows
    crash_handler = std::make_shared<Crash>("Crash report");
    crash_handler->RefreshRender(crash_handler);
    Cherry::AddWindow(crash_handler->GetAppWindow()); // Publish this window into the workspace
  };

private:
  std::shared_ptr<Crash> crash_handler;
};

static std::shared_ptr<CrashHandler> c_CrashHandler;

Cherry::Application *CreateCrash(int argc, char **argv)
{
  Cherry::ApplicationSpecification spec;
  std::shared_ptr<CrashHandlerLayer> layer = std::make_shared<CrashHandlerLayer>();

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
  spec.IconPath = Cherry::GetPath("resources/imgs/icon_crash.png");

  Cherry::Application *app = new Cherry::Application(spec);
  app->SetFavIconPath(Cherry::GetPath("resources/imgs/icon_crash.png"));
  app->AddFont("Consola", Cherry::GetPath("resources/fonts/consola.ttf"), 17.0f);

  app->AddLocale("fr", Cherry::GetPath("resources/locales/fr.json"));
  app->AddLocale("en", Cherry::GetPath("resources/locales/en.json"));
  app->SetDefaultLocale("en");
  app->SetLocale("fr");

  app->PushLayer(layer);
  app->SetMenubarCallback([app, layer]()
                          {
                            ImVec4 grayColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        
                            ImVec4 graySeparatorColor = ImVec4(0.4f, 0.4f, 0.4f, 0.5f);
                            ImVec4 darkBackgroundColor = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
                            ImVec4 lightBorderColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);       

                            ImGui::PushStyleColor(ImGuiCol_PopupBg, darkBackgroundColor);

                            ImGui::PushStyleColor(ImGuiCol_Border, lightBorderColor);

                            ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);

                            static bool t;

                            if (ImGui::BeginMenu("Options"))
                            {
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Settings");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);


                              if(ImGui::Button("Set en"))
                              {
  app->SetLocale("en");
                              }

                              if(ImGui::Button("Set fr"))
                              {
  app->SetLocale("fr");
                              }

                              if(ImGui::Button("Set es"))
                              {
  app->SetLocale("es");
                              }
                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem("Logs Utility", "Overview of all logs", &t))
                              {
                              }

                              ImGui::GetFont()->Scale *= 0.8;
                              ImGui::PushFont(ImGui::GetFont());

                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                              ImGui::PushStyleColor(ImGuiCol_Text, grayColor);
                              ImGui::Text("Main stuff");
                              ImGui::PopStyleColor();

                              ImGui::PushStyleColor(ImGuiCol_Separator, graySeparatorColor);
                              ImGui::Separator();
                              ImGui::PopStyleColor();

                              ImGui::GetFont()->Scale = 0.84;
                              ImGui::PopFont();
                              ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

                              if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
                              {
                              }

                              if (ImGui::MenuItem("Manage modules", "Manage modules loaded/registered", &t))
                              {
                              }

                              if (ImGui::MenuItem("Templates modules", "Create, add template in your project", &t))
                              {
                              }

                              ImGui::EndMenu();
                            }

                            ImGui::PopStyleVar();  
                            ImGui::PopStyleColor(2); });

  c_CrashHandler = std::make_shared<CrashHandler>();
  return app;
}

int VortexMaker::VortexCrash(int argc, char **argv)
{
    return Cherry::ThirdMain(argc, argv, CreateCrash);
}