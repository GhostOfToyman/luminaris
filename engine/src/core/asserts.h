#pragma once

#include "defines.h"

// Disable assertion by commenting out below line
#define LASSERTIONS_ENABLED

#ifdef LASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

// Implemented in logger.c
LAPI void report_assertion_failure(const char *expression, const char *message,
                                   const char *file, i32 line);

#define LASSERT(expr)                                                          \
  {                                                                            \
    if (expr) {                                                                \
    } else {                                                                   \
      report_assertion_failure(#expr, "", __FILE__, __LINE__);                 \
      debugBreak();                                                            \
    }                                                                          \
  }

#define LASSERT_MSG(expr, msg)                                                 \
  {                                                                            \
    if (expr) {                                                                \
    } else {                                                                   \
      report_assertion_failure(#expr, msg, __FILE__, __LINE__);                \
      debugBreak();                                                            \
    }                                                                          \
  }

#ifndef _DEBUG
#define LASSERT_DEBUG(expr)                                                    \
  {                                                                            \
    if (expr) {                                                                \
    } else {                                                                   \
      report_assertion_failure(#expr, "", __FILE__, __LINE__);                 \
      debugBreak();                                                            \
    }                                                                          \
  }
#else
#define LASSERT_DEBUG(expr)
#endif

#else
#define LASSERT(expr)          // do nothing
#define LASSERT_MSG(expr, msg) // do nothing
#define LASSERT_DEBUG(expr)    // do nothing
#endif
