#include "../instances/instance.h"
#include "../core/ContentBrowser.hpp"
#include "../core/ProjectViewer.hpp"
#include "../core/LogUtility.hpp"
#include "../core/ModuleManager.hpp"
#include "../core/TemplateManager.hpp"
#include "../core/ProjectSettings.hpp"

#include "./instanceFactory.h"

#include "../../../../lib/uikit/src/EntryPoint.h"

using namespace VortexMaker;

#ifndef EDITOR_H
#define EDITOR_H

static std::vector<std::shared_ptr<ModuleDetails>> moduleDetailsInstances;

class Instance : public InstanceFactory
{
public:
  void SpawnInstance(std::shared_ptr<ModuleDetails> instance) override
  {
    instance->name = instance->m_module->m_name;
    instance->opened = true;
    moduleDetailsInstances.push_back(instance);
  };

  void UnspawnInstance(std::shared_ptr<ModuleDetails> instance) override
  {
    std::string instanceName = instance->name;
    instance = nullptr;
  };
};

class EditorLayer : public UIKit::Layer
{
public:
  EditorLayer(){};

  void menubar(const std::shared_ptr<EditorLayer> &applayer, UIKit::Application *app);

  void OnFinish() override
  {
    
  }

  void OnUIRender() override
  {
    PushStyle();

    ImGui::ShowDemoWindow();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.00f));
    ImGui::RenderNotifications();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);

    if (this->ShowContentBrowser)
    {
      static ContentBrowserPanel contentBrowser;
      contentBrowser.OnImGuiRender();
    }

    if (this->ShowProjectViewer)
    {
      static ProjectViewer projectViewer(this->m_ctx, &factory);
      projectViewer.OnImGuiRender();
    }

    if (this->ShowProjectSettings)
    {
      static ProjectSettings projectSettings(this->m_ctx, &factory);
      projectSettings.OnImGuiRender();
    }

    if (this->ShowModulesManager)
    {
      static ModuleManager moduleManager(this->m_ctx, &factory);
      moduleManager.OnImGuiRender();
    }

    if (this->ShowTemplateManager)
    {
      static TemplateManager templateManager(this->m_ctx, &factory);
      templateManager.OnImGuiRender();
    }

    if (this->ShowLogUtility)
    {
      static LogUtility logUtility(this->m_ctx, &factory);
      logUtility.OnImGuiRender();
    }


    for (auto window : moduleDetailsInstances)
    {
      if (window->render() == "closed")
      {
        this->factory.UnspawnInstance(window);
      };
    }

    // Modules rendering
    for (auto em : this->m_ctx->IO.em)
    {
      if (em->m_state == "running")
      {
        // Main renderer channel
        em->render();

        // Subinstances renderers
        for (auto instance : em->GetModuleRenderInstances())
        {
          instance->render();
        }
      }
    }

    std::cout << this->m_ctx->registered_logs.size() << std::endl;

    PopStyle();
  }
  void PushStyle()
  {
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 11.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 11.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7.0f);
  }

  void PopStyle()
  {
    ImGui::PopStyleVar(8);
  }
  VxContext *m_ctx;

  // Global TaskProcessor instance
  // TaskProcessor taskProcessor;

  std::thread receiveThread;
  bool ShowContentBrowser = false;
  bool ShowProjectViewer = false;
  bool ShowModulesManager = false;
  bool ShowTemplateManager = false;
  bool ShowProjectSettings = false;
  bool ShowLogUtility = false;

  Instance factory;

private:
  std::vector<std::string> instanciedWindowsNames;
};


namespace VortexMaker
{
   int VortexEditor(int argc, char **argv);

  // Old call
  /*static int VortexEditor(int argc, char **argv)
  {
    return UIKit::Main(argc, argv);
  }*/
}

// Old creation function
/*static UIKit::Application *UIKit::CreateApplication(int argc, char **argv)
{
  int port = atoi(argv[1]);

  std::shared_ptr<EditorLayer> applayer = std::make_shared<EditorLayer>();
  applayer->m_ctx = VortexMaker::GetCurrentContext();

  UIKit::ApplicationSpecification spec;
  std::string name = applayer->m_ctx->name + " - Vortex Editor";
  spec.Name = name;
  spec.CustomTitlebar = true;
  spec.IconPath = "icon.png";

  UIKit::Application *app = new UIKit::Application(spec);

  app->PushLayer(applayer);
  app->SetMenubarCallback([app, applayer]()
                          { applayer->menubar(applayer, app); });

  return app;
}*/

#endif