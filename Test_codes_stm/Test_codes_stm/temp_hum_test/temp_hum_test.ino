#include <Arduino.h>
#include <Wire.h>
#include "uRTCLib.h"

// ---------- Pins ----------
#define HUM1_CH1    PA_0
#define HUM2_CH2    PA_1
#define PT100_CH1   PA_2
#define PT100_CH2   PA_3
#define LED_BUZ     PC_1

// ---------- RTC ----------
uRTCLib rtc(0x68, 0x57);

// ---------- UART ----------
HardwareSerial Serial1(PA_10, PA_9);

// ---------- Constants ----------
#define ADC_MAX     4095.0
#define VREF        3.3
#define SHUNT_OHM   150.0

// ---------- Variables ----------
uint16_t hum_adc[2];
uint16_t pt_adc[2];
float hum_percent[2];
float temp_c[2];

// ---------- Setup ----------
void setup()
{
  analogReadResolution(12);
  Serial1.begin(9600);

  pinMode(HUM1_CH1, INPUT_ANALOG);
  pinMode(HUM2_CH2, INPUT_ANALOG);
  pinMode(PT100_CH1, INPUT_ANALOG);
  pinMode(PT100_CH2, INPUT_ANALOG);

  pinMode(LED_BUZ, OUTPUT);

  Wire.begin();
  URTCLIB_WIRE.begin();
  rtc.refresh();

  Serial1.println("@BOOT,PHASE2_CONVERSION#");
}

// ---------- Read ADC ----------
void read_adc()
{
  hum_adc[0] = analogRead(HUM1_CH1);
  hum_adc[1] = analogRead(HUM2_CH2);
  pt_adc[0]  = analogRead(PT100_CH1);
  pt_adc[1]  = analogRead(PT100_CH2);
}

// ---------- Convert Humidity ----------
void convert_humidity()
{
  for (int i = 0; i < 2; i++) {
    hum_percent[i] = (hum_adc[i] / ADC_MAX) * 100.0;
    if (hum_percent[i] < 0) hum_percent[i] = 0;
    if (hum_percent[i] > 100) hum_percent[i] = 100;
  }
}

// ---------- Convert Temperature ----------
float convert_pt100(uint16_t adc)
{
  float voltage = (adc * VREF) / ADC_MAX;
  float current_mA = (voltage / SHUNT_OHM) * 1000.0;
  float temp = ((current_mA - 4.0) / 16.0) * 250.0 - 50.0;
  return temp;
}

void convert_temperature()
{
  temp_c[0] = convert_pt100(pt_adc[0]);
  temp_c[1] = convert_pt100(pt_adc[1]);
}

// ---------- UART Send ----------
void send_uart()
{
  rtc.refresh();

  Serial1.print('@');
  Serial1.print("T=");
  Serial1.print(rtc.hour());
  Serial1.print(':');
  Serial1.print(rtc.minute());
  Serial1.print(':');
  Serial1.print(rtc.second());

  Serial1.print(",H1=");
  Serial1.print(hum_percent[0], 1);
  Serial1.print(",H2=");
  Serial1.print(hum_percent[1], 1);

  Serial1.print(",TC1=");
  Serial1.print(temp_c[0], 1);
  Serial1.print(",TC2=");
  Serial1.print(temp_c[1], 1);

  Serial1.println('#');
}

// ---------- LED ----------
void led_blink()
{
  digitalWrite(LED_BUZ, HIGH);
  delay(500);
  digitalWrite(LED_BUZ, LOW);
  delay(500);
}

// ---------- Loop ----------
void loop()
{
  read_adc();
  convert_humidity();
  convert_temperature();
  send_uart();
  led_blink();
}
