#include "assert.h"
#include "logger.h"
#include "platform/platform.h"

// TODO temporary
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

b8 init_logger() {
  // TODO: create log file
  return TRUE;
}

void shutdown_logger() {
  // TODO: cleanup logging/ write queued messafes
}

void log_output(log_level level, const char *message, ...) {
  const char *level_str[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ",
                              "[INFO]: ",  "[DEBUG]: ", "[TRACE]: "};
  b8 is_error = level < LOG_LEVEL_WARN;

  const i32 msg_length = 32000;
  char out_message[msg_length];
  memset(out_message, 0, sizeof(out_message));

  // Format original message
  __builtin_va_list arg_ptr;
  va_start(arg_ptr, message);
  vsnprintf(out_message, 32000, message, arg_ptr);
  va_end(arg_ptr);

  char out_message2[32000];
  sprintf(out_message2, "%s%s\n", level_str[level], out_message);

  if (is_error) {
    platform_console_write_error(out_message2, level);
  } else {
    platform_console_write(out_message2, level);
  }
}

void report_assertion_failure(const char *expression, const char *message,
                              const char *file, i32 line) {
  log_output(LOG_LEVEL_FATAL,
             "Assertion failed: %s, Message: %s, File: %s, Line: %d\n",
             expression, message, file, line);
}
