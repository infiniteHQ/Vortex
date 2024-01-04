// UI (with imgui)
#include "../../backend/Source/Devtools/Application.h"
#include "../../backend/Source/Devtools/EntryPoint.h"
#include "../../backend/Platform/GUI/Devtools/Image.h"
#include "../../backend/Platform/GUI/Devtools/UI/UI.h"
#include "../../vendor/imgui/imgui.h"

using namespace VortexMaker;


// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
    ImGuiStyle &style = ImGui::GetStyle();
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
    ExampleLayer() { };

    virtual void OnUIRender() override
    {

        // ImGui::Begin("Hello");
        // ImGui::Button("Button");
        // ImGui::End();

        //ImGui::ShowDemoWindow();

        // Elementsy
        //UI_ShowContextMatrixes(); // change to active label
    }

    // Fonction pour obtenir la couleur en fonction de la valeur de l'entier
    ImU32 GetColorForValue(int value)
    {
        // Calculer les composantes de couleur en fonction de la valeur
        int red = 255 - (value * 255 / 1500);
        int green = value * 255 / 1500;
        int blue = 255;

        // Assurez-vous que les composantes sont comprises entre 0 et 255
        red = (red < 0) ? 0 : (red > 255) ? 255
                                          : red;
        green = (green < 0) ? 0 : (green > 255) ? 255
                                                : green;
        blue = (blue < 0) ? 0 : (blue > 255) ? 255
                                             : blue;

        // Créez la couleur ImGui avec les composantes calculées
        return IM_COL32(red, green, blue, 255);
    }

    //void ShowContextMatrixes() { m_UI_ShowContextMatrixes = true; }

private:
    //bool m_UI_ShowContextMatrixes = false;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv, VxContext* ctx)
{
    int port = atoi(argv[1]);

    Walnut::ApplicationSpecification spec;
    spec.Name = "Vortex";
    spec.CustomTitlebar = false;

    Walnut::Application *app = new Walnut::Application(spec);
    std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();

    app->PushLayer(exampleLayer);
    app->SetMenubarCallback([app, exampleLayer]()
    {
    if (ImGui::BeginMenu("O")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Build")) {
    ImGui::Text("Build all project now");
      if (ImGui::MenuItem("Build all")) {
      }
      
    ImGui::Text("Build only parts");
      if (ImGui::MenuItem("Build toolchain(s)")) {
      }
      if (ImGui::MenuItem("Build package(s)")) {
      }
      if (ImGui::MenuItem("Build other...")) {
      }

      ImGui::EndMenu();
    }
    });
    
    return app;
}

// Faire plutot un combo Registered / Active / Innactive / Desactiverd / OnError
// et des types (matrix, provider etc)
// possibilité de filtrer