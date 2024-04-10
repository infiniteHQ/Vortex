#include "../../src/vortex.h"
#include "../../src/vortex_internals.h"

bool VortexMaker::RegisterHost(std::shared_ptr<VxHost> host, nlohmann::json toolchainData)
{
  VxContext &ctx = *CVortexMaker;

  host->name = toolchainData["host"]["name"].get<std::string>();
  host->author = toolchainData["host"]["author"].get<std::string>();
  host->type = toolchainData["host"]["type"].get<std::string>();
  host->state = toolchainData["host"]["state"].get<std::string>();
  host->vendor = toolchainData["host"]["vendor"].get<std::string>();
  host->platform = toolchainData["host"]["platform"].get<std::string>();

  //host->host_arch = toolchainData["configs"]["host_arch"].get<std::string>();
  //host->target_arch = toolchainData["configs"]["target_arch"].get<std::string>();

  host->toolchainToUse = toolchainData["build"]["use_toolchain"].get<std::string>();

  host->localPackagesPath = toolchainData["data"]["packages"].get<std::string>();
  host->localScriptsPath = toolchainData["data"]["scripts"].get<std::string>();
  host->localPatchsPath = toolchainData["data"]["patchs"].get<std::string>();

  // Register file emplacement

  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    host->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }
  host->FindPackages();


  nlohmann::json tasklists = toolchainData["content"]["tasklists"];
  for (auto &t : tasklists)
  {
    host->RegisterTasklist(t["label"].get<std::string>());
  }
  host->FindTasklists();

  ctx.IO.hosts.push_back(host);

  return true;
}
