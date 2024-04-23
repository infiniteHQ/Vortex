#include "../../include/vortex.h"
#include "./src/instances/toolchainInstance/ToolchainRenderInstance.h"
#include "./src/toolchain.h"


#ifndef CToolchainModule
ToolchainsModuleCTX *CToolchainModule = NULL;
#endif


PackagesModuleCTX *CPackagesModule = NULL;
TasklistModuleCTX *CTasklistsModule = NULL;


 void CreateModuleContext(){
  ToolchainsModuleCTX *ctx = VX_NEW(ToolchainsModuleCTX);
  CToolchainModule = ctx;
}



// TODO : Documentation des apis & vortex install "modulname" & vortex register (current dir)
// TODO : Toolchains & task reconsitution & VIDEO

class ToolchainsModule : public ModuleInterface
{
public:
    /**
     * @brief When the module is turning on, this function is executed. In this, all components are declared, all contents are loaded
    */
    void execute() override
    {
        CreateModuleContext();
        CToolchainModule->m_interface = ModuleInterface::GetEditorModuleByName(this->m_name);
        
        
        // Adding functions
        this->AddFunction(RegisterToolchains, "RegisterToolchains");


        this->ExecFunction("RegisterToolchains");
        // Adding events

    }

    /**
     * @brief This is the main rendering channel. There is the main "window" of the module.
    */
    void render() override
    {
        
		ImGui::Begin("Toolchains");
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("table_", 3, flags))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            // Iterate through each package
            for (int i = 0; i < CToolchainModule->m_toolchains.size(); i++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);

                    if (column == 0)
                    {
                        std::string label = "Open ###" + CToolchainModule->m_toolchains[i]->name;
                        if (ImGui::Button(label.c_str()))
                        {
                            // Create a render instance for the package and add it
                            VxContext *ctx = VortexMaker::GetCurrentContext();
                            std::shared_ptr<ToolchainRenderInstance> instance = std::make_shared<ToolchainRenderInstance>(ctx, CToolchainModule->m_toolchains[i]);
                            instance->name = CToolchainModule->m_toolchains[i]->name;
                            this->AddModuleRenderInstance(instance);
                        }
                    }
                    if (column == 1)
                    {
                        ImGui::Text(CToolchainModule->m_toolchains[i]->name.c_str());
                    }
                    if (column == 2)
                    {
                        ImGui::Text("Thu 28 Apr 2024");
                    }
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
        // "Launcher" of regitered toolchains
    }
/*
    void openToolchain(){ // Make a function
        // Get instance, open it and create a render instance

        // Ctx (from module), Toolchain
        ToolchainRenderInstance instance(VortexMaker::GetCurrentContext(), nullptr);
        this->AddModuleRenderInstance(instance);
    }*/
};

extern "C" ModuleInterface *create_em()
{
    return new ToolchainsModule();
}

// Initialiser les modules dans le contexte depuis le vortex.config
