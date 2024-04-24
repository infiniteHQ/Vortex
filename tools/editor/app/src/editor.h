
#include "../instances/instance.h"
#include "../core/ContentBrowser.hpp"
#include "../core/ProjectViewer.hpp"
#include "../core/ModuleManager.hpp"

#include "./instanceFactory.h"
using namespace VortexMaker;

static std::vector<std::shared_ptr<ModuleDetails>> moduleDetailsInstances;

static void PushStyle()
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

static void PopStyle()
{
  ImGui::PopStyleVar(8);
}

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

class ExampleLayer : public UIKit::Layer
{
public:
  ExampleLayer(){};

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

    if (this->ShowModulesManager)
    {
      static ModuleManager moduleManager(this->m_ctx, &factory);
      moduleManager.OnImGuiRender();
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

    PopStyle();
  }

  VxContext *m_ctx;

  // Global TaskProcessor instance
  // TaskProcessor taskProcessor;

  std::thread receiveThread;
  bool ShowContentBrowser = false;
  bool ShowProjectViewer = false;
  bool ShowModulesManager = false;
  bool ShowProjectSettings = false;

  Instance factory;

private:
  std::vector<std::string> instanciedWindowsNames;
};

UIKit::Application *UIKit::CreateApplication(int argc, char **argv)
{
  int port = atoi(argv[1]);

  UIKit::ApplicationSpecification spec;
  std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
  exampleLayer->m_ctx = VortexMaker::GetCurrentContext();
  std::string name = exampleLayer->m_ctx->name + " - Vortex Editor";
  spec.Name = name;
  spec.CustomTitlebar = true;
  spec.IconPath = "icon.png";

  UIKit::Application *app = new UIKit::Application(spec);

  app->PushLayer(exampleLayer);
  app->SetMenubarCallback([app, exampleLayer]()
                          {
                            static int number = 1;
                            if (ImGui::BeginMenu("File"))
                            {
                              if (ImGui::MenuItem("Exit"))
                              {
                                app->Close();
                              }
                              ImGui::EndMenu();
                            }
                            if (ImGui::BeginMenu("Edit"))
                            {
                              if (ImGui::MenuItem("Project Settings", "Main configurations of this project"))
                              {
                                app->Close();
                              }
                              ImGui::Separator();
                              if (ImGui::MenuItem("Manage plugins", "Add, remove, edit plugins of this project"))
                              {
                                app->Close();
                              }
                              if (ImGui::MenuItem("Manage modules", "Project file manager", &exampleLayer->ShowModulesManager))
                              {
                              }
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Window"))
                            {
                              if (ImGui::MenuItem("Show bottom toolbar", "Get some usefull tools in a bottom bar."))
                              {
                                app->Close();
                              }
                              if (ImGui::MenuItem("Show simplified header", "Reduce the size of header"))
                              {

                                app->m_Specification.CustomTitlebar = !app->m_Specification.CustomTitlebar;
                              }

                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Tools"))
                            {

                              if (ImGui::MenuItem("Content Browser", "Project file manager", &exampleLayer->ShowContentBrowser))
                              {
                              }
                              if (ImGui::MenuItem("Project Viewer", "Project component manager", &exampleLayer->ShowProjectViewer))
                              {
                              }
                              ImGui::EndMenu();
                            }

                            if (ImGui::BeginMenu("Help"))
                            {
                              if (ImGui::MenuItem("News", "Get latest Vortex news"))
                              {
                                app->Close();
                              }
                              if (ImGui::MenuItem("Community", "Join a community of creators"))
                              {
                                app->Close();
                              }
                              if (ImGui::MenuItem("Tutorials", "Get bunch of tutorials"))
                              {
                                app->Close();
                              }
                              if (ImGui::MenuItem("Documentation", "See official documentation of Vortex Maker"))
                              {
                                app->Close();
                              }
                              ImGui::Separator();
                              if (ImGui::MenuItem("Exit"))
                              {
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