#include <vortex.h>
#include <vortex/scripting/scripting.hpp>

namespace VortexMaker {
namespace Script {

VXLUA_FUNC(LogInfo) {
  std::string log = lua_getstring(L, 1);

  VXINFO("test", log);
  return 0;
}

void RegisterLogicAPI(lua_State *L) {
  // Application
  VXLUA_REGISTER(L, -1, Close);
}
} // namespace Script
} // namespace VortexMaker