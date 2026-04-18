#include <Arduino.h>
#include "config.h"

// ---- UART ----
HardwareSerial Serial1(PA_10, PA_9);

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

  // ---- REAL digital inputs (ACTIVE HIGH, external pull-down) ----
// ---- ACTIVE HIGH inputs, internal pull-down ----
//pinMode(IN_C1, INPUT_PULLDOWN);
//pinMode(IN_C1_HP, INPUT_PULLDOWN);
//pinMode(IN_C1_LP, INPUT_PULLDOWN);
//pinMode(IN_C1_OL, INPUT_PULLDOWN);
//
//pinMode(IN_C2, INPUT_PULLDOWN);
//pinMode(IN_C2_HP, INPUT_PULLDOWN);
//pinMode(IN_C2_LP, INPUT_PULLDOWN);
//pinMode(IN_C2_OL, INPUT_PULLDOWN);
//
//pinMode(IN_HT1_OL, INPUT_PULLDOWN);
//pinMode(IN_HT2_OL, INPUT_PULLDOWN);
//
//pinMode(IN_DOOR, INPUT_PULLDOWN);
//pinMode(IN_PHASE, INPUT_PULLDOWN);
//
//pinMode(IN_FAN1_P, INPUT_PULLDOWN);
//pinMode(IN_FAN2_P, INPUT_PULLDOWN);
//pinMode(IN_HT1_P, INPUT_PULLDOWN);
//pinMode(IN_HT2_P, INPUT_PULLDOWN);
//pinMode(IN_HF1_P, INPUT_PULLDOWN);
//pinMode(IN_HF2_P, INPUT_PULLDOWN);

}

// ---------- STATUS JSON (ACTIVE HIGH LOGIC) ----------
void sendStatus(const char *state)
{
  Serial1.print("{\"DT\":\"0-1-1 0:32:39\",");
  Serial1.print("\"T\":5.6,");
  Serial1.print("\"H\":70.0,");

  // ---- Compressor 1 ----
  Serial1.print("\"C1P\":");  Serial1.print(digitalRead(IN_C1));     Serial1.print(',');
  Serial1.print("\"C1HP\":"); Serial1.print(digitalRead(IN_C1_HP));  Serial1.print(',');
  Serial1.print("\"C1LP\":"); Serial1.print(digitalRead(IN_C1_LP));  Serial1.print(',');
  Serial1.print("\"C1OL\":"); Serial1.print(digitalRead(IN_C1_OL));  Serial1.print(',');

  // ---- Compressor 2 ----
  Serial1.print("\"C2P\":");  Serial1.print(digitalRead(IN_C2));     Serial1.print(',');
  Serial1.print("\"C2HP\":"); Serial1.print(digitalRead(IN_C2_HP));  Serial1.print(',');
  Serial1.print("\"C2LP\":"); Serial1.print(digitalRead(IN_C2_LP));  Serial1.print(',');
  Serial1.print("\"C2OL\":"); Serial1.print(digitalRead(IN_C2_OL));  Serial1.print(',');

  // ---- Humidifier ----
  Serial1.print("\"HF1P\":"); Serial1.print(digitalRead(IN_HF1_P)); Serial1.print(',');
  Serial1.print("\"HF2P\":"); Serial1.print(digitalRead(IN_HF2_P)); Serial1.print(',');

  // ---- Fans ----
  Serial1.print("\"F1P\":");  Serial1.print(digitalRead(IN_FAN1_P));Serial1.print(',');
  Serial1.print("\"F2P\":");  Serial1.print(digitalRead(IN_FAN2_P));Serial1.print(',');

  // ---- Heaters ----
  Serial1.print("\"HT1P\":");  Serial1.print(digitalRead(IN_HT1_P));  Serial1.print(',');
  Serial1.print("\"HT1OL\":"); Serial1.print(digitalRead(IN_HT1_OL)); Serial1.print(',');
  Serial1.print("\"HT2P\":");  Serial1.print(digitalRead(IN_HT2_P));  Serial1.print(',');
  Serial1.print("\"HT2OL\":"); Serial1.print(digitalRead(IN_HT2_OL)); Serial1.print(',');

  // ---- Door & Phase ----
  Serial1.print("\"DR\":"); Serial1.print(digitalRead(IN_DOOR));  Serial1.print(',');
  Serial1.print("\"PH\":"); Serial1.print(digitalRead(IN_PHASE)); Serial1.print(',');

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
  allRelaysOff();
  sendStatus("IDLE");
  delay(3000);

  for (int i = 0; i < RELAY_COUNT; i++) {
    allRelaysOff();
    digitalWrite(relay_pins[i], HIGH);
    sendStatus("TEST");
    delay(3000);
  }

  allRelaysOff();
  sendStatus("IDLE");
  while (1);
}
