#include "../../../../main/include/vortex.h"
#include "../../../../main/include/vortex_internals.h"

void VxScript::Refresh()
{
  VortexMaker::LogInfo("Core", "Refreshing script " + this->name + " from " + this->configFilePath);
  nlohmann::json scriptData = VortexMaker::DumpJSON(this->configFilePath);

  VortexMaker::LogInfo("Core", "Getting script \"script\" datas from " + this->configFilePath);
  this->name = scriptData["script"]["name"].get<std::string>();
  this->author = scriptData["script"]["author"].get<std::string>();
}

void VxScript::PushSave(std::shared_ptr<VxScriptSave> save)
{
    VxContext &ctx = *CVortexMaker;
    nlohmann::json scriptData;
    scriptData["script"]["name"] = save->name;
    scriptData["script"]["author"] = save->author;


    std::ofstream file(this->configFilePath);
    if (file.is_open())
    {
        file << std::setw(4) << scriptData << std::endl;
          VortexMaker::LogInfo("Core", "Object saved to " + this->configFilePath);
        file.close();
    }
    else
    {
        VortexMaker::LogError("Core", "Unable to open file " + this->configFilePath + " for writing!");
    }
}