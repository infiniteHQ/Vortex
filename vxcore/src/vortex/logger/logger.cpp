//
//  logger.cpp
//  Sources for logging and debug features
//
//	Copyright (c) 2026 Infinite
//
//	This work is licensed under the terms of the Apache-2.0 license.
//	For a copy, see <https://github.com/infiniteHQ/Vortex/blob/main/LICENSE>.
//

#include "../../../include/vortex/logger/logger.hpp"

#include "../../../include/vortex_internals.h"

VORTEX_API void vxe::drop_loggers() {
  spdlog::drop_all();
}

VORTEX_API std::shared_ptr<spdlog::logger> vxe::create_log_pool(const std::string &pool_name) {
  auto ctx = vxe::get_current_context();

#if defined(_WIN32) || defined(_WIN64)
  std::string file_path =
      vxe::get_home_directory() + "\\.vx\\sessions\\" + ctx->state.session_id + "\\logs\\" + pool_name + ".log";
#else
  std::string file_path =
      vxe::get_home_directory() + "/.vx/sessions/" + ctx->state.session_id + "/logs/" + pool_name + ".log";
#endif

  std::shared_ptr<spdlog::logger> new_logger = spdlog::daily_logger_mt(pool_name, file_path, 0, 0);
  new_logger->flush_on(spdlog::level::info);

  ctx->pool_loggers.push_back({ pool_name, new_logger });

  return new_logger;
}

VORTEX_API std::shared_ptr<spdlog::logger> vxe::create_global_logger() {
  auto ctx = vxe::get_current_context();

#if defined(_WIN32) || defined(_WIN64)
  std::string file_path = vxe::get_home_directory() + "\\.vx\\sessions\\" + ctx->state.session_id + "\\logs\\global.log";
#else
  std::string file_path = vxe::get_home_directory() + "/.vx/sessions/" + ctx->state.session_id + "/logs/global.log";
#endif

  ctx->global_logger = spdlog::basic_logger_mt("global_logger", file_path);
  ctx->global_logger->flush_on(spdlog::level::info);
  return ctx->global_logger;
}

VORTEX_API std::shared_ptr<spdlog::logger> vxe::create_console_logger() {
  auto ctx = vxe::get_current_context();
  ctx->console_logger = spdlog::stdout_color_mt("console");
  return ctx->console_logger;
}

VORTEX_API void vxe::log_info(const std::string &pool_name, const std::string &scope, const std::string &message) {
  auto ctx = vxe::get_current_context();
  if (ctx->logger) {
    bool finded = false;

    for (auto pool : ctx->pool_loggers) {
      if (pool.first == pool_name) {
        finded = true;

        ctx->global_logger->info("[{}] {}", pool_name, "[" + scope + "] : " + message);

        pool.second->info("[{}] {}", pool_name, "[" + scope + "] : " + message);

        ctx->console_logger->info("[" + scope + "] : " + message);

        if (ctx->logger_registering) {
          std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::info, scope, message);
          log->m_timestamp = vxe::get_current_timestamp();
          ctx->registered_logs.push_back(log);
        }

        return;
      }
    }

    if (!finded) {
      std::shared_ptr<spdlog::logger> logger = vxe::create_log_pool(pool_name);

      ctx->global_logger->info("[{}] {}", pool_name, "[" + scope + "] : " + message);

      logger->info("[{}] {}", pool_name, "[" + scope + "] : " + message);

      ctx->console_logger->info("[" + scope + "] : " + message);
    }
  }
}

/**
 * @brief log_info logs an informational message.
 *
 * This function logs an informational message using the provided scope and
 * message strings.
 *
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void vxe::log_info(const std::string &scope, const std::string &message) {
  auto ctx = vxe::get_current_context();
  if (ctx->logger) {
    ctx->global_logger->info("[{}] {}", "global", "[" + scope + "] : " + message);
    ctx->console_logger->info("[" + scope + "] : " + message);

    if (ctx->logger_registering) {
      std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::info, scope, message);
      log->m_timestamp = vxe::get_current_timestamp();
      ctx->registered_logs.push_back(log);
    }
  }
}

/**
 * @brief log_warn logs a warning message.
 *
 * This function logs a warning message using the provided scope and message
 * strings.
 *
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void vxe::log_warn(const std::string &scope, const std::string &message) {
  auto ctx = vxe::get_current_context();
  if (ctx->logger) {
    spdlog::warn("[" + scope + "] : " + message);

    if (ctx->logger_registering) {
      std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::warn, scope, message);
      log->m_timestamp = vxe::get_current_timestamp();
      ctx->registered_logs.push_back(log);
    }
  }
}

/**
 * @brief log_error logs an error message.
 *
 * This function logs an error message using the provided scope and message
 * strings.
 *
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void vxe::log_error(const std::string &scope, const std::string &message) {
  auto ctx = vxe::get_current_context();
  if (ctx->logger) {
    spdlog::error("[" + scope + "] : " + message);

    if (ctx->logger_registering) {
      std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::err, scope, message);
      log->m_timestamp = vxe::get_current_timestamp();
      ctx->registered_logs.push_back(log);
    }
  }
}

/**
 * @brief log_fatal logs a fatal error message.
 *
 * This function logs a fatal error message using the provided scope and message
 * strings.
 *
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void vxe::log_fatal(const std::string &scope, const std::string &message) {
  auto ctx = vxe::get_current_context();
  if (ctx->logger) {
    spdlog::critical("[" + scope + "] : " + message);

    if (ctx->logger_registering) {
      std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::critical, scope, message);
      log->m_timestamp = vxe::get_current_timestamp();
      ctx->registered_logs.push_back(log);
    }
  }
}
