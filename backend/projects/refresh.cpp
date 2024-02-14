#include "../../vortex.h"
#include "../../vortex_internals.h"


void VxHost::Refresh(){
  std::cout << this->configFilePath << std::endl;
  nlohmann::json toolchainData = VortexMaker::DumpJSON(this->configFilePath);

  this->name = toolchainData["host"]["name"].get<std::string>();
  this->author = toolchainData["host"]["author"].get<std::string>();
  this->type = toolchainData["host"]["type"].get<std::string>();
  this->state = toolchainData["host"]["state"].get<std::string>();
  this->vendor = toolchainData["host"]["vendor"].get<std::string>();
  this->platform = toolchainData["host"]["platform"].get<std::string>();
  this->host_arch = toolchainData["host"]["host_arch"].get<std::string>();
  this->target_arch = toolchainData["host"]["target_arch"].get<std::string>();
  this->toolchainToUse = toolchainData["build"]["useToolchain"].get<std::string>();
}

VORTEX_API void VortexMaker::RefreshDistToolchains() // Rename to RefreshDistHostsList
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  ctx.IO.distToolchains.clear();

  // Dist Toolchains
  for (const auto &file : SearchFiles(ctx.paths.toolchainDistFolder, "toolchain.dist.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);

      VxDistToolchain toolchain;

      toolchain.path = file;

      size_t position = toolchain.path.find("/toolchain.dist.config");
      if (position != std::string::npos)
      {
        toolchain.path.erase(position, 22);
      }

      VortexMaker::RegisterDistToolchain(toolchain, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}


VORTEX_API void VortexMaker::RefreshDistHosts() // Rename to RefreshDistHostsList
{
  VxContext &ctx = *CVortexMaker;

  // clear existing dist toolchains
  ctx.IO.distHosts.clear();

  // Dist Toolchains
  for (const auto &file : SearchFiles(ctx.paths.hostDistFolder, "host.dist.config"))
  {
    try
    {
      nlohmann::json filecontent = DumpJSON(file);

      VxDistHost host;

      host.path = file;

      size_t position = host.path.find("/host.dist.config");
      if (position != std::string::npos)
      {
        host.path.erase(position, 17);
      }

      VortexMaker::RegisterDistHost(host, filecontent);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error : " << e.what() << std::endl;
    }
  }
}
