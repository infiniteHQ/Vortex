#include "../../src/vortex.h"
#include "../../src/vortex_internals.h"

bool VortexMaker::RegisterToolchain(std::shared_ptr<VxToolchain> toolchain, nlohmann::json toolchainData)
{
  VxContext &ctx = *CVortexMaker;
  
  toolchain->name = toolchainData["toolchain"]["name"].get<std::string>();
  toolchain->author = toolchainData["toolchain"]["author"].get<std::string>();
  toolchain->type = toolchainData["toolchain"]["type"].get<std::string>();
  toolchain->description = toolchainData["toolchain"]["description"].get<std::string>();
  toolchain->version = toolchainData["toolchain"]["version"].get<std::string>();
  toolchain->state = toolchainData["toolchain"]["state"].get<std::string>();

  toolchain->builder_vendor = toolchainData["configs"]["builder_vendor"].get<std::string>();
  toolchain->builder_arch = toolchainData["configs"]["builder_arch"].get<std::string>();
  toolchain->builder_platform = toolchainData["configs"]["builder_platform"].get<std::string>();
  toolchain->builder_cpu = toolchainData["configs"]["builder_cpu"].get<std::string>();
  toolchain->builder_fpu = toolchainData["configs"]["builder_fpu"].get<std::string>();

  toolchain->target_vendor = toolchainData["configs"]["target_vendor"].get<std::string>();
  toolchain->target_arch = toolchainData["configs"]["target_arch"].get<std::string>();
  toolchain->target_platform = toolchainData["configs"]["target_platform"].get<std::string>();
  toolchain->target_cpu = toolchainData["configs"]["target_cpu"].get<std::string>();
  toolchain->target_fpu = toolchainData["configs"]["target_fpu"].get<std::string>();

  toolchain->host_vendor = toolchainData["configs"]["host_vendor"].get<std::string>();
  toolchain->host_arch = toolchainData["configs"]["host_arch"].get<std::string>();
  toolchain->host_platform = toolchainData["configs"]["host_platform"].get<std::string>();
  toolchain->host_cpu = toolchainData["configs"]["host_cpu"].get<std::string>();
  toolchain->host_fpu = toolchainData["configs"]["host_fpu"].get<std::string>();

  //toolchain->compressionMode = toolchainData["configs"]["compression"].get<std::string>();

  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    toolchain->RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }
  toolchain->FindPackages();

  nlohmann::json tasklists = toolchainData["content"]["tasklists"];
  for (auto &t : tasklists)
  {
    toolchain->RegisterTasklist(t["label"].get<std::string>());
  }
  toolchain->FindTasklists();

  toolchain->Init();

  ctx.IO.toolchains.push_back(toolchain);

  return true;
}
