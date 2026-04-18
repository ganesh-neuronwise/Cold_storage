#pragma once
#include <Arduino.h>

// ---------- Persistent settings structure ----------
struct Settings {
  float tempSet;
  float humSet;
  uint8_t runEnable;
  uint32_t crc;
};

// ---------- EEPROM API ----------
void loadSettings();
void saveSettings();
void resetSettings();

// ---------- Global runtime variables ----------
extern float tempSet;
extern float humSet;
extern bool  runEnable;
