#include "core/lmemory.h"
#include "core/lstring.h"

#include <string.h>

u64 lstrlen(const char *str) { return strlen(str); }

char *lstrdup(const char *str) {
  u64 length = lstrlen(str);
  char *copy = lallocate(length + 1, MEM_TAG_STRING);
  lcopy_memory(copy, str, length);
  return copy;
}
