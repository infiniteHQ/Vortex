#include "../../../../lib/uikit/src/EntryPoint.h"
#include "../../../../main/include/vortex.h"
#include "../core/Crash.hpp"
using namespace VortexMaker;

#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

class CrashHandlerLayer : public UIKit::Layer
{
public:
  CrashHandlerLayer(){
    this->LayerName = "CrashHandlerLayer";
  };

  void menubar(const std::shared_ptr<CrashHandlerLayer> &applayer, UIKit::Application *app);

  void OnUpdate(float timestep) override
  {
    
  }

  void OnUIRender() override
  {
    PushStyle();

      static Crash details(this->m_ctx, this->ParentWindow);
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
  bool ShowSystemSettings = false;

private:
  std::vector<std::string> instanciedWindowsNames;
};

static UIKit::Application *CreateCrashHandler(int argc, char **argv)
{
  int port = atoi(argv[1]);

  std::shared_ptr<CrashHandlerLayer> applayer = std::make_shared<CrashHandlerLayer>();
  applayer->m_ctx = VortexMaker::GetCurrentContext();

  UIKit::ApplicationSpecification spec;
  std::string name = "Vortex crash reporter";
  spec.Name = name;
  spec.Width = 800;
  spec.Height = 650;
  spec.Name = name;
  spec.WindowOnlyClosable = true;
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
  static int VortexCrashHandler(int argc, char **argv)
  {
    return UIKit::ThirdMain(argc, argv, CreateCrashHandler);
  }
}

#endif