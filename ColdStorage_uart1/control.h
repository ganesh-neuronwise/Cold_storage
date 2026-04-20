#pragma once
#include <Arduino.h>

// ---------- INPUT READ HELPER ----------
//this is old logic 
static inline uint8_t readInput(uint8_t pin, uint8_t polarity)
{
  uint8_t raw = digitalRead(pin);
  return polarity ? raw : !raw;   // returns LOGICAL state
}
//static inline uint8_t readInput(uint8_t pin, uint8_t polarity)
//{
//  return digitalRead(pin);   // HIGH = 1, LOW = 0
//}

// ---- System control ----
void allRelaysOff();
bool faultDetected();

// ---- Fan / compressor timing ----
bool fanPrerunDone();
void resetFanPrerun();

// ---- Control ----
void controlTemperature(float T, float Tsp);
//void controlHumidity(float H, float Hsp);

// ---- Compressor helpers ----
bool canStartCompressor(uint8_t idx);
bool canStopCompressor(uint8_t idx);
void startCompressor(uint8_t pin, uint8_t idx);
void stopCompressor(uint8_t pin, uint8_t idx);

// ---- Sensors ----
float readHumidity();
float readTemperature();
bool tempSensorFault(float current_mA);

// reset the faults 
void resetFaultLatch();
