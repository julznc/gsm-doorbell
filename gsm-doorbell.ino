
#include "utils.h"

const int STATUS_LED = 13;

void setup() {
  DBG("started %s", "gsm-doorbell");
  pinMode(STATUS_LED, OUTPUT);
}

void loop() {
  DBG("running");
  digitalWrite(STATUS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_LED, LOW);
  delay(500);
}

