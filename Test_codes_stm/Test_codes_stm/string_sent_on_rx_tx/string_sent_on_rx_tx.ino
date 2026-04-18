#include <Arduino.h>

// UART to ESP
HardwareSerial Serial1(PA_10, PA_9); // RX=PA10, TX=PA9

void setup()
{
  Serial1.begin(9600);
}

void loop()
{
  Serial1.println("STM:HELLO");
  delay(1000);
}
