#include <Arduino.h>
#include <Wire.h>
#include "uRTCLib.h"

// UART for output (to ESP or USB-TTL)
HardwareSerial Serial1(PA_10, PA_9);

// RTC object (DS3231 / DS1307)
uRTCLib rtc(0x68, 0x57);

// ====== CHANGE THIS TO 1 ONLY WHEN YOU WANT TO SET TIME ======
#define SET_RTC_TIME  1   // <-- set to 0 after setting time

void setup()
{
  Serial1.begin(9600);

  Wire.begin();
  URTCLIB_WIRE.begin();

#if SET_RTC_TIME
  // rtc.set(second, minute, hour, dayOfWeek, day, month, year);
  // dayOfWeek: 1=Mon ... 7=Sun

  rtc.set(
    30,   // seconds
    52,  // minutes
    11,  // hours (24h format)
    4,   // day of week
    2,   // day
    4,   // month
    26   // year (2026 → write 26)
  );

  Serial1.println("RTC TIME SET");
#endif
}

void loop()
{
  rtc.refresh();

  Serial1.print("DATE: ");
  Serial1.print(rtc.year());  Serial1.print('-');
  Serial1.print(rtc.month()); Serial1.print('-');
  Serial1.print(rtc.day());

  Serial1.print("  TIME: ");
  Serial1.print(rtc.hour());  Serial1.print(':');
  Serial1.print(rtc.minute());Serial1.print(':');
  Serial1.println(rtc.second());

  delay(1000);
}
