#include "../../src/vortex.h"
#include "../../src/vortex_internals.h"

bool VortexMaker::RegisterScript(std::shared_ptr<VxScript> script, nlohmann::json packageData){

    VxContext* ctx = VortexMaker::GetCurrentContext(); 

    size_t position = script->path.find("/script.config");
    // script->configFilePath = filepath;

    if (position != std::string::npos)
    {
        script->path.erase(position, 17);
    }

    script->description = packageData["script"]["description"].get<std::string>();
    script->author = packageData["script"]["author"].get<std::string>();
    script->name = packageData["script"]["name"].get<std::string>();

  

    ctx->IO.scripts.push_back(script);

    return true;
}
