#include "../../main/include/vortex.h"
#include "../../main/include/vortex_internals.h"


bool VortexMaker::RegisterDistHost(VxDistHost host, nlohmann::json packageData)
{
  VxContext &ctx = *CVortexMaker;

  host.name = packageData["host"]["name"].get<std::string>();
  host.target = packageData["host"]["target"].get<std::string>();
  host.CC = packageData["configs"]["CC"].get<std::string>();
  host.CXX = packageData["configs"]["CXX"].get<std::string>();
  host.AR = packageData["configs"]["AR"].get<std::string>();
  host.AS = packageData["configs"]["AS"].get<std::string>();
  host.LD = packageData["configs"]["LD"].get<std::string>();
  host.STRIP = packageData["configs"]["STRIP"].get<std::string>();
  host.RANLIB = packageData["configs"]["RANLIB"].get<std::string>();

  ctx.IO.distHosts.push_back(host);
  return true;
}
