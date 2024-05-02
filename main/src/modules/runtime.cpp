#include "../../include/modules/runtime.h"

VORTEX_API void VortexMaker::StartAllModules()
{
    // Get reference to the Vortex context
    VxContext &ctx = *CVortexMaker;

    // Start all modules & create data environments
    // TODO create dist envs
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