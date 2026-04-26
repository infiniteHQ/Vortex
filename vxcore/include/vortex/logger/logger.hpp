#pragma once
#include <vortex.h>

#ifndef VORTEX_LOGGER_HPP
#define VORTEX_LOGGER_HPP

namespace vxe {
  VORTEX_API std::shared_ptr<spdlog::logger> create_log_pool(const std::string &pool_name);
  VORTEX_API std::shared_ptr<spdlog::logger> create_global_logger();
  VORTEX_API std::shared_ptr<spdlog::logger> create_console_logger();
  VORTEX_API void drop_loggers();

  VORTEX_API void log_info(const std::string &scope, const std::string &message);
  VORTEX_API void log_info(const std::string &pool_name, const std::string &scope, const std::string &message);
  VORTEX_API void log_warn(const std::string &scope, const std::string &message);
  VORTEX_API void log_error(const std::string &scope, const std::string &message);
  VORTEX_API void log_fatal(const std::string &scope, const std::string &message);

#define VXINFO(scope, message)                log_info(scope, message);
#define VXPOOLINFO(pool_name, scope, message) log_info(pool_name, scope, message);
#define VXWARN(scope, message)                log_warn(scope, message);
#define VXERROR(scope, message)               log_error(scope, message);
#define VXFATAL(scope, message)               log_fatal(scope, message);
}  // namespace vxe

#endif  // VORTEX_LOGGER_HPP