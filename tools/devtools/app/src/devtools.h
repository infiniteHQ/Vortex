// UI (with imgui)
#include "../../backend/Source/Devtools/Application.h"
#include "../../backend/Source/Devtools/EntryPoint.h"
#include "../../backend/Platform/GUI/Devtools/Image.h"
#include "../../backend/Platform/GUI/Devtools/UI/UI.h"
#include "../../vendor/imgui/imgui.h"

#include <thread>

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

        ImGui::ShowDemoWindow();

        // Elementsy
        UI_ShowContextToolchains(); // change to active label
        UI_ShowContextHosts();
        UI_ShowContextToolchainsTobuild(); // change to active label
        UI_ShowContextDistToolchains();
        UI_ShowContextDistHost();

        UI_ShowToolchainPannel();       
        UI_ShowHostPannel();         
        UI_ShowToolchainBuildPannel();
        UI_ShowDistHostPannel();
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


    void ShowToolchainPannel(VxToolchain *toolchain)
    {
        this->m_currentToolchainForPannel = toolchain;
        m_UI_ShowToolchainPannel = true;
    }

    void ShowHostPannel(VxHost *host)
    {
        this->m_currentHostForPannel = host;
        m_UI_ShowHostPannel = true;
    }

    void BuildToolchain(){

              this->m_currentToolchainForPannel->PreBuild();
              this->m_currentToolchainForPannel->Build();
              this->m_currentToolchainForPannel->PostBuild();
    }


    void BuildHost(){

              this->m_currentHostForPannel->PreBuild();
              this->m_currentHostForPannel->Build();
              this->m_currentHostForPannel->PostBuild();
    }


    void ShowToolchainBuildPannel(VxToolchain *toolchain)
    {
        this->m_currentToolchainForPannelToBuild = toolchain;
        m_UI_ShowToolchainBuildPannel = true;
    }


    void ShowDistHostPannel(VxDistHost *host)
    {
        this->m_currentDistHostForPannel = host;
        m_UI_ShowDistHostPannel = true;
    }

    void UI_ShowToolchainBuildPannel()
    {
        if (!m_UI_ShowToolchainBuildPannel)
            return;


        if (ImGui::Begin("Toolchain Build Pannel"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.5;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Toolchain :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentToolchainForPannelToBuild->name.c_str());
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentToolchainForPannelToBuild->author.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "State :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentToolchainForPannelToBuild->state.c_str());
        }
        ImGui::End();
    }

    void UI_ShowDistHostPannel()
    {
        if (!m_UI_ShowDistHostPannel)
            return;

        if (ImGui::Begin("Dist Host Pannel"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.5;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Dist Host :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentDistHostForPannel->name.c_str());
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentDistHostForPannel->author.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "State :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentDistHostForPannel->state.c_str());

            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MatrixPannelTabBar", tab_bar_flags))
            {
                 if(ImGui::BeginTabItem("Testing")){
                    if(ImGui::Button("Testing Chroot")){
                        this->m_currentDistHostForPannel->TestingChroot();
                    }
                    ImGui::EndTabItem();
                }
            ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void UI_ShowHostPannel()
    {
        if (!m_UI_ShowHostPannel)
            return;

        if (ImGui::Begin("Host Pannel"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.5;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Host :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentHostForPannel->name.c_str());
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentHostForPannel->author.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "State :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentHostForPannel->state.c_str());

            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MatrixPannelTabBar", tab_bar_flags))
            {
                std::string registeredpackages_title = "Registered Packages (";
                registeredpackages_title += std::to_string(m_currentHostForPannel->registeredPackages.size());
                registeredpackages_title += ")";
                if (ImGui::BeginTabItem((char*)registeredpackages_title.c_str()))
                {
                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    static ImGuiTableFlags flagsd =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;


                    if (ImGui::BeginTable("RegisteredPackages", 3, flagsd))
                    {
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Resolved", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (auto package : m_currentHostForPannel->registeredPackages)
                        { 
                            
                            ImGui::TableNextRow();

                            for (int column = 0; column < 2; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)package->label.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)package->emplacement.c_str());
                                }
                                if (column == 2)
                                {
                                  if(package->resolved){
                                    ImGui::Text("Resolved");
                                  }else{
                                    ImGui::Text("Not resolved...");

                                  }
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                
                std::string elements_title = "Packages (";
                elements_title += std::to_string(m_currentHostForPannel->packages.size());
                elements_title += ")";
                if (ImGui::BeginTabItem((char*)elements_title.c_str()))
                {
                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    static ImGuiTableFlags flagsd =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;


                    if (ImGui::BeginTable("Packages", 5, flagsd))
                    {
                        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Runtime", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (auto package : m_currentHostForPannel->packages)
                        { 
                            
                            ImGui::TableNextRow();

                            for (int column = 0; column < 5; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                  if(ImGui::Button("Open")){

                                  }
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)package->label.c_str());
                                }
                                if (column == 2)
                                {
                                    //ImGui::Text((char *)element->attached_matrix->GetId().c_str());
                                }
                                if (column == 3)
                                {
                                }
                                if (column == 4)
                                {
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                
              if(ImGui::BeginTabItem("Scripts")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Patchs")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Modules")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Compilation")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Build")){

            if(ImGui::Button("Build")){
            std::thread Thread([&]() { this->BuildHost();});
            this->receiveThread.swap(Thread);
            }


            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

            for(auto package : m_currentHostForPannel->packages){
              std::string label = "Package : " + package->label;
             if (ImGui::TreeNode((char*)label.c_str()))
            {
                std::string label;

                for(auto action : package->actions){
                ImGui::SetNextItemOpen(false, ImGuiCond_Once);

                std::string ddname = "action-" + action->type + "-" + action->executionSequence + "-" + std::to_string(action->priority);
                if(package->GetDiagCode(ddname) == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += ddname + " [Success]";
                }
                else if(package->GetDiagCode(ddname) == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += ddname + " [Wait]";
                }
                else if(package->GetDiagCode(ddname) == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += ddname + " [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += ddname + " [Fail]";

                }


                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput(ddname).c_str());
                    ImGui::TreePop();
                }
                ddname.clear();
                ImGui::PopStyleColor();


                }

                ImGui::SetNextItemOpen(false, ImGuiCond_Once);

                if(package->GetDiagCode("decompression") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Decompression [Success]";
                }
                else if(package->GetDiagCode("decompression") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Decompression [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Decompression [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Decompression [Fail]";

                }

                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("decompression").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();




                if(package->GetDiagCode("configuration") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Configuration [Success]";
                }
                else if(package->GetDiagCode("configuration") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Configuration [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Configuration [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Configuration [Fail]";

                }

                ImGui::SetNextItemOpen(false, ImGuiCond_Once);
                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("configuration").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();


                if(package->GetDiagCode("compilation") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Compilation [Success]";
                }
                else if(package->GetDiagCode("compilation") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Compilation [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Compilation [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Compilation [Fail]";

                }
                ImGui::SetNextItemOpen(false, ImGuiCond_Once);
                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("compilation").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();

                if(package->GetDiagCode("installation") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Installation [Success]";
                }
                else if(package->GetDiagCode("installation") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Installation [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Installation [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Installation [Fail]";

                }
                ImGui::SetNextItemOpen(false, ImGuiCond_Once);
                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("installation").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();
            ImGui::TreePop();
            }
        }
        
                ImGui::EndTabItem();
              }
              ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void UI_ShowToolchainPannel()
    {
        if (!m_UI_ShowToolchainPannel)
            return;

        if (ImGui::Begin("Toolchain Pannel"))
        {
            float oldsize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 1.5;
            ImGui::PushFont(ImGui::GetFont());

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Toolchain :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentToolchainForPannel->name.c_str());
            ImGui::GetFont()->Scale = oldsize;
            ImGui::PopFont();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "Author :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentToolchainForPannel->author.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "State :");
            ImGui::SameLine();
            ImGui::Text((char *)this->m_currentToolchainForPannel->state.c_str());

            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MatrixPannelTabBar", tab_bar_flags))
            {
                std::string registeredpackages_title = "Registered Packages (";
                registeredpackages_title += std::to_string(m_currentToolchainForPannel->registeredPackages.size());
                registeredpackages_title += ")";
                if (ImGui::BeginTabItem((char*)registeredpackages_title.c_str()))
                {
                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    static ImGuiTableFlags flagsd =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;


                    if (ImGui::BeginTable("RegisteredPackages", 3, flagsd))
                    {
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Resolved", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (auto package : m_currentToolchainForPannel->registeredPackages)
                        { 
                            
                            ImGui::TableNextRow();

                            for (int column = 0; column < 2; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                    ImGui::Text((char *)package->label.c_str());
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)package->emplacement.c_str());
                                }
                                if (column == 2)
                                {
                                  if(package->resolved){
                                    ImGui::Text("Resolved");
                                  }else{
                                    ImGui::Text("Not resolved...");

                                  }
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                
                std::string elements_title = "Packages (";
                elements_title += std::to_string(m_currentToolchainForPannel->packages.size());
                elements_title += ")";
                if (ImGui::BeginTabItem((char*)elements_title.c_str()))
                {
                    int selected_row = -1; // Variable pour suivre la ligne sélectionnée

                    static ImGuiTableFlags flagsd =
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;


                    if (ImGui::BeginTable("Packages", 5, flagsd))
                    {
                        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableHeadersRow();

                        for (auto package : m_currentToolchainForPannel->packages)
                        { 
                            
                            ImGui::TableNextRow();

                            for (int column = 0; column < 5; column++)
                            {
                                ImGui::TableSetColumnIndex(column);

                                if (column == 0)
                                {
                                  if(ImGui::Button("Open")){

                                  }
                                }
                                if (column == 1)
                                {
                                    ImGui::Text((char *)package->label.c_str());
                                }
                                if (column == 2)
                                {
                                    //ImGui::Text((char *)element->attached_matrix->GetId().c_str());
                                }
                                if (column == 3)
                                {
                                }
                                if (column == 4)
                                {
                                    ImGui::Text((char *)std::to_string(package->priority).c_str());

                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndTabItem();
                }
                
              if(ImGui::BeginTabItem("Scripts")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Patchs")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Modules")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Compilation")){
                ImGui::EndTabItem();
              }
              if(ImGui::BeginTabItem("Build")){

            if(ImGui::Button("Build")){
            std::thread Thread([&]() { this->BuildToolchain();});
            this->receiveThread.swap(Thread);
            }

            ImGui::Separator();
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

            for(auto package : m_currentToolchainForPannel->packages){
              std::string label = "Package : " + package->label;
             if (ImGui::TreeNode((char*)label.c_str()))
            {
                std::string label;

                for(auto action : package->actions){
                ImGui::SetNextItemOpen(false, ImGuiCond_Once);

                std::string ddname = "action-" + action->type + "-" + action->executionSequence + "-" + std::to_string(action->priority);
                if(package->GetDiagCode(ddname) == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += ddname + " [Success]";
                }
                else if(package->GetDiagCode(ddname) == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += ddname + " [Wait]";
                }
                else if(package->GetDiagCode(ddname) == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += ddname + " [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += ddname + " [Fail]";

                }


                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput(ddname).c_str());
                    ImGui::TreePop();
                }
                ddname.clear();
                ImGui::PopStyleColor();


                }

                ImGui::SetNextItemOpen(false, ImGuiCond_Once);

                if(package->GetDiagCode("decompression") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Decompression [Success]";
                }
                else if(package->GetDiagCode("decompression") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Decompression [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Decompression [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Decompression [Fail]";

                }

                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("decompression").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();




                if(package->GetDiagCode("configuration") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Configuration [Success]";
                }
                else if(package->GetDiagCode("configuration") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Configuration [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Configuration [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Configuration [Fail]";

                }

                ImGui::SetNextItemOpen(false, ImGuiCond_Once);
                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("configuration").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();


                if(package->GetDiagCode("compilation") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Compilation [Success]";
                }
                else if(package->GetDiagCode("compilation") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Compilation [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Compilation [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Compilation [Fail]";

                }
                ImGui::SetNextItemOpen(false, ImGuiCond_Once);
                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("compilation").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();

                if(package->GetDiagCode("installation") == 0){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                    label.clear();
                    label += "Installation [Success]";
                }
                else if(package->GetDiagCode("installation") == 999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.6f, 0.0f, 1.0f));
                    label.clear();
                    label += "Installation [Wait]";
                }
                else if(package->GetDiagCode("decompression") == -999){
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    label.clear();
                    label += "Installation [Build not started]";
                }
                else{
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    label.clear();
                    label += "Installation [Fail]";

                }
                ImGui::SetNextItemOpen(false, ImGuiCond_Once);
                if (ImGui::TreeNode((char*)label.c_str()))
                {
                    ImGui::Text((char*)package->GetDiagOutput("installation").c_str());
                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();
            ImGui::TreePop();
            }
        }
        
                ImGui::EndTabItem();
              }
              ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void UI_ShowContextToolchainsTobuild()
    {
        if (!m_UI_ShowContextToolchainsToBuild)
            return;

        if (ImGui::Begin("Select a toolchain for build"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("RegisteredToolchains", 7, flags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Target", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.toolchains.size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 6; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                if (ImGui::Button("Build"))
                                {
                                  this->ShowToolchainBuildPannel(&m_ctx->IO.toolchains[i]);
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
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].target_arch.c_str());
                            }
                            if (column == 4)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].type.c_str());
                            }
                            if (column == 5)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].state.c_str());
                            }
                            if (column == 6)
                            {
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

    void UI_ShowContextHosts()
    {
        if (!m_UI_ShowContextHosts)
            return;

        if (ImGui::Begin("Registered Host(s)"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("RegisteredToolchains", 7, flags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Target", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.hosts.size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 6; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open";

                                if (ImGui::Button((char*)label.c_str()))
                                {
                                    this->ShowHostPannel(&m_ctx->IO.hosts[i]);
                                    //this->ShowMatrixPannel(m_ctx->IO.GetAllActiveMatrixes()[i]);
                                };
                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m_ctx->IO.hosts[i].name.c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)m_ctx->IO.hosts[i].author.c_str());
                            }
                            if (column == 3)
                            {
                                ImGui::Text((char *)m_ctx->IO.hosts[i].target_arch.c_str());
                            }
                            if (column == 4)
                            {
                                ImGui::Text((char *)m_ctx->IO.hosts[i].type.c_str());
                            }
                            if (column == 5)
                            {
                                ImGui::Text((char *)m_ctx->IO.hosts[i].state.c_str());
                            }
                            if (column == 6)
                            {
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

    void UI_ShowContextToolchains()
    {
        if (!m_UI_ShowContextToolchains)
            return;

        if (ImGui::Begin("Registered Toolchain(s)"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                if (ImGui::BeginTable("RegisteredToolchains", 7, flags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Target", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.toolchains.size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 6; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open";

                                if (ImGui::Button((char*)label.c_str()))
                                {
                                    this->ShowToolchainPannel(&m_ctx->IO.toolchains[i]);
                                    //this->ShowMatrixPannel(m_ctx->IO.GetAllActiveMatrixes()[i]);
                                };

                                if (ImGui::Button("Build"))
                                {
                                    m_ctx->IO.toolchains[i].PreBuild();
                                    m_ctx->IO.toolchains[i].Build();
                                    m_ctx->IO.toolchains[i].PostBuild();
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
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].target_arch.c_str());
                            }
                            if (column == 4)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].type.c_str());
                            }
                            if (column == 5)
                            {
                                ImGui::Text((char *)m_ctx->IO.toolchains[i].state.c_str());
                            }
                            if (column == 6)
                            {
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

    void UI_ShowContextDistToolchains()
    {
        if (!m_UI_ShowContextDistToolchains)
            return;

        if (ImGui::Begin("Registered Dist Toolchain(s)"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

                


                if (ImGui::BeginTable("RegisteredToolchains", 7, flags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Platform", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.distToolchains.size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 6; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open";

                                if (ImGui::Button((char*)label.c_str()))
                                {
                                    //this->ShowToolchainPannel(&m_ctx->IO.distToolchains[i]);
                                    //this->ShowMatrixPannel(m_ctx->IO.GetAllActiveMatrixes()[i]);
                                };

                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m_ctx->IO.distToolchains[i].name.c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)m_ctx->IO.distToolchains[i].author.c_str());
                            }
                            if (column == 3)
                            {
                                ImGui::Text((char *)m_ctx->IO.distToolchains[i].platform.c_str());
                            }
                            if (column == 4)
                            {
                                ImGui::Text((char *)m_ctx->IO.distToolchains[i].type.c_str());
                            }
                            if (column == 5)
                            {
                                ImGui::Text((char *)m_ctx->IO.distToolchains[i].state.c_str());
                            }
                            if (column == 6)
                            {
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

    void UI_ShowContextDistHost()
    {
        if (!m_UI_ShowContextDistHost)
            return;

        if (ImGui::Begin("Registered Dist Host(s)"))
        {

                static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;


                if (ImGui::BeginTable("RegisteredToolchains", 7, flags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Platform", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    //int i = 0;
                    //for (auto m : m_ctx->IO.GetAllActiveMatrixes())
                    for (int i = 0; i < m_ctx->IO.distHosts.size(); i++)
                    {

                        ImGui::TableNextRow();
                        for (int column = 0; column < 6; column++)
                        {
                            ImGui::TableSetColumnIndex(column);

                            if (column == 0)
                            {
                                std::string label = "Open";

                                if (ImGui::Button((char*)label.c_str()))
                                {
                                    this->ShowDistHostPannel(&m_ctx->IO.distHosts[i]);
                                    //this->ShowToolchainPannel(&m_ctx->IO.distToolchains[i]);
                                    //this->ShowMatrixPannel(m_ctx->IO.GetAllActiveMatrixes()[i]);
                                };

                            }
                            if (column == 1)
                            {
                                ImGui::Text((char *)m_ctx->IO.distHosts[i].name.c_str());
                            }
                            if (column == 2)
                            {
                                ImGui::Text((char *)m_ctx->IO.distHosts[i].author.c_str());
                            }
                            if (column == 3)
                            {
                                ImGui::Text((char *)m_ctx->IO.distHosts[i].platform.c_str());
                            }
                            if (column == 4)
                            {
                                ImGui::Text((char *)m_ctx->IO.distHosts[i].type.c_str());
                            }
                            if (column == 5)
                            {
                                ImGui::Text((char *)m_ctx->IO.distHosts[i].state.c_str());
                            }
                            if (column == 6)
                            {
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
    void ShowContextHosts() { m_UI_ShowContextHosts = true; }
    void ShowContextDistToolchains() { m_UI_ShowContextDistToolchains = true; }
    void ShowContextDistHost() { m_UI_ShowContextDistHost = true; }
    void ShowToolchainToBuild() { m_UI_ShowContextToolchainsToBuild = true; }
    void ShowToolchain() { m_UI_ShowToolchainPannel = true; }
    void ShowHost() { m_UI_ShowHostPannel = true; }
    void ShowDistHost() { m_UI_ShowDistHostPannel = true; }

    VxContext* m_ctx;


    std::thread receiveThread;

private:
    bool m_UI_ShowContextToolchains = false;
    bool m_UI_ShowToolchainPannel = false;
    bool m_UI_ShowToolchainBuildPannel = false;
    bool m_UI_ShowContextToolchainsToBuild = false;
    bool m_UI_ShowContextDistToolchains = false;
    bool m_UI_ShowContextDistHost = false;
    bool m_UI_ShowContextHosts = false;
    bool m_UI_ShowHostPannel = false;
    bool m_UI_ShowDistHostPannel = false;
    

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

    if (ImGui::BeginMenu("Window")) {
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


    ImGui::Separator();

    if (ImGui::BeginMenu("Content & Data")) {
      if (ImGui::MenuItem("Toolchain(s)")) {
        exampleLayer->ShowContextToolchains();
      }
      if (ImGui::MenuItem("Host(s)")) {
        exampleLayer->ShowContextHosts();
      }
      if (ImGui::MenuItem("System(s)")) {
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Chrooter(s)")) {
      }
      if (ImGui::MenuItem("Package(s)")) {
      }
      if (ImGui::MenuItem("Strapper(s)")) {
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Builded Toolchain(s)")) {
        exampleLayer->ShowContextDistToolchains();
      }
      if (ImGui::MenuItem("Builded Host(s)")) {
        exampleLayer->ShowContextDistHost();
        
      }
      if (ImGui::MenuItem("Builded System(s)")) {
      }
      ImGui::Separator();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Build")) {
      if (ImGui::MenuItem("Build a Toolchain")) {
        exampleLayer->ShowToolchainToBuild();
      }
      

      ImGui::EndMenu();
    }
    });


    return app;
}

// Faire plutot un combo Registered / Active / Innactive / Desactiverd / OnError
// et des types (matrix, provider etc)
// possibilité de filtrer