#include <Arduino.h>

#define TEST_TX PB10
#define TEST_RX PB11

void setup()
{
  // Use pins as normal GPIO
  pinMode(TEST_TX, OUTPUT);
  pinMode(TEST_RX, OUTPUT);

  digitalWrite(TEST_TX, LOW);
  digitalWrite(TEST_RX, LOW);  digitalWrite(TEST_RX, LOW);

}
  digitalWrite(TEST_RX, LOW  digitalWrite(TEST_RX, LOW);
);

void loop()
{
  // ---- HIGH for 2 sec ----
  digitalWrite(TEST_TX, HIGH);
  digitalWrite(TEST_RX, HIGH);
  delay(2000);

  // ---- LOW for 2 sec ----
  digitalWrite(TEST_TX, LOW);
  digitalWrite(TEST_RX, LOW);
  delay(2000);
}
