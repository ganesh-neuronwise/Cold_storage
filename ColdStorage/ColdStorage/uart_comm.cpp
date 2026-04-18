#include "uart_comm.h"
#include "config.h"
#include "control.h"
#include "storage.h"

//extern HardwareSerial Serial3;

static String rxBuf;
volatile bool setReceived = false;

void process_uart_rx()
{
  while (Serial3.available())
  {
    char c = Serial3.read();

    if (c == '\n') {

      if (rxBuf.indexOf("\"CMD\":\"SET\"") >= 0) {

        int tPos = rxBuf.indexOf("\"TSP\":");
        int hPos = rxBuf.indexOf("\"HSP\":");

        if (tPos > 0 && hPos > 0) {
          tempSet = rxBuf.substring(tPos + 6).toFloat();
          humSet  = rxBuf.substring(hPos + 6).toFloat();
          saveSettings();
          setReceived = true;

          Serial3.print("{\"ACK\":\"SET\",\"SRC\":\"ESP\",\"TSP\":");
          Serial3.print(tempSet,1);
          Serial3.print(",\"HSP\":");
          Serial3.print(humSet,1);
          Serial3.println("}");
        }
      }
      else if (rxBuf.indexOf("\"CMD\":\"RUN\"") >= 0) {

        if (faultDetected())
          Serial3.println("{\"ACK\":\"RUN\",\"ERR\":\"FAULT\"}");
        else {
          runEnable = true;
          saveSettings();
          Serial3.println("{\"ACK\":\"RUN\",\"ST\":\"FAN_PRERUN\"}");
        }
      }
      else if (rxBuf.indexOf("\"CMD\":\"STOP\"") >= 0) {
        runEnable = false;
        saveSettings();
        Serial3.println("{\"ACK\":\"STOP\",\"ST\":\"IDLE\"}");
      }

      rxBuf = "";
    }
    else rxBuf += c;
  }
}

void resetSetReceived() { setReceived = false; }

void send_status_json(
  uRTCLib &rtc,
  float T, float H,
  const char *stateStr,
  const uint8_t *relay_pins
)
{
  rtc.refresh();

  Serial3.print("{\"DT\":\"");
  Serial3.print(rtc.year()); Serial3.print('-');
  Serial3.print(rtc.month());Serial3.print('-');
  Serial3.print(rtc.day());  Serial3.print(' ');
  Serial3.print(rtc.hour()); Serial3.print(':');
  Serial3.print(rtc.minute());Serial3.print(':');
  Serial3.print(rtc.second());
  Serial3.print("\",");

  Serial3.print("\"T\":"); Serial3.print(T,1); Serial3.print(',');
  Serial3.print("\"H\":"); Serial3.print(H,1); Serial3.print(',');

  Serial3.print("\"C1P\":");  Serial3.print(readInput(IN_C1, POL_C1_P)); Serial3.print(',');
  Serial3.print("\"C1HP\":"); Serial3.print(readInput(IN_C1_HP, POL_C1_HP)); Serial3.print(',');
  Serial3.print("\"C1LP\":"); Serial3.print(readInput(IN_C1_LP, POL_C1_LP)); Serial3.print(',');
  Serial3.print("\"C1OL\":"); Serial3.print(readInput(IN_C1_OL, POL_C1_OL)); Serial3.print(',');

  Serial3.print("\"C2P\":");  Serial3.print(readInput(IN_C2, POL_C2_P)); Serial3.print(',');
  Serial3.print("\"C2HP\":"); Serial3.print(readInput(IN_C2_HP, POL_C2_HP)); Serial3.print(',');
  Serial3.print("\"C2LP\":"); Serial3.print(readInput(IN_C2_LP, POL_C2_LP)); Serial3.print(',');
  Serial3.print("\"C2OL\":"); Serial3.print(readInput(IN_C2_OL, POL_C2_OL)); Serial3.print(',');

  Serial3.print("\"HF1P\":"); Serial3.print(readInput(IN_HF1_P, POL_HF1_P)); Serial3.print(',');
  Serial3.print("\"HF2P\":"); Serial3.print(readInput(IN_HF2_P, POL_HF2_P)); Serial3.print(',');

  Serial3.print("\"F1P\":"); Serial3.print(readInput(IN_FAN1_P, POL_FAN1_P)); Serial3.print(',');
  Serial3.print("\"F2P\":"); Serial3.print(readInput(IN_FAN2_P, POL_FAN2_P)); Serial3.print(',');

  Serial3.print("\"HT1P\":"); Serial3.print(readInput(IN_HT1_P, POL_HT1_P)); Serial3.print(',');
  Serial3.print("\"HT1OL\":"); Serial3.print(readInput(IN_HT1_OL, POL_HT1_OL)); Serial3.print(',');
  Serial3.print("\"HT2P\":"); Serial3.print(readInput(IN_HT2_P, POL_HT2_P)); Serial3.print(',');
  Serial3.print("\"HT2OL\":"); Serial3.print(readInput(IN_HT2_OL, POL_HT2_OL)); Serial3.print(',');

  Serial3.print("\"DR\":"); Serial3.print(readInput(IN_DOOR, POL_DOOR)); Serial3.print(',');
  Serial3.print("\"PH\":"); Serial3.print(readInput(IN_PHASE, POL_PHASE)); Serial3.print(',');

  Serial3.print("\"R\":[");
  for (int i = 0; i < RELAY_COUNT; i++) {
    Serial3.print(digitalRead(relay_pins[i]));
    if (i < RELAY_COUNT - 1) Serial3.print(',');
  }
  Serial3.print("],");

  Serial3.print("\"ST\":\"");
  Serial3.print(stateStr);
  Serial3.println("\"}");
}
