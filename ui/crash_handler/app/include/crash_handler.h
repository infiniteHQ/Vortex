#include "../../../../main/include/vortex.h"
#include "../../../../main/include/vortex_internals.h"
#include "../../../../lib/cherry/cherry.hpp"

using namespace VortexMaker;

#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

class CrashHandlerLayer : public Cherry::Layer
{
public:
  CrashHandlerLayer(){
    this->LayerName = "CrashHandlerLayer";
  };

  //void menubar(const std::shared_ptr<CrashHandlerLayer> &applayer, Cherry::Application *app);

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

static Cherry::Application *CreateCrashHandler(int argc, char **argv)
{
  int port = atoi(argv[1]);

  std::shared_ptr<CrashHandlerLayer> applayer = std::make_shared<CrashHandlerLayer>();
  applayer->m_ctx = VortexMaker::GetCurrentContext();

  Cherry::ApplicationSpecification spec;
  std::string name = "Vortex crash reporter";
  spec.Name = name;
  spec.Width = 800;
  spec.Height = 650;
  spec.Name = name;
  spec.WindowOnlyClosable = true;
  spec.CustomTitlebar = true;
  spec.IconPath = "icon.png";

  Cherry::Application *app = new Cherry::Application(spec);

  app->PushLayer(applayer);
  
  return app;
}

namespace VortexMaker
{
  static int VortexCrashHandler(int argc, char **argv)
  {
    return Cherry::ThirdMain(argc, argv, CreateCrashHandler);
  }
}

#endif