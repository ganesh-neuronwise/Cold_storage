#include <Arduino.h>

// ---------- UART ----------
HardwareSerial Serial1(PA_10, PA_9);   // RX , TX

// ---------- LOGIC MODE ----------
// 0 = active LOW  (GND = 1)
// 1 = active HIGH (VCC = 1)
#define LOGIC_ACTIVE_LOW 0
#define LOGIC_ACTIVE_HIGH 1

// change this if you want reverse logic
#define INPUT_MODE LOGIC_ACTIVE_LOW


// ---------- INPUT PINS ----------
#define IN_C1      PC_13
#define IN_C1_HP   PC_14
#define IN_C1_LP   PC_15
#define IN_C1_OL   PC_0

#define IN_C2      PC_2
#define IN_C2_HP   PC_3
#define IN_C2_LP   PA_6
#define IN_C2_OL   PA_7

#define IN_HT1_OL  PB_12
#define IN_HT2_OL  PB_14

#define IN_DOOR    PC_7
#define IN_PHASE   PC_6

#define IN_FAN1_P  PB_0
#define IN_FAN2_P  PB_1

#define IN_HT1_P   PB_2
#define IN_HT2_P   PB_13

#define IN_HF1_P   PC_4
#define IN_HF2_P   PC_5



// ---------- READ HELPER ----------
uint8_t readInput(uint8_t pin)
{
  uint8_t raw = digitalRead(pin);

  if (INPUT_MODE == LOGIC_ACTIVE_LOW)
    return !raw;    // GND = 1
  else
    return raw;     // HIGH = 1
}



void setup()
{
  Serial1.begin(9600);

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

  pinMode(IN_FAN1_P, INPUT_PULLUP);
  pinMode(IN_FAN2_P, INPUT_PULLUP);

  pinMode(IN_HT1_P, INPUT_PULLUP);
  pinMode(IN_HT2_P, INPUT_PULLUP);

  pinMode(IN_HF1_P, INPUT_PULLUP);
  pinMode(IN_HF2_P, INPUT_PULLUP);
}



void loop()
{

  Serial1.print("{");

  Serial1.print("\"C1P\":");   Serial1.print(readInput(IN_C1));     Serial1.print(",");
  Serial1.print("\"C1HP\":");  Serial1.print(readInput(IN_C1_HP));  Serial1.print(",");
  Serial1.print("\"C1LP\":");  Serial1.print(readInput(IN_C1_LP));  Serial1.print(",");
  Serial1.print("\"C1OL\":");  Serial1.print(readInput(IN_C1_OL));  Serial1.print(",");

  Serial1.print("\"C2P\":");   Serial1.print(readInput(IN_C2));     Serial1.print(",");
  Serial1.print("\"C2HP\":");  Serial1.print(readInput(IN_C2_HP));  Serial1.print(",");
  Serial1.print("\"C2LP\":");  Serial1.print(readInput(IN_C2_LP));  Serial1.print(",");
  Serial1.print("\"C2OL\":");  Serial1.print(readInput(IN_C2_OL));  Serial1.print(",");

  Serial1.print("\"HF1P\":");  Serial1.print(readInput(IN_HF1_P));  Serial1.print(",");
  Serial1.print("\"HF2P\":");  Serial1.print(readInput(IN_HF2_P));  Serial1.print(",");

  Serial1.print("\"F1P\":");   Serial1.print(readInput(IN_FAN1_P)); Serial1.print(",");
  Serial1.print("\"F2P\":");   Serial1.print(readInput(IN_FAN2_P)); Serial1.print(",");

  Serial1.print("\"HT1P\":");  Serial1.print(readInput(IN_HT1_P));  Serial1.print(",");
  Serial1.print("\"HT1OL\":"); Serial1.print(readInput(IN_HT1_OL)); Serial1.print(",");
  Serial1.print("\"HT2P\":");  Serial1.print(readInput(IN_HT2_P));  Serial1.print(",");
  Serial1.print("\"HT2OL\":"); Serial1.print(readInput(IN_HT2_OL)); Serial1.print(",");

  Serial1.print("\"DR\":");    Serial1.print(readInput(IN_DOOR));   Serial1.print(",");
  Serial1.print("\"PH\":");    Serial1.print(readInput(IN_PHASE));

  Serial1.println("}");

  delay(1000);
}
