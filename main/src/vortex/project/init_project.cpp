#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"
#include "../../../include/modules/load.h"

/**
 * @brief Initialize project settings based on provided JSON configurations.
 * 
 * This function initializes project settings based on the configurations provided in a JSON object.
 * It sets various properties of the VortexMaker context object accordingly.
 * 
 * @param main_configs JSON object containing main project configurations.
 */
VORTEX_API void VortexMaker::InitProject(const nlohmann::json& main_configs)
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Initialize context properties based on JSON configurations
    ctx.author = main_configs["project"]["author"].get<std::string>();
    ctx.description = main_configs["project"]["description"].get<std::string>();
    ctx.label = main_configs["project"]["name"].get<std::string>();
    ctx.name = main_configs["project"]["name"].get<std::string>();
    ctx.type = main_configs["project"]["type"].get<std::string>();
    ctx.version = main_configs["project"]["version"].get<std::string>();

    ctx.packagesPath = main_configs["data"]["packages"].get<std::string>();
    ctx.toolchainsPath = main_configs["data"]["toolchains"].get<std::string>();
    ctx.gposPath = main_configs["data"]["gpos"].get<std::string>();
    ctx.scriptsPath = main_configs["data"]["scripts"].get<std::string>(); // Fixed typo here
    ctx.hostsPath = main_configs["data"]["hosts"].get<std::string>();

    ctx.paths.toolchainDistFolder = main_configs["dist"]["toolchains"].get<std::string>();
    ctx.paths.hostDistFolder = main_configs["dist"]["hosts"].get<std::string>();

    // Set project path to current working directory
    ctx.projectPath = fs::current_path();

    ctx.logger = true;
    ctx.logger_registering = true;

    // Load modules installed in the current project
    VortexMaker::LoadEditorModules(ctx.projectPath, ctx.IO.em_handles, ctx.IO.em);

    // Load modules installed in the system
    // Note: These modules are simply initialized in the project, not loaded, but we can add these in CLI/GUI 
    VortexMaker::LoadSystemModules(ctx.IO.sys_em);
    
    // TODO : On a dedicated function
    for(auto em : ctx.IO.em)
    {
        {
            // Set the module data path
            std::string datapath = ctx.projectPath;
            datapath += "/.vx/data/" + em->m_name;
            em->m_datapath = datapath;

            // Try to create the datapath folder (if not exist yet)
            std::string cmd = "sudo mkdir ";
            cmd += datapath.c_str();
            system(cmd.c_str());
        }

        // If the module is a autoexecution module, execute it now.
        if(em->m_auto_exec){
            em->Start();
        }
    }
}
