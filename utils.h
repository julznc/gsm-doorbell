#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>

#define DEBUG

#ifdef DEBUG
  #define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
  #define DBG(fmt, ...)  debug_print("%d %s[%d] " fmt, millis(), __FILENAME__, __LINE__, ## __VA_ARGS__)
#else
  #define DBG(...)
#endif

void debug_print(const char *fmt, ...);

#endif //UTILS_H

