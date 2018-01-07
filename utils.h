#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>

#define DEBUG
//#undef DEBUG


#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef DEBUG
  #define DBG(fmt, ...)   serial_print("%d %s[%d] " fmt "\r\n", millis(), __FILENAME__, __LINE__, ## __VA_ARGS__)
#else
  #define DBG(...)
#endif

#define ERR(fmt, ...)     serial_print("%d %s[%d] [ERR] " fmt "\r\n", millis(), __FILENAME__, __LINE__, ## __VA_ARGS__)
#define PRINT(fmt, ...)   serial_print(fmt "\r\n", ## __VA_ARGS__)

void serial_init(void);
void serial_print(const char *fmt, ...);

#define serial_available()  SerialUSB.available()
#define serial_read()       SerialUSB.read()

#endif //UTILS_H

