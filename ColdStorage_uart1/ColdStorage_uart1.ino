#include <Arduino.h>
#include <Wire.h>
#include "uRTCLib.h"
#include "config.h"
#include "uart_comm.h"
#include "control.h"
#include <EEPROM.h>
#include "storage.h"


uRTCLib rtc(0x68, 0x57);
HardwareSerial Serial1(PA_10, PA_9);   // RX , TX
//HardwareSerial Serial3(PB11, PB10);   // RX , TX For Uart second Port 
//extern bool testMode;



unsigned long waitSetStart = 0;
//bool setReceived = false;
bool askSent = false;
bool eepromFallbackUsed = false;   // prevents EEPROM reload loop


#define ESP_WAIT_MS   3000   // wait 3 sec for ESP
#define ASK_RETRY_MS  3000   // wait after ASKSET

#define LED_PIN PC_1

unsigned long ledTimer = 0;
unsigned long heartbeatTimer = 0;
uint8_t heartbeatCount = 0;
bool ledState = false;



// ---------- SYSTEM STATES ----------
enum SystemState {
  ST_BOOT,
  ST_WAIT_SET,
  ST_IDLE,
  ST_FAN_PRERUN,
  ST_COOLING,
  ST_HUM_CONTROL,
  ST_FAULT

};

// ---------- PERIODIC SETPOINT SYNC ----------
#define SET_SYNC_INTERVAL_MS (60UL * 60UL * 1000UL)  // 1 hour

unsigned long lastSetSyncTime = 0;


SystemState sysState = ST_BOOT;

// ---------- VARIABLES ----------
float temperature = 0;
float humidity    = 0;





void setup()
{
  Serial1.begin(9600);
  analogReadResolution(12);

  // All inputs temporary PULLUP (hardware undecided)
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

  loadSettings();
  

  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relay_pins[i], OUTPUT);
    digitalWrite(relay_pins[i], LOW);
  }

  Wire.begin();
  URTCLIB_WIRE.begin();
  pinMode(LED_PIN, OUTPUT);
digitalWrite(LED_PIN, LOW);

}


void loop()
{
  // TODO: replace with ADC
  temperature = readTemperature();
  humidity    = readHumidity();
//  humidity = 60.70;
process_uart_rx(&rtc);
delay(5);
  
  // ---------- PERIODIC ASKSET SYNC ----------
if (runEnable) {
  if (millis() - lastSetSyncTime >= SET_SYNC_INTERVAL_MS) {
    Serial1.println("{\"CMD\":\"ASKSET\"}");
    lastSetSyncTime = millis();
  }
}


  switch (sysState)
  {
    case ST_BOOT:
      allRelaysOff();
      waitSetStart = millis();
      askSent = false;
      resetSetReceived();
      setReceived = false;
      eepromFallbackUsed = false;   // ✅ ADD THIS
      lastSetSyncTime = millis(); 
      sysState = ST_WAIT_SET;
      break;




    case ST_IDLE:

    // 🔥 In normal mode only
    if (!testMode)
        allRelaysOff();

    resetFanPrerun();

    if (!runEnable)
        resetFaultLatch();

    // 🔥 Do not auto advance while in test mode
    if (runEnable && !testMode)
        sysState = ST_FAN_PRERUN;

    break;

    case ST_FAN_PRERUN:
    if (testMode) {
        break;
    }
      if (fanPrerunDone())
        sysState = ST_COOLING;
      break;

    case ST_COOLING:
        if (testMode) {
        // 🔥 DO NOTHING IN TEST MODE
        break;
    }

      if (faultDetected()) {
        sysState = ST_FAULT;
        break;
      }
      controlTemperature(temperature, tempSet);
//      controlHumidity(humidity, humSet);
      break;
    case ST_WAIT_SET:

      // ---- Send ASKSET once ----
      if (!askSent && millis() - waitSetStart >= ESP_WAIT_MS) {
        Serial1.println("{\"CMD\":\"ASKSET\"}");
        askSent = true;
        waitSetStart = millis();   // 🔴 RESET TIMER HERE
      }

      // ---- SET received from ESP ----
      if (setReceived) {
        sysState = ST_IDLE;
        break;
      }

      // ---- ESP timeout → EEPROM fallback ----
      if (askSent && millis() - waitSetStart >= ASK_RETRY_MS) {

        if (!eepromFallbackUsed) {
          loadSettings();
          eepromFallbackUsed = true;
          Serial1.println("{\"ACK\":\"SET\",\"SRC\":\"EEPROM\"}");
        }

        sysState = ST_IDLE;
        break;
      }

      break;


    case ST_FAULT:
      allRelaysOff();
      resetFanPrerun();
      if (!faultDetected())
        sysState = ST_IDLE;
      break;
  }

  const char *stateStr =
    (sysState == ST_COOLING)    ? "COOLING" :
    (sysState == ST_FAN_PRERUN) ? "FAN_PRERUN" :
    (sysState == ST_WAIT_SET)  ? "WAIT_SET" :
    (sysState == ST_FAULT)     ? "FAULT" :
    "IDLE";

// ---------- LED STATUS ENGINE (NON BLOCKING) ----------

unsigned long now = millis();

// 🔴 FAULT → LED SOLID ON
if (sysState == ST_FAULT) {
    digitalWrite(LED_PIN, HIGH);
}

// 🟡 WAIT / EEPROM / NO RUN → FAST BLINK
else if (!runEnable || sysState == ST_WAIT_SET) {

    if (now - ledTimer > 3000) {   // ~20 blinks/min
        ledTimer = now;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
}

// 🟢 NORMAL RUN → 7 heartbeat blinks per minute
else {

    if (now - heartbeatTimer > 60000) {
        heartbeatTimer = now;
        heartbeatCount = 14;   // 7 blinks = ON/OFF pairs
    }

    if (heartbeatCount > 0 && now - ledTimer > 200) {
        ledTimer = now;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        heartbeatCount--;
    }

    if (heartbeatCount == 0)
        digitalWrite(LED_PIN, LOW);
}



static uint32_t lastStatusMs = 0;

// send every 1000 ms non-blocking
if (!testMode && millis() - lastStatusMs >= 1000)
{
    lastStatusMs = millis();

    send_status_json(
        rtc,
        temperature,
        humidity,
        stateStr,
        relay_pins
    );
}

// keep UART responsive always
process_uart_rx(&rtc);

delay(5);
}
