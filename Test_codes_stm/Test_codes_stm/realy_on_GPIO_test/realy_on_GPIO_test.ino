#include <Arduino.h>
#include "config.h"

// ---- UART (same as final firmware) ----
HardwareSerial Serial1(PA_10, PA_9);

// ---------- Helper (active LOW real inputs) ----------
static inline uint8_t RD(uint8_t pin)
{
  return digitalRead(pin) ? 0 : 1;
}

// ---------- Relay OFF ----------
void allRelaysOff()
{
  for (int i = 0; i < RELAY_COUNT; i++)
    digitalWrite(relay_pins[i], LOW);
}

// ---------- Setup ----------
void setup()
{
  Serial1.begin(9600);
  delay(1000);

  // ---- Relay outputs ----
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relay_pins[i], OUTPUT);
    digitalWrite(relay_pins[i], LOW);
  }

  // ---- REAL digital inputs ----
  pinMode(IN_C1, INPUT_PULLUP);
  pinMode(IN_C1_HP, INPUT_PULLUP);
  pinMode(IN_C1_LP, INPUT_PULLUP);
  pinMode(IN_C1_OL, INPUT_PULLUP);

  pinMode(IN_C2, INPUT_PULLUP);
  pinMode(IN_C2_HP, INPUT_PULLUP);
  pinMode(IN_C2_LP, INPUT_PULLUP);
  pinMode(IN_C2_OL, INPUT_PULLUP);

  pinMode(IN_HT1_OL, INPUT_PULLUP);
  pinMode(IN_HT2_OL, INPUT_PULLUP);

  pinMode(IN_DOOR, INPUT_PULLUP);
  pinMode(IN_PHASE, INPUT_PULLUP);
  pinMode(PB0, INPUT_PULLUP);
pinMode(PB1, INPUT_PULLUP);
pinMode(PB2, INPUT_PULLUP);
pinMode(PB13, INPUT_PULLUP);
pinMode(PC4, INPUT_PULLUP);
pinMode(PC5, INPUT_PULLUP);

}

// ---------- FINAL-FIRMWARE-COMPATIBLE STATUS ----------
void sendStatus(const char *state)
{
  Serial1.print("{\"DT\":\"0-1-1 0:32:39\",");
  Serial1.print("\"T\":5.6,");
  Serial1.print("\"H\":70.0,");

  // ---- Compressor 1 feedback ----
  Serial1.print("\"C1P\":");  Serial1.print(RD(IN_C1));     Serial1.print(',');
  Serial1.print("\"C1HP\":"); Serial1.print(RD(IN_C1_HP));  Serial1.print(',');
  Serial1.print("\"C1LP\":"); Serial1.print(RD(IN_C1_LP));  Serial1.print(',');
  Serial1.print("\"C1OL\":"); Serial1.print(RD(IN_C1_OL));  Serial1.print(',');

  // ---- Compressor 2 feedback ----
  Serial1.print("\"C2P\":");  Serial1.print(RD(IN_C2));     Serial1.print(',');
  Serial1.print("\"C2HP\":"); Serial1.print(RD(IN_C2_HP));  Serial1.print(',');
  Serial1.print("\"C2LP\":"); Serial1.print(RD(IN_C2_LP));  Serial1.print(',');
  Serial1.print("\"C2OL\":"); Serial1.print(RD(IN_C2_OL));  Serial1.print(',');

  // ---- COMMAND-based P (current firmware behavior) ----
Serial1.print("\"HF1P\":"); Serial1.print(RD(IN_HF1_P)); Serial1.print(',');

Serial1.print("\"HF2P\":"); Serial1.print(RD(IN_HF2_P)); Serial1.print(',');

Serial1.print("\"F1P\":");  Serial1.print(RD(IN_FAN1_P));Serial1.print(',');
Serial1.print("\"F2P\":");  Serial1.print(RD(IN_FAN2_P));Serial1.print(',');
Serial1.print("\"HT1P\":"); Serial1.print(RD(IN_HT1_P)); Serial1.print(',');
  Serial1.print("\"HT1OL\":");Serial1.print(RD(IN_HT1_OL));       Serial1.print(',');
Serial1.print("\"HT2P\":"); Serial1.print(RD(IN_HT2_P)); Serial1.print(',');
  Serial1.print("\"HT2OL\":");Serial1.print(RD(IN_HT2_OL));       Serial1.print(',');

  // ---- Door & Phase ----
  Serial1.print("\"DR\":"); Serial1.print(RD(IN_DOOR));  Serial1.print(',');
  Serial1.print("\"PH\":"); Serial1.print(RD(IN_PHASE)); Serial1.print(',');

  // ---- Relay array ----
  Serial1.print("\"R\":[");
  for (int i = 0; i < RELAY_COUNT; i++) {
    Serial1.print(digitalRead(relay_pins[i]));
    if (i < RELAY_COUNT - 1) Serial1.print(',');
  }
  Serial1.print("],");

  Serial1.print("\"ST\":\"");
  Serial1.print(state);
  Serial1.println("\"}");
}

// ---------- Loop ----------
void loop()
{
  // ---- IDLE ----
  allRelaysOff();
  sendStatus("IDLE");
  delay(3000);

  // ---- Test each relay ----
  for (int i = 0; i < RELAY_COUNT; i++) {
    allRelaysOff();
    digitalWrite(relay_pins[i], HIGH);
    sendStatus("TEST");
    delay(3000);
  }

  allRelaysOff();
  sendStatus("IDLE");

  while (1); // stop
}
