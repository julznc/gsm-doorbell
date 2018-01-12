
#include "fona.h"
#include "utils.h"


#define STATUS_LED    13  // arduino led

#define FONA_KEY      12  // Key pin
#define FONA_PSTAT    10  // Power Status pin
#define FONA_RST      2   // Reset pin
#define FONA_RI       11  // Ring Indicator pin


Adafruit_FONA fona = Adafruit_FONA(&Serial1, FONA_KEY, FONA_PSTAT, FONA_RST, FONA_RI);

void update_status(void);
void check_fona(void);
char imei[16] = {0};

void setup()
{
  delay(3000);
  PRINT("started %s", "gsm-doorbell");
  update_status();

  DBG("detecting fona device...");
  if (!fona.begin(9600)) {
    ERR("Couldn't find FONA");
    while (1) delay(100);
  }

  PRINT("Found FONA (type=%u)", fona.type());

  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    PRINT("IMEI: %s", imei);
  }

  //set up the FONA to send a +CMTI notification when an SMS is received
  fona.print("AT+CNMI=2,1\r\n");

  PRINT("FONA Ready");
}

void loop()
{
  update_status();
  check_fona();
}

void update_status(void)
{
  static unsigned long prev_ms = 0;
  static int out = HIGH;

  unsigned long ms = millis();
  if (prev_ms == 0) {
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, out);
    prev_ms = ms;
  }

  if (ms - prev_ms >= 500) {
    out = !out;
    digitalWrite(STATUS_LED, out);
    prev_ms = ms;
  }
}

void check_fona(void)
{
  if (!fona.available())
    return; // no data available

  // for notifications from the FONA
  char notificationBuffer[128];
  char* bufPtr = notificationBuffer;
  size_t charCount = 0;

  while (fona.available() && (charCount < (sizeof(notificationBuffer)-1))) {
    char c = fona.read();
    if (('\r'==c) || ('\n'==c))
        break;
    *bufPtr++ = c;
    charCount++;
    if (!fona.available())
        delay(500);
  }

  if (0==charCount)
    return; // empty notif

  *bufPtr = 0; // null terminate
  //PRINT("notif (%u): %s", charCount, notificationBuffer);
  PRINT("%s", notificationBuffer);

  // sms slot number
  int slot = 0;
  if (1 != sscanf(notificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot)) {
    return; // no sms received yet
  }

  PRINT("slot %d", slot);

  char senderIDbuffer[32];  // sender number
  if (! fona.getSMSSender(slot, senderIDbuffer, sizeof(senderIDbuffer)-1)) {
    ERR("Didn't find SMS message in slot!");
  }
  PRINT("FROM: %s", senderIDbuffer);

  // Retrieve SMS value.
  uint16_t smslen;
  char smsBuffer[250];
  if (fona.readSMS(slot, smsBuffer, sizeof(smsBuffer), &smslen)) { // pass in buffer and max len!
    PRINT("%s", smsBuffer);
  }

  if (fona.deleteSMS(slot)) {
    PRINT("delete sms %d - ok", slot);
  } else {
    ERR("Couldn't delete SMS in slot %d", slot);
    fona.print(F("AT+CMGD=?\r\n"));
  }
}
