#include "storage.h"
#include <EEPROM.h>

// EEPROM base address
#define EEPROM_ADDR  0

// defaults
#define DEF_TEMP_SP  5.0
#define DEF_HUM_SP   70.0

// existing globals
float tempSet = DEF_TEMP_SP;
float humSet  = DEF_HUM_SP;
bool  runEnable = false;

// ---------- CRC ----------
static uint32_t calcCRC(const Settings &s)
{
  const uint8_t *p = (const uint8_t *)&s;
  uint32_t crc = 0;

  for (size_t i = 0; i < sizeof(Settings) - sizeof(uint32_t); i++)
    crc = crc * 31 + p[i];

  return crc;
}

// ---------- LOAD ----------
void loadSettings()
{
  Settings s;
  EEPROM.get(EEPROM_ADDR, s);

  uint32_t crc = calcCRC(s);

  if (crc != s.crc) {
    // invalid / first boot
    resetSettings();
    saveSettings();
    return;
  }

  tempSet   = s.tempSet;
  humSet    = s.humSet;
  runEnable = s.runEnable;
}

// ---------- SAVE ----------
void saveSettings()
{
  Settings s;
  s.tempSet   = tempSet;
  s.humSet    = humSet;
  s.runEnable = runEnable;
  s.crc       = calcCRC(s);

  EEPROM.put(EEPROM_ADDR, s);   // writes immediately on STM32
}


// ---------- RESET ----------
void resetSettings()
{
  tempSet   = DEF_TEMP_SP;
  humSet    = DEF_HUM_SP;
  runEnable = false;
}
