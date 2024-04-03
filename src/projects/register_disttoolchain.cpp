#include "../../src/vortex.h"
#include "../../src/vortex_internals.h"

bool VortexMaker::RegisterDistToolchain(VxDistToolchain toolchain, nlohmann::json packageData)
{
  VxContext &ctx = *CVortexMaker;

  toolchain.name = packageData["toolchain"]["name"].get<std::string>();
  toolchain.target = packageData["toolchain"]["target"].get<std::string>();
  toolchain.CC = packageData["configs"]["CC"].get<std::string>();
  toolchain.CXX = packageData["configs"]["CXX"].get<std::string>();
  toolchain.AR = packageData["configs"]["AR"].get<std::string>();
  toolchain.AS = packageData["configs"]["AS"].get<std::string>();
  toolchain.LD = packageData["configs"]["LD"].get<std::string>();
  toolchain.STRIP = packageData["configs"]["STRIP"].get<std::string>();
  toolchain.RANLIB = packageData["configs"]["RANLIB"].get<std::string>();

  ctx.IO.distToolchains.push_back(toolchain);
  return true;
}
