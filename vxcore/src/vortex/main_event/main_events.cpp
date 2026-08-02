//
//  main_events.cpp
//  Sources for  main events features of the Vortex core.
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

void vxe::trigger_save_all_event() {
  ArgumentValues args;
  ReturnValues ret;
  vxe::call_output_event("save_all", args, ret, "vortex");
}