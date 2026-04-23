#pragma once
#include <Arduino.h>
#include "uRTCLib.h"
extern volatile bool setReceived;
extern bool testMode;

void send_status_json(
  uRTCLib &rtc,
  float T, float H,
  const char *stateStr,
  const uint8_t *relay_pins
);


//void process_uart_rx();
void process_uart_rx(uRTCLib *rtc = nullptr);
extern float tempSet, humSet;
extern bool runEnable;
void resetSetReceived();
extern HardwareSerial Serial3;
