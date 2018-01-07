
#include "fona.h"
#include "utils.h"


#define STATUS_LED    13  // arduino led

#define FONA_KEY      12  // Key pin
#define FONA_PSTAT    10  // Power Status pin
#define FONA_RST      2   // Reset pin
#define FONA_RI       11  // Ring Indicator pin


Adafruit_FONA fona = Adafruit_FONA(&Serial1, FONA_KEY, FONA_PSTAT, FONA_RST, FONA_RI);

void setup()
{
  serial_init();
  while ('c' != serial_read()) {
    DBG("enter 'c' to start");
    delay(1000);
  }

  PRINT("started %s", "gsm-doorbell");

  pinMode(STATUS_LED, OUTPUT);

  DBG("detecting fona device...");
  if (!fona.begin(9600)) {
    ERR("Couldn't find FONA");
    while (1) delay(100);
  }

  PRINT("Found FONA (type=%u)", fona.type());
}

void loop()
{
  //DBG("running fona %u", fona.type());
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_LED, LOW);
  delay(500);
}

