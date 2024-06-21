#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"


VORTEX_API std::shared_ptr<spdlog::logger> VortexMaker::CreateLogPool(const std::string &pool_name)
{
  return nullptr;
}

VORTEX_API std::shared_ptr<spdlog::logger> VortexMaker::CreateGlobalLogger()
{
  VxContext &ctx = *CVortexMaker;
  std::string file_path = VortexMaker::getHomeDirectory() + "/.vx/sessions/" + ctx.state.session_id + "/logs/global.log";
  ctx.global_logger = spdlog::basic_logger_mt("global_logger", file_path);
  ctx.global_logger->flush_on(spdlog::level::info);
  return nullptr;
}

VORTEX_API std::shared_ptr<spdlog::logger> VortexMaker::CreateConsoleLogger()
{
  VxContext &ctx = *CVortexMaker;
  ctx.console_logger = spdlog::stdout_color_mt("console");
  return nullptr;
}

VORTEX_API void VortexMaker::LogInfo(const std::string &pool_name, const std::string &scope, const std::string &message)
{
  VxContext &ctx = *CVortexMaker;
  if (ctx.logger)
  {
    for (auto pool : ctx.pool_loggers)
    {
      if (pool.first == pool_name)
      {
        spdlog::info("[" + scope + "] : " + message);

        if (ctx.logger_registering)
        {
          std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::info, scope, message);
          log->m_timestamp = VortexMaker::getCurrentTimeStamp();
          ctx.registered_logs.push_back(log);
        }
      }
    }
  }
}

/**
 * @brief LogInfo logs an informational message.
 * 
 * This function logs an informational message using the provided scope and message strings.
 * 
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void VortexMaker::LogInfo(const std::string& scope, const std::string& message){
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){

        // Ajout du nom du thread au message pour le logger global
        ctx.global_logger->info("[{}] {}", "global", "[" + scope + "] : "+ message);

        // Également logguer dans le logger global (console)
        ctx.console_logger->info("[" + scope + "] : "+ message);

      if(ctx.logger_registering){
        std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::info, scope, message);
        log->m_timestamp = VortexMaker::getCurrentTimeStamp();
        ctx.registered_logs.push_back(log);
      }
    }
}

/**
 * @brief LogWarn logs a warning message.
 * 
 * This function logs a warning message using the provided scope and message strings.
 * 
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void VortexMaker::LogWarn(const std::string& scope, const std::string& message){
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::warn("[" + scope + "] : "+ message);

      if(ctx.logger_registering){
        std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::warn, scope, message);
        log->m_timestamp = VortexMaker::getCurrentTimeStamp();
        ctx.registered_logs.push_back(log);
      }
    }
}

/**
 * @brief LogError logs an error message.
 * 
 * This function logs an error message using the provided scope and message strings.
 * 
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void VortexMaker::LogError(const std::string& scope, const std::string& message){
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::error("[" + scope + "] : " + message);

      if(ctx.logger_registering){
        std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::err, scope, message);
        log->m_timestamp = VortexMaker::getCurrentTimeStamp();
        ctx.registered_logs.push_back(log);
      }
    }
}

/**
 * @brief LogFatal logs a fatal error message.
 * 
 * This function logs a fatal error message using the provided scope and message strings.
 * 
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void VortexMaker::LogFatal(const std::string& scope, const std::string& message){
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::critical("[" + scope + "] : "+ message);

      if(ctx.logger_registering){
        std::shared_ptr<VxSystemLog> log = std::make_shared<VxSystemLog>(spdlog::level::level_enum::critical, scope, message);
        log->m_timestamp = VortexMaker::getCurrentTimeStamp();
        ctx.registered_logs.push_back(log);
      }
    }
}
