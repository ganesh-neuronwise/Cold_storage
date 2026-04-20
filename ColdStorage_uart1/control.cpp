#include "control.h"
#include "config.h"

/*
  CONTROL PHILOSOPHY (FINAL):

  - Temperature is PRIMARY
  - Humidity is SECONDARY
  - Fans always ON when RUN
  - Compressor is the only real dehumidifier
  - Heater is used ONLY:
      * to raise temperature
      * OR to balance temperature during dehumidification
  - "P" inputs are DISPLAY ONLY (never used here)
  - Compressor health depends ONLY on HP / LP / OL
  - Heater health depends ONLY on OL
*/

/* ================= CONFIGURABLE PARAMETERS ================= */

// Temperature hysteresis (example: ±1°C)
static float TEMP_ON_DT   = 1.0f;   // start cooling above Tsp + 1
static float TEMP_OFF_DT  = 1.0f;   // stop cooling below Tsp - 1

// Compressor staging
static float TEMP_STAGE2_DT = 5.0f; // above this delta → 2 compressors

// Humidity control
static float HUM_DB         = 2.0f; // ± deadband
static float HUM_STAGE2_DH  = 10.0f; // large humidity error → 2 humidifiers

// Heater staging
static float HEAT_STAGE2_DT = 5.0f;   // if Tsp - T > 5°C → both heaters


/* ================= TIMERS ================= */

static unsigned long fanPrerunStart = 0;
static bool fanPrerunActive = false;

static unsigned long compOnTime[2]  = {0, 0};
static unsigned long compOffTime[2] = {0, 0};

#define DEVICE_MIN_MS 30000UL   // minimum ON/OFF for all relays

static unsigned long relayLastChange[RELAY_COUNT] = {0};

// ---------- COMPRESSOR STRICT TIMING ----------
static unsigned long COMP_MIN_ON_MS  = 3UL * 60UL * 1000UL;   // 3 minutes
static unsigned long COMP_MIN_OFF_MS = 3UL * 60UL * 1000UL;   // 3 minutes




/* ================= LOW LEVEL RELAY SAFETY ================= */

static bool canChangeRelay(uint8_t idx)
{
  return (millis() - relayLastChange[idx]) >= DEVICE_MIN_MS;
}

static void relayOnSafe(uint8_t pin, uint8_t idx)
{
  if (digitalRead(pin) == LOW && canChangeRelay(idx)) {
    digitalWrite(pin, HIGH);
    relayLastChange[idx] = millis();
  }
}

static void relayOffSafe(uint8_t pin, uint8_t idx)
{
  if (digitalRead(pin) == HIGH && canChangeRelay(idx)) {
    digitalWrite(pin, LOW);
    relayLastChange[idx] = millis();
  }
}

// for reset the faults 
static bool latchedFault = false;


/* ================= GLOBAL RELAY OFF ================= */

void allRelaysOff()
{
  for (int i = 0; i < RELAY_COUNT; i++) {
    digitalWrite(relay_pins[i], LOW);
    relayLastChange[i] = millis();
  }
}

/* ================= FAN PRE-RUN ================= */

bool fanPrerunDone()
{
  if (!fanPrerunActive) {
    fanPrerunActive = true;
    fanPrerunStart = millis();
    relayOnSafe(R_FAN1, 4);
    relayOnSafe(R_FAN2, 5);
  }
  return (millis() - fanPrerunStart) >= FAN_PRERUN_MS;
}

void resetFanPrerun()
{
  fanPrerunActive = false;
}

/* ================= HEALTH CHECKS ================= */

static bool comp1Healthy()
{
  return !( readInput(IN_C1_HP, POL_C1_HP) ||
            readInput(IN_C1_LP, POL_C1_LP) ||
            readInput(IN_C1_OL, POL_C1_OL) );
}

static bool comp2Healthy()
{
  return !( readInput(IN_C2_HP, POL_C2_HP) ||
            readInput(IN_C2_LP, POL_C2_LP) ||
            readInput(IN_C2_OL, POL_C2_OL) );
}

static bool heater1Healthy()
{
  return !readInput(IN_HT1_OL, POL_HT1_OL);
}

static bool heater2Healthy()
{
  return !readInput(IN_HT2_OL, POL_HT2_OL);
}


/* ================= FAULT DETECTION ================= */

bool faultDetected()
{
    // ---- ONLY REAL SYSTEM FAULT ----
    if (readInput(IN_PHASE, POL_PHASE)) {
        return true;    // immediate stop
    }

    // Do NOT latch compressor/heater OL as system fault
    return false;
}


/* ================= COMPRESSOR TIMING ================= */

bool canStartCompressor(uint8_t idx)
{
    return (millis() - compOffTime[idx]) >= COMP_MIN_OFF_MS;
}

bool canStopCompressor(uint8_t idx)
{
    return (millis() - compOnTime[idx]) >= COMP_MIN_ON_MS;
}


void startCompressor(uint8_t pin, uint8_t idx)
{
    if (digitalRead(pin) == LOW && canStartCompressor(idx)) {
        relayOnSafe(pin, idx);
        compOnTime[idx] = millis();
    }
}


void stopCompressor(uint8_t pin, uint8_t idx)
{
    if (digitalRead(pin) == HIGH && canStopCompressor(idx)) {
        relayOffSafe(pin, idx);
        compOffTime[idx] = millis();
    }
}


/* ================= TEMPERATURE + HUMIDITY CONTROL ================= */

void controlTemperature(float T, float Tsp)
{
  /* Fans always ON */
  relayOnSafe(R_FAN1, 4);
  relayOnSafe(R_FAN2, 5);

  float T_ON  = Tsp + TEMP_ON_DT;
  float T_OFF = Tsp - TEMP_OFF_DT;

  bool needCool = (T >= T_ON);
  bool stopCool = (T <= T_OFF);

  float deltaT = T - Tsp;

  bool needDehumidify = false;
  bool needHumidify   = false;

  /* Humidity evaluation */
  extern float humSet;
  float H = readHumidity();

  if (H > humSet + HUM_DB) needDehumidify = true;
  else if (H < humSet - HUM_DB) needHumidify = true;

  /* ---------- COOLING FOR TEMPERATURE ---------- */
  if (needCool) {

    relayOffSafe(R_HT1, 6);
    relayOffSafe(R_HT2, 7);

    bool c1 = comp1Healthy();
    bool c2 = comp2Healthy();

    if (deltaT > TEMP_STAGE2_DT && c1 && c2) {
      startCompressor(R_COMP1, 0);
      startCompressor(R_COMP2, 1);
    }
    else {
      if (c1) startCompressor(R_COMP1, 0);
      else if (c2) startCompressor(R_COMP2, 1);
    }
  }

/* ---------- DEHUMIDIFICATION AT TEMP OK ---------- */
if (!needCool && !stopCool && needDehumidify) {

    bool c1 = comp1Healthy();
    bool c2 = comp2Healthy();

    if (c1) startCompressor(R_COMP1, 0);
    else if (c2) startCompressor(R_COMP2, 1);

    bool h1 = heater1Healthy();
    bool h2 = heater2Healthy();

    // Heater balancing during dehumidification
    if (h1) {
        relayOnSafe(R_HT1, 6);
        relayOffSafe(R_HT2, 7);
    }
    else if (h2) {
        relayOnSafe(R_HT2, 7);
        relayOffSafe(R_HT1, 6);
    }
}

  /* ---------- STOP COOLING ---------- */
  if (stopCool) {
    stopCompressor(R_COMP1, 0);
    stopCompressor(R_COMP2, 1);
  }

  /* ---------- HEATING ONLY ---------- */
/* ---------- HEATING ONLY (STAGED + FAILOVER) ---------- */

float heatDelta = Tsp - T;

bool h1 = heater1Healthy();
bool h2 = heater2Healthy();

// -------- SMALL ERROR (SAVE POWER) --------
if (heatDelta < 4.5f) {
    // do nothing
    relayOffSafe(R_HT1, 6);
    relayOffSafe(R_HT2, 7);
}

// -------- MEDIUM ERROR --------
else if (heatDelta < 8.0f) {
    if (h1) {
        relayOnSafe(R_HT1, 6);
        relayOffSafe(R_HT2, 7);
    } else if (h2) {
        relayOnSafe(R_HT2, 7);
        relayOffSafe(R_HT1, 6);
    }
}

// -------- LARGE ERROR --------
else {
    if (h1 && h2) {
        relayOnSafe(R_HT1, 6);
        relayOnSafe(R_HT2, 7);
    }
}


  /* ---------- HUMIDITY ONLY ---------- */
// Humidifier allowed ONLY when:
// - we are NOT cooling
// - we are NOT dehumidifying

if (needHumidify) {

    // BLOCK only if actively dehumidifying
    if (needDehumidify) {
        relayOffSafe(R_HF1, 2);
        relayOffSafe(R_HF2, 3);
    }
    else {

        relayOnSafe(R_HF1, 2);

        if ((humSet - H) > HUM_STAGE2_DH)
            relayOnSafe(R_HF2, 3);
        else
            relayOffSafe(R_HF2, 3);
    }
}
else {
    relayOffSafe(R_HF1, 2);
    relayOffSafe(R_HF2, 3);
}
}
/* ================= SENSOR READINGS ================= */

float readHumidity()
{
  uint16_t adc = analogRead(HUM_CH);
  float voltage = (adc * 3.3f) / 4095.0f;
  float rh = (voltage / 3.3f) * 100.0f;

  if (rh < 0) rh = 0;
  if (rh > 100) rh = 100;
  return rh;
}

float readTemperature()
{
  uint16_t adc = analogRead(TEMP_CH);

  // ADC → Voltage
  float voltage = (adc * 3.3f) / 4095.0f;

  // Voltage → Current (mA)
  float current_mA = (voltage / SHUNT_OHM) * 1000.0f;

  // ===== ADC BOARD CALIBRATION =====
  current_mA *= 1.10f;   // your STM ADC correction

  // ===== FINAL PT100 TRANSMITTER CALIBRATION =====
  float temp = (current_mA * 14.96f) - 122.4f;

  // Safety clamp
  if (temp < -50) temp = -50;
  if (temp > 150) temp = 150;

  return temp;
}




void resetFaultLatch()
{
  latchedFault = false;
}
