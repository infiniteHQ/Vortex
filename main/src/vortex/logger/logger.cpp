#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief LogInfo logs an informational message.
 * 
 * This function logs an informational message using the provided scope and message strings.
 * 
 * @param scope The scope of the log message.
 * @param message The message to be logged.
 */
VORTEX_API void VortexMaker::LogInfo(const std::string& scope, const std::string& message){
    // TODO: Add metrics about logs & store log into the context (for earlier uses)
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::info("[" + scope + "] -> "+ message);
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
    // TODO: Add metrics about logs & store log into the context (for earlier uses)
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::warn("[" + scope + "] -> "+ message);
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
    // TODO: Add metrics about logs & store log into the context (for earlier uses)
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::error("[" + scope + "] -> " + message);
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
    // TODO: Add metrics about logs & store log into the context (for earlier uses)
    VxContext &ctx = *CVortexMaker;
    if(ctx.logger){
      spdlog::critical("[" + scope + "] -> "+ message);
    }
}
