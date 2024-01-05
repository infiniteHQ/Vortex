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
    ExampleLayer(VxContext* ctx) {this->m_ctx = ctx;};

    virtual void OnUIRender() override
    {

        // ImGui::Begin("Hello");
        // ImGui::Button("Button");
        // ImGui::End();

        //ImGui::ShowDemoWindow();

        // Elementsy
        UI_ShowContextToolchains(); // change to active label
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


    void UI_ShowContextToolchains()
    {
        if (!m_UI_ShowContextToolchains)
            return;

        if (ImGui::Begin("Actives Matrixes"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("gdfgh", 4, flags))
                {
                    ImGui::TableSetupColumn("Pannel", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.toolchains.size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 3; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open ";
                                label += m_ctx->IO.toolchains[i].name.c_str();

                                if (ImGui::Button((char*)label.c_str()))
                                {
                                    //this->ShowMatrixPannel(m_ctx->IO.GetAllActiveMatrixes()[i]);
                                };
                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].name.c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].author.c_str());
                            }
                            if (column == 3)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].type.c_str());
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            
         

            ImGui::Columns(1); // Reviens à une seule colonne

            // Redimensionnement automatique de la hauteur entre les deux enfants
            float minHeight = ImGui::GetContentRegionAvail().y;                       // Hauteur minimale
            float maxHeight = ImGui::GetIO().DisplaySize.y - ImGui::GetCursorPos().y; // Hauteur maximale

            if (minHeight > maxHeight)
            {
                minHeight = maxHeight;
            }
        }
        ImGui::End();
    }


    void ShowContextToolchains() { m_UI_ShowContextToolchains = true; }

    VxContext* m_ctx;

private:
    bool m_UI_ShowContextToolchains = false;
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
    app->SetMenubarCallback([app, exampleLayer]()
    {
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

    if (ImGui::BeginMenu("Content")) {
      if (ImGui::MenuItem("Toolchains")) {
        exampleLayer->ShowContextToolchains();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window")) {
      if (ImGui::MenuItem("Contents Window")) {
        
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
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