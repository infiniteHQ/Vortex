//
//  main_events.hpp
//  Headers for main events of the Vortex core
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include <vortex.h>

#ifndef VORTEX_MAIN_EVENTS_HPP
#define VORTEX_MAIN_EVENTS_HPP

namespace vxe {
  // outputs events (from inside)
  VORTEX_API void trigger_save_all_event();
  VORTEX_API void trigger_content_browser_opened_event();
  VORTEX_API void trigger_console_logs_opened_event();

  // inputs events (from outside)
  VORTEX_API void register_open_content_browser_event();
}  // namespace vxe

#endif  // VORTEX_MAIN_EVENTS_HPP