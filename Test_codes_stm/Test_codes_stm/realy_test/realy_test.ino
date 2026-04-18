#define LED_BUZ PC_1   // Status LED / Buzzer

// Relay pins array
const uint8_t relay_pins[] = {
  PC_10,   // RL1
  PC_11,   // RL2
  PC_12,   // RL3
  PD_2,    // RL4
  PB_3,    // RL5
  PB_4,    // RL6
  PB_5,     // RL7
  PB_8     // RL8
};


#define RELAY_COUNT (sizeof(relay_pins) / sizeof(relay_pins[0]))

void setup()
{
  pinMode(LED_BUZ, OUTPUT);

  // Configure relay pins
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    pinMode(relay_pins[i], OUTPUT);
    digitalWrite(relay_pins[i], LOW);   // ALL RELAYS OFF at boot (IMPORTANT)
  }

  digitalWrite(LED_BUZ, LOW);
}

void loop()
{
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {

    // ---- Relay ON ----
    digitalWrite(relay_pins[i], HIGH);
    digitalWrite(LED_BUZ, HIGH);
    delay(2000);

    // ---- Relay OFF ----
    digitalWrite(relay_pins[i], LOW);
    digitalWrite(LED_BUZ, LOW);
    delay(1000);
  }

  // Gap before repeating full cycle
  delay(3000);
}
