#include "../../../../lib/uikit/src/EntryPoint.h"
#include "../../../../main/include/vortex.h"
#include "../../../../main/include/modules/load.h"
#include "../core/ProjectManager.hpp"
#include "../core/SystemSettings.hpp"
#include "../core/LauncherLogUtility.hpp"
#include "../core/Details.hpp"
using namespace VortexMaker;

#ifndef LAUNCHER_H
#define LAUNCHER_H

class LauncherLayer : public UIKit::Layer
{
public:
  LauncherLayer(){
    this->LayerName = "LauncherLayer";
  };

  void menubar(const std::shared_ptr<LauncherLayer> &applayer, UIKit::Application *app);

  void OnUpdate(float timestep) override
  {

  }

  void OnUIRender() override
  {
    PushStyle();

    ImGui::ShowDemoWindow();

    if (this->ShowProjectManager)
    {
      static ProjectManager projectManager(this->m_ctx, this->ParentWindow);
      projectManager.OnImGuiRender(this->ParentWindow, this->m_WindowControlCallbalck);
    } 

    if (this->ShowLogUtility)
    {
      static LauncherLogUtility logUtility(this->m_ctx, this->ParentWindow);
      logUtility.OnImGuiRender(this->ParentWindow, this->m_WindowControlCallbalck);
    } 
    
    if (this->ShowSystemSettings)
    {
      static SystemSettings systemSettings(this->m_ctx, this->ParentWindow);
      systemSettings.OnImGuiRender(this->ParentWindow, this->m_WindowControlCallbalck);
    } 

    static Details details(this->m_ctx, this->ParentWindow);
    details.OnImGuiRender(this->ParentWindow, this->m_WindowControlCallbalck);

    PopStyle();
  }

  VxContext *m_ctx;

  void PushStyle()
  {
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 11.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 11.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, 7.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7.0f);
  }

  void PopStyle()
  {
    ImGui::PopStyleVar(8);
  }

  // Global TaskProcessor instance
  // TaskProcessor taskProcessor;

  std::thread receiveThread;
  bool ShowContentBrowser = false;
  bool ShowProjectViewer = false;
  bool ShowModulesManager = false;
  bool ShowProjectManager = true;
  bool ShowLogUtility = true;
  bool ShowSystemSettings = false;

private:
  std::vector<std::string> instanciedWindowsNames;
};

static UIKit::Application *CreateLauncher(int argc, char **argv)
{
  int port = atoi(argv[1]);

  std::shared_ptr<LauncherLayer> applayer = std::make_shared<LauncherLayer>();
  applayer->m_ctx = VortexMaker::GetCurrentContext();


  VortexMaker::LoadSystemModules(applayer->m_ctx->IO.sys_em);

  UIKit::ApplicationSpecification spec;
  std::string name = "Vortex Launcher";
  spec.Name = name;
  spec.Width = 1550;
  spec.Height = 850;
  spec.Name = name;
  spec.CustomTitlebar = true;
  spec.IconPath = "icon.png";

  UIKit::Application *app = new UIKit::Application(spec);

  app->PushLayer(applayer);
  app->SetMenubarCallback([app, applayer]()
                          { applayer->menubar(applayer, app); });

  return app;
}

namespace VortexMaker
{
  static int VortexLauncher(int argc, char **argv)
  {
    return UIKit::ThirdMain(argc, argv, CreateLauncher);
  }
}

#endif