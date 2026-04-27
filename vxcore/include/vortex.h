//
//  vortex.h
//  Root header files, including all the vortex back-end features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

// (Integer encoded as XYYZZ for use in #if preprocessor conditionals, e.g. '#if
// VORTEX_VERSION_NUM >= 12345')
#define VORTEX_VERSION     "1.0"
#define VORTEX_VERSION_NUM 1000

#pragma once

#include "vortex_configs.h"

// Includes
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <regex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#if !defined(_WIN32) || !defined(_WIN64)
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>
#endif

// Third party includes
#include "../../lib/cherry/cherry.hpp"
#include "../../lib/json/single_include/nlohmann/json.hpp"
#include "../../lib/spdlog/include/spdlog/sinks/basic_file_sink.h"
#include "../../lib/spdlog/include/spdlog/sinks/daily_file_sink.h"
#include "../../lib/spdlog/include/spdlog/sinks/rotating_file_sink.h"
#include "../../lib/spdlog/include/spdlog/sinks/stdout_color_sinks.h"
#include "../../lib/spdlog/include/spdlog/spdlog.h"

namespace fs = std::filesystem;

#ifndef VORTEX_API
#define VORTEX_API
#endif
#ifndef VORTEX_EXT_API
#define VORTEX_EXT_API VORTEX_API
#endif

struct EnvProject;

struct ArgumentValues;
struct ReturnValues;
struct Theme;

struct CommandOutput;
struct ItemHandlerInterface;

// Internals (from vortex_internals.h)
struct VxContext;

namespace vxe {
  VORTEX_API std::shared_ptr<VxContext> create_context();
  VORTEX_API void destroy_context(std::shared_ptr<VxContext> ctx = nullptr);
  VORTEX_API std::shared_ptr<VxContext> get_current_context();
  VORTEX_API void set_current_context(std::shared_ptr<VxContext> ctx);
  VORTEX_API void initialize();
}  // namespace vxe
//_____________________________________________________________________________

namespace vxe {
  /**
   * @brief Data structure for modules & plugins functions, events and other
   * stuff, oriented around JSON architecture to easely communicate complex types
   * or large amount of data. Formatted in std::string to increase the simplicity
   * of data transferts
   */
  struct Values {
   public:
    Values() { };
    Values(const std::string &val) : value(val) { };
    std::string get() {
      return value;
    };
    nlohmann::json get_json() {
      return nlohmann::json::parse(value);
    };

    template<typename T>
    T get_json(const std::string &val) {
      return nlohmann::json::parse(val).get<T>();
    };

    void set(const std::string &val) {
      value = val;
    };
    void set_json(const nlohmann::json &val) {
      value = val.dump();
    };

   private:
    std::string value = "null";
  };
}  // namespace vxe

struct ArgumentValues : public vxe::Values {
  using vxe::Values::Values;
};

struct ReturnValues : public vxe::Values {
  using vxe::Values::Values;
};