#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// Disable debug and trace logging in release builds
#if LRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
  LOG_LEVEL_FATAL = 0,
  LOG_LEVEL_ERROR = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_INFO = 3,
  LOG_LEVEL_DEBUG = 4,
  LOG_LEVEL_TRACE = 5,
} log_level;

b8 init_logger();
void shutdown_logger();

LAPI void log_output(log_level level, const char *message, ...);

// Logs a fatal message
#define LOG_FATAL(message, ...)                                                \
  log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef LOG_ERROR
// Logs an error message
#define LOG_ERROR(message, ...)                                                \
  log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
// Logs a warning message
#define LOG_WARN(message, ...)                                                 \
  log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
// Does nothing when warnings are disabled
#define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
// Logs an info message
#define LOG_INFO(message, ...)                                                 \
  log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
// Does nothing when info is disabled
#define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
// Logs a debug message
#define LOG_DEBUG(message, ...)                                                \
  log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
// Does nothing when debug is disabled
#define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
// Logs a trace message
#define LOG_TRACE(message, ...)                                                \
  log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
// Does nothing when trace is disabled
#define LOG_TRACE(message, ...)
#endif
