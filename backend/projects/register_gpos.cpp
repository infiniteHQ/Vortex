#include "../../vortex.h"
#include "../../vortex_internals.h"

bool VortexMaker::RegisterGPOS(std::shared_ptr<VxGPOSystem> gpos, nlohmann::json toolchainData)
{
  VxContext &ctx = *CVortexMaker;

  gpos->name = toolchainData["gpos"]["name"].get<std::string>();
  gpos->author = toolchainData["gpos"]["author"].get<std::string>();
  gpos->type = toolchainData["gpos"]["type"].get<std::string>();
  gpos->state = toolchainData["gpos"]["state"].get<std::string>();
  gpos->vendor = toolchainData["gpos"]["vendor"].get<std::string>();
  gpos->platform = toolchainData["gpos"]["platform"].get<std::string>();
  gpos->target_arch = toolchainData["gpos"]["target_arch"].get<std::string>();

  // Register file emplacement

  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    gpos->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }
  gpos->FindPackages();


  nlohmann::json tasklists = toolchainData["content"]["tasklists"];
  for (auto &t : tasklists)
  {
    gpos->RegisterTasklist(t["label"].get<std::string>());
  }
  gpos->FindTasklists();

  ctx.IO.gpoSystems.push_back(gpos);

  return true;
}
