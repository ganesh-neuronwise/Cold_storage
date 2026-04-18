#include <Arduino.h>

// UART to ESP / USB-TTL
HardwareSerial Serial1(PA_10, PA_9);

// -------- DIGITAL INPUT DEFINITIONS --------
// Active LOW (opto → GND)

#define IN_C1     PC_13
#define IN_C1_HP  PC_14
#define IN_C1_LP  PC_15
#define IN_C1_OL  PC_0

#define IN_C2     PC_2
#define IN_C2_HP  PC_3
#define IN_C2_LP  PA_6
#define IN_C2_OL  PA_7

#define IN_DOOR   PC_6
#define IN_PHASE  PC_7

static inline uint8_t RD(uint8_t pin)
{
  return !digitalRead(pin);   // 1 = active, 0 = inactive
}

void setup()
{
  Serial1.begin(9600);

  pinMode(IN_C1,     INPUT_PULLUP);
  pinMode(IN_C1_HP,  INPUT_PULLUP);
  pinMode(IN_C1_LP,  INPUT_PULLUP);
  pinMode(IN_C1_OL,  INPUT_PULLUP);

  pinMode(IN_C2,     INPUT_PULLUP);
  pinMode(IN_C2_HP,  INPUT_PULLUP);
  pinMode(IN_C2_LP,  INPUT_PULLUP);
  pinMode(IN_C2_OL,  INPUT_PULLUP);

  pinMode(IN_DOOR,   INPUT_PULLUP);
  pinMode(IN_PHASE,  INPUT_PULLUP);

  Serial1.println("DIGITAL INPUT TEST START");
}

void loop()
{
  Serial1.print("C1=");
  Serial1.print(RD(IN_C1));
  Serial1.print(" HP=");
  Serial1.print(RD(IN_C1_HP));
  Serial1.print(" LP=");
  Serial1.print(RD(IN_C1_LP));
  Serial1.print(" OL=");
  Serial1.print(RD(IN_C1_OL));

  Serial1.print(" | C2=");
  Serial1.print(RD(IN_C2));
  Serial1.print(" HP=");
  Serial1.print(RD(IN_C2_HP));
  Serial1.print(" LP=");
  Serial1.print(RD(IN_C2_LP));
  Serial1.print(" OL=");
  Serial1.print(RD(IN_C2_OL));

  Serial1.print(" | DOOR=");
  Serial1.print(RD(IN_DOOR));
  Serial1.print(" PHASE=");
  Serial1.print(RD(IN_PHASE));

  Serial1.println();
  delay(1000);
}
