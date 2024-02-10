// UI (with imgui)
#include "../../backend/Source/editor/Application.h"
#include "../../backend/Source/editor/EntryPoint.h"
#include "../../backend/Platform/GUI/editor/Image.h"
#include "../../backend/Platform/GUI/editor/UI/UI.h"
#include "../../vendor/imgui/imgui.h"
#include "../instances/instance.h"
#include "../core/ContentBrowser.hpp"

#include <thread>

using namespace VortexMaker;


// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 1.30f)));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
    ImGui::PopStyleVar(2);
}

class ExampleLayer : public Walnut::Layer
{
public:
    ExampleLayer(VxContext* ctx) {
      this->m_ctx = ctx;

    
    };

    virtual void OnUIRender() override
    {

        ImGui::ShowDemoWindow();
        

        // Instances
        for(auto window : instanciedWindows){
          window->render();
        }
    }
    
    void AddInstanceOfWindow(std::shared_ptr<InstanceWindow> win, std::string winName, std::shared_ptr<VxHost> host){
      win->name = winName;
      win->setup(host);
      this->instanciedWindows.push_back(win);
    }

    VxContext* m_ctx;


    std::thread receiveThread;

private:
    bool m_UI_ShowContextToolchains = false;

    std::vector<std::shared_ptr<InstanceWindow>> instanciedWindows;
    std::vector<std::string> instanciedWindowsNames;
    

        ContentBrowserPanel pannel;

    // To remove 
    VxToolchain *m_currentToolchainForPannel;
    VxToolchain *m_currentToolchainForPannelToBuild;
    VxHost *m_currentHostForPannel;
    VxDistHost *m_currentDistHostForPannel;
    VxDistToolchain *m_currentDistToolchainForPannel;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv, VxContext* ctx)
{
    int port = atoi(argv[1]);

    Walnut::ApplicationSpecification spec;
    std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>(ctx);
    std::string name = exampleLayer->m_ctx->name + " - Vortex Editor";
    spec.Name = name;
    spec.CustomTitlebar = false;


    Walnut::Application *app = new Walnut::Application(spec);


    app->PushLayer(exampleLayer);
    app->SetMenubarCallback([app, exampleLayer, ctx]()
    {
    ImGuiStyle &style = ImGui::GetStyle();
    
		style.FrameRounding = 5.0f;
		style.FrameBorderSize = 1.0f;
		style.IndentSpacing = 11.0f;
		style.ScrollbarSize = 11.0f;
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(15.0f, 6.0f);
		style.ItemSpacing = ImVec2(9.0f, 3.0f);
		style.TabRounding = 7.0f;

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
        static std::string label = "Open" + host.name;

      if (ImGui::MenuItem(label.c_str())) {
        std::shared_ptr<InstanceWindow> window = std::make_shared<InstanceWindow>(ctx);
        std::string label = "test_";
        label += number;
        number++;
        std::shared_ptr<VxHost> hostToOpen = std::make_shared<VxHost>(host);
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
      if (ImGui::MenuItem("Contents Window")) {
        
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