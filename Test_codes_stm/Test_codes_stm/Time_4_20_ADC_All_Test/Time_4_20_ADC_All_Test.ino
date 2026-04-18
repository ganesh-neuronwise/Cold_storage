#include <Arduino.h>
#include <Wire.h>
#include "uRTCLib.h"

// ---------- UART ----------
HardwareSerial Serial3(PA_10, PA_9); // RX, TX

// ---------- RTC ----------
uRTCLib rtc(0x68, 0x57);

// ---------- ANALOG PINS ----------
#define TEMP1_CH PA_2
#define TEMP2_CH PA_3

#define ADC0_CH  PA_0
#define ADC4_CH  PA_4
#define ADC5_CH  PA_5

// ---------- ADC SETTINGS ----------
float VREF = 3.3f;
float ADC_MAX = 4095.0f;

// ---------- 4-20 mA SETTINGS ----------
float SHUNT_OHM = 150.0f;

// calibration parameters
float CURRENT_GAIN = 1.10f;
float TEMP_SLOPE   = 14.96f;
float TEMP_OFFSET  = -122.4f;


// ---------- READ RAW ADC ----------
uint16_t readADC(uint8_t pin)
{
  return analogRead(pin);
}


// ---------- ADC → VOLTAGE ----------
float adcToVoltage(uint16_t adc)
{
  return (adc * VREF) / ADC_MAX;
}


// ---------- VOLTAGE → CURRENT ----------
float voltageToCurrent(float voltage)
{
  float current = (voltage / SHUNT_OHM) * 1000.0f;
  current *= CURRENT_GAIN;
  return current;
}


// ---------- CURRENT → TEMP ----------
float currentToTemp(float current)
{
  return (current * TEMP_SLOPE) + TEMP_OFFSET;
}


// ---------- PRINT TEMP LOOP ----------
void printTempLoop(const char *name, uint8_t pin)
{
  uint16_t adc = readADC(pin);
  float voltage = adcToVoltage(adc);
  float current = voltageToCurrent(voltage);
  float temp = currentToTemp(current);

  Serial3.println(name);
  Serial3.print("ADC: "); Serial3.println(adc);
  Serial3.print("VOLT: "); Serial3.print(voltage,3); Serial3.println(" V");
  Serial3.print("CURR: "); Serial3.print(current,2); Serial3.println(" mA");
  Serial3.print("TEMP: "); Serial3.print(temp,2); Serial3.println(" C");
  Serial3.println();
}


// ---------- PRINT DIRECT ADC ----------
void printADC(const char *name, uint8_t pin)
{
  uint16_t adc = analogRead(pin);
  float voltage = adcToVoltage(adc);

  Serial3.print(name);
  Serial3.print(": ");
  Serial3.print(voltage,3);
  Serial3.println(" V");
}


void setup()
{
  Serial3.begin(9600);
  analogReadResolution(12);

  Wire.begin();
  URTCLIB_WIRE.begin();

  Serial3.println("STM SENSOR DIAGNOSTIC START");
}


void loop()
{
  rtc.refresh();

  Serial3.println();
  Serial3.print("TIME: ");

  Serial3.print(rtc.year()); Serial3.print("-");
  Serial3.print(rtc.month()); Serial3.print("-");
  Serial3.print(rtc.day()); Serial3.print(" ");

  Serial3.print(rtc.hour()); Serial3.print(":");
  Serial3.print(rtc.minute()); Serial3.print(":");
  Serial3.println(rtc.second());

  Serial3.println();

  // ---------- TEMP LOOPS ----------
  printTempLoop("TEMP LOOP PA2", TEMP1_CH);
  printTempLoop("TEMP LOOP PA3", TEMP2_CH);

  // ---------- ADC INPUTS ----------
  printADC("ADC PA0", ADC0_CH);
  printADC("ADC PA4", ADC4_CH);
  printADC("ADC PA5", ADC5_CH);

  Serial3.println("-----------------------------");

  delay(2000);
}
