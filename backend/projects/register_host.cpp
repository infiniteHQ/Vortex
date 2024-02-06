#include "../../vortex.h"
#include "../../vortex_internals.h"

bool VortexMaker::RegisterHost(nlohmann::json toolchainData)
{
  VxContext &ctx = *CVortexMaker;
  VxHost host;

  host.name = toolchainData["host"]["name"].get<std::string>();
  host.author = toolchainData["host"]["author"].get<std::string>();
  host.type = toolchainData["host"]["type"].get<std::string>();
  host.state = toolchainData["host"]["state"].get<std::string>();
  host.vendor = toolchainData["host"]["vendor"].get<std::string>();
  host.platform = toolchainData["host"]["platform"].get<std::string>();
  host.host_arch = toolchainData["host"]["host_arch"].get<std::string>();
  host.target_arch = toolchainData["host"]["target_arch"].get<std::string>();
  host.toolchainToUse = toolchainData["build"]["useToolchain"].get<std::string>();

  nlohmann::json packages = toolchainData["content"]["packages"];
  for (auto &pkg : packages)
  {
    host.RegisterPackage(pkg["label"].get<std::string>(), pkg["origin"].get<std::string>());
  }
  host.FindPackages();

  ctx.IO.hosts.push_back(host);

  return true;
}
