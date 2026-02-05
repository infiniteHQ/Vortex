#include <vortex.h>
#include <vortex/scripting/scripting.hpp>

namespace VortexMaker {
namespace Script {

VXLUA_FUNC(LogInfo) {
  std::string log = vxlua_getstring(L, 1);

  VXINFO("test", log);
  return 0;
}

void RegisterLogicAPI(lua_State *L) {
  // Application
  VXLUA_REGISTER(L, -1, LogInfo);
}
} // namespace Script
} // namespace VortexMaker