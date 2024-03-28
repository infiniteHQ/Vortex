// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Source/editor/EntryPoint.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../backend/Platform/GUI/editor/UI/Notification.h"
#include "../../backend/Platform/GUI/editor/UI/IconsFontAwesome6.h"
#include "../../vendor/imgui/imgui.h"
#include "../instances/instance.h"
#include "../instances/Components/Host/HostInstance.h"
#include "../instances/Components/Toolchain/ToolchainInstance.h"
#include "../core/ContentBrowser.hpp"
#include "../core/ProjectViewer.hpp"


#include "./instanceFactory.h"
using namespace VortexMaker;

// Toolchain window not appear when we activate it from Project viewer.

static std::vector<std::shared_ptr<InstanceWindow>> instanciedWindows;
static std::vector<std::shared_ptr<HostInstance>> hostInstances;
static std::vector<std::shared_ptr<ToolchainInstance>> toolchainInstances;
static std::vector<std::shared_ptr<PackageInstance>> packageInstances;
static std::vector<std::shared_ptr<TasklistInstance>> tasklistInstances;
static std::vector<std::shared_ptr<GPOSInstance>> gposInstances;
static std::vector<std::shared_ptr<ReportInstance>> reportInstances;
static std::vector<std::shared_ptr<TextEditor>> texteditorInstances;
static std::vector<std::shared_ptr<ScriptInstance>> scriptInstances;


static void PushStyle()
{
ImGuiStyle& style = ImGui::GetStyle();
ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 11.0f);
ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 11.0f);
ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, 6.0f));
ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9.0f, 3.0f));
ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7.0f);

}

static void PopStyle()
{
ImGui::PopStyleVar(8);
}

class Instance : public InstanceFactory {
  public:
  void SpawnInstance(std::shared_ptr<HostInstance> instance) override {
    instance->name = instance->host->name;
    instance->opened = true;
    hostInstances.push_back(instance);
  };

  void SpawnInstance(std::shared_ptr<ToolchainInstance> instance) override {
    instance->name = instance->toolchain->name;
    instance->opened = true;
    toolchainInstances.push_back(instance);
  };


  void SpawnInstance(std::shared_ptr<PackageInstance> instance) override {
    instance->name = instance->package->name;
    instance->opened = true;
    packageInstances.push_back(instance);
  };

  void SpawnInstance(std::shared_ptr<TasklistInstance> instance) override {
    instance->name = instance->tasklist->label;
    instance->opened = true;
    tasklistInstances.push_back(instance);
  };


  void SpawnInstance(std::shared_ptr<GPOSInstance> instance) override {
    instance->name = instance->gpos->name;
    instance->opened = true;
    gposInstances.push_back(instance);
  };

  void SpawnInstance(std::shared_ptr<ReportInstance> instance) override {
    instance->name = instance->task->id;
    instance->opened = true;
    reportInstances.push_back(instance);
  };

  void SpawnInstance(std::shared_ptr<TextEditor> instance) override {
    instance->name = VortexMaker::gen_random(6);
    instance->opened = true;
    texteditorInstances.push_back(instance);
  };

  void SpawnInstance(std::shared_ptr<ScriptInstance> instance) override {
    instance->name = instance->script->name;
    instance->opened = true;
    scriptInstances.push_back(instance);
  };


  void UnspawnInstance(std::shared_ptr<ScriptInstance> instance) override {
    std::string instanceName = instance->name;
    instance = nullptr;
  };

  void UnspawnInstance(std::shared_ptr<TextEditor> instance) override {
    std::string instanceName = instance->name;
    instance = nullptr;
  };

  void UnspawnInstance(std::shared_ptr<TasklistInstance> instance) override {
    std::string instanceName = instance->name;
    instance = nullptr;
  };

  void UnspawnInstance(std::shared_ptr<PackageInstance> instance) override {
    std::string instanceName = instance->name;
    instance = nullptr;
  };

  void UnspawnInstance(std::shared_ptr<HostInstance> instance) override {
    std::string instanceName = instance->name;
    instance = nullptr;
  };


  void UnspawnInstance(std::shared_ptr<ToolchainInstance> instance) override {
    std::string instanceName = instance->name;
    //instance->toolchain->taskProcessor->stopWorker();
    instance = nullptr;
  };

  void UnspawnInstance(std::shared_ptr<GPOSInstance> instance) override {
    std::string instanceName = instance->name;
    instance = nullptr;
  };

  void UnspawnInstance(std::shared_ptr<ReportInstance> instance) override {
    std::string instanceName = instance->name;
    instance->name = "null";
    instance = nullptr;
  };


};


class ExampleLayer : public Walnut::Layer
{
public:


  ExampleLayer(VxContext *ctx)
  {
    this->m_ctx = ctx;
  };

  virtual void OnUIRender() override
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
    // Instances
    for (auto window : hostInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}    
    for (auto window : toolchainInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    for (auto window : packageInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    for (auto window : tasklistInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    for (auto window : gposInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    for (auto window : reportInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    for (auto window : texteditorInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    for (auto window : scriptInstances){if(window->render() == "closed"){this->factory.UnspawnInstance(window);};}
    
    PopStyle();
    


  }

  void AddInstanceOfWindow(std::shared_ptr<InstanceWindow> win, std::string winName, std::shared_ptr<VxHost> host)
  {
    win->name = winName;
    win->opened = true;
    win->setup(host);
    instanciedWindows.push_back(win);
  }

  VxContext *m_ctx;


// Global TaskProcessor instance
TaskProcessor taskProcessor;

  std::thread receiveThread;
  bool ShowContentBrowser = false;
  bool ShowProjectViewer = false;
  bool ShowProjectSettings = false;

  Instance factory;
private:


  std::vector<std::string> instanciedWindowsNames;


  // To remove
  VxToolchain *m_currentToolchainForPannel;
  VxToolchain *m_currentToolchainForPannelToBuild;
  VxHost *m_currentHostForPannel;
  VxDistHost *m_currentDistHostForPannel;
  VxDistToolchain *m_currentDistToolchainForPannel;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv, VxContext *ctx)
{
  int port = atoi(argv[1]);

  Walnut::ApplicationSpecification spec;
  std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>(ctx);
  std::string name = exampleLayer->m_ctx->name + " - Vortex Editor";
  spec.Name = name;
  spec.CustomTitlebar = true;

  Walnut::Application *app = new Walnut::Application(spec);



  app->PushLayer(exampleLayer);
  app->SetMenubarCallback([app, exampleLayer, ctx]()
                          {
      static  int number=1;
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window")) {
      for(auto host : ctx->IO.hosts){
        static std::string label = "Open" + host->name;

      if (ImGui::MenuItem(label.c_str())) {
        std::shared_ptr<InstanceWindow> window = std::make_shared<InstanceWindow>(ctx);
        std::string label = "test_";
        label += number;
        number++;
        std::shared_ptr<VxHost> hostToOpen = std::make_shared<VxHost>();
        exampleLayer->AddInstanceOfWindow(window, label, hostToOpen);

      }

      }
      /*
      if (ImGui::MenuItem("CreateInstance")) {
        std::shared_ptr<InstanceWindow> window = std::make_shared<InstanceWindow>(ctx);
        std::string label = "test_";
        label += number;
        number++;
        exampleLayer->AddInstanceOfWindow(window, label, host);
      }*/
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools")) {

            if (ImGui::MenuItem("Content Browser", "Project file manager", &exampleLayer->ShowContentBrowser))
            {
            }
            if (ImGui::MenuItem("Project Viewer", "Project component manager", &exampleLayer->ShowProjectViewer))
            {
            }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    } 
    

    
    });

  return app;
}

// Faire plutot un combo Registered / Active / Innactive / Desactiverd / OnError
// et des types (matrix, provider etc)
// possibilité de filtrer