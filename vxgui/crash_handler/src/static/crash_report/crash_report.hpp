//
//  crash_report.hpp
//  Headers for the CrashReport utility, displayed on the Vortex Crash Handler
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#pragma once
#include "../../../../../vxcore/include/vortex.h"
#include "../../../../../vxcore/include/vortex_internals.h"

#ifndef CRASH_H
#define CRASH_H

class VORTEX_API CrashReport : public std::enable_shared_from_this<CrashReport> {
 public:
  CrashReport(const std::string &name, const std::string &session_id);

  // window
  std::shared_ptr<Cherry::AppWindow> &get_app_window();
  static std::shared_ptr<CrashReport> create(const std::string &name, const std::string &id);
  void setup_render_callback();
  void render();

  // logic
  std::string get_home_directory();

 private:
  std::string session_id_;
  std::shared_ptr<Cherry::AppWindow> app_window_;
};

#endif  // CRASH_H