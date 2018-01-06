
#include <Arduino.h>
#include "utils.h"

void serial_init(void)
{
  static int initialized = 0;
  if (!initialized) {
    SerialUSB.begin(115200);
    while (!SerialUSB) ;
    initialized = 1;
  }
}
void serial_print(const char *fmt, ...)
{
  char buff[256];
  va_list args;
  va_start (args, fmt);
  vsnprintf(buff, sizeof(buff), fmt, args);
  va_end (args);
  SerialUSB.print(buff);;
}

