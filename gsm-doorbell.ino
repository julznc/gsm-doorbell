
#include "fona.h"
#include "utils.h"

#define STATUS_LED  13
#define FONA_RST    2


HardwareSerial *fonaSerial = &Serial1;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void setup()
{
  serial_init();
  while ('c' != serial_read()) {
    DBG("enter 'c' to start");
    delay(1000);
  }

  PRINT("started %s", "gsm-doorbell");

  pinMode(STATUS_LED, OUTPUT);

  fonaSerial->begin(4800);
  DBG("detecting fona device...");
  if (!fona.begin(*fonaSerial)) {
    ERR("Couldn't find FONA");
    while (1) delay(100);
  }

  PRINT("Found FONA (type=%u)", fona.type());
}

void loop()
{
  DBG("running fona %u", fona.type());
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_LED, LOW);
  delay(500);
}

