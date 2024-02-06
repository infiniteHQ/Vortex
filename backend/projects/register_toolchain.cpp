#include "../../vortex.h"
#include "../../vortex_internals.h"

bool VortexMaker::RegisterToolchain(nlohmann::json toolchainData)
{
  VxContext &ctx = *CVortexMaker;
  VxToolchain toolchain;

  toolchain.name = toolchainData["toolchain"]["name"].get<std::string>();
  toolchain.author = toolchainData["toolchain"]["author"].get<std::string>();
  toolchain.type = toolchainData["toolchain"]["type"].get<std::string>();
  toolchain.state = toolchainData["toolchain"]["state"].get<std::string>();
  toolchain.vendor = toolchainData["toolchain"]["vendor"].get<std::string>();
  toolchain.platform = toolchainData["toolchain"]["platform"].get<std::string>();

  toolchain.target_arch = toolchainData["configs"]["target_arch"].get<std::string>();
  
  toolchain.target_arch = toolchainData["configs"]["target_arch"].get<std::string>();
  toolchain.host_arch = toolchainData["configs"]["host_arch"].get<std::string>();
  toolchain.builder_arch = toolchainData["configs"]["builder_arch"].get<std::string>();

  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    toolchain.RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }

  toolchain.FindPackages();

  ctx.IO.toolchains.push_back(toolchain);

  return true;
}
