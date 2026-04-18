#include "uart_comm.h"
#include "config.h"
#include "control.h"
#include "storage.h"


//extern HardwareSerial Serial1;

static String rxBuf;
//volatile bool setReceived = false;

//extern bool testMode;
volatile bool setReceived = false;
bool testMode = false;


void process_uart_rx()
{
  while (Serial1.available())
  {
    char c = Serial1.read();

    if (c == '\n') {

      if (rxBuf.indexOf("\"CMD\":\"SET\"") >= 0) {

        int tPos = rxBuf.indexOf("\"TSP\":");
        int hPos = rxBuf.indexOf("\"HSP\":");

        if (tPos > 0 && hPos > 0) {
          tempSet = rxBuf.substring(tPos + 6).toFloat();
          humSet  = rxBuf.substring(hPos + 6).toFloat();
          saveSettings();
          setReceived = true;

          Serial1.print("{\"ACK\":\"SET\",\"SRC\":\"ESP\",\"TSP\":");
          Serial1.print(tempSet,1);
          Serial1.print(",\"HSP\":");
          Serial1.print(humSet,1);
          Serial1.println("}");
        }
      }
else if (rxBuf.indexOf("\"CMD\":\"RUN\"") >= 0) {

    testMode = false;   // 🔥 ADD THIS

    if (faultDetected())
        Serial1.println("{\"ACK\":\"RUN\",\"ERR\":\"FAULT\"}");
    else {
        runEnable = true;
        saveSettings();
        Serial1.println("{\"ACK\":\"RUN\",\"MODE\":\"AUTO\"}");
    }
}
      else if (rxBuf.indexOf("\"CMD\":\"STOP\"") >= 0) {
        runEnable = false;
        testMode = true;
        
        allRelaysOff();
        saveSettings();
        Serial1.println("{\"ACK\":\"STOP\",\"MODE\":\"TEST\"}");
      }
      else if (rxBuf.indexOf("\"CMD\":\"TEST\"") >= 0 && testMode)
{
    int cPos = rxBuf.indexOf("\"C\":\"");
    int sPos = rxBuf.indexOf("\"S\":");

    if (cPos > 0 && sPos > 0)
    {
        int endPos = rxBuf.indexOf("\"", cPos + 5);
String comp = rxBuf.substring(cPos + 5, endPos);
        int state = rxBuf.substring(sPos + 4).toInt();

        uint8_t pin = 255;
        uint8_t idx = 255;

        if (comp == "C1") { pin = R_COMP1; idx = 0; }
        else if (comp == "C2") { pin = R_COMP2; idx = 1; }
        else if (comp == "HF1") { pin = R_HF1; idx = 2; }
        else if (comp == "HF2") { pin = R_HF2; idx = 3; }
        else if (comp == "FAN1") { pin = R_FAN1; idx = 4; }
        else if (comp == "FAN2") { pin = R_FAN2; idx = 5; }
        else if (comp == "HT1") { pin = R_HT1; idx = 6; }
        else if (comp == "HT2") { pin = R_HT2; idx = 7; }

        if (pin != 255)
        {
            if (state)
                digitalWrite(pin, HIGH);
            else
                digitalWrite(pin, LOW);

            Serial1.print("{\"RESP\":\"");
            Serial1.print(state);
            Serial1.println("\"}");
        }
    }
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

  Serial1.print("{\"DT\":\"");
  Serial1.print(rtc.year()); Serial1.print('-');
  Serial1.print(rtc.month());Serial1.print('-');
  Serial1.print(rtc.day());  Serial1.print(' ');
  Serial1.print(rtc.hour()); Serial1.print(':');
  Serial1.print(rtc.minute());Serial1.print(':');
  Serial1.print(rtc.second());
  Serial1.print("\",");

  Serial1.print("\"T\":"); Serial1.print(T,1); Serial1.print(',');
  Serial1.print("\"H\":"); Serial1.print(H,1); Serial1.print(',');

  Serial1.print("\"C1P\":");  Serial1.print(readInput(IN_C1, POL_C1_P)); Serial1.print(',');
  Serial1.print("\"C1HP\":"); Serial1.print(readInput(IN_C1_HP, POL_C1_HP)); Serial1.print(',');
  Serial1.print("\"C1LP\":"); Serial1.print(readInput(IN_C1_LP, POL_C1_LP)); Serial1.print(',');
  Serial1.print("\"C1OL\":"); Serial1.print(readInput(IN_C1_OL, POL_C1_OL)); Serial1.print(',');

  Serial1.print("\"C2P\":");  Serial1.print(readInput(IN_C2, POL_C2_P)); Serial1.print(',');
  Serial1.print("\"C2HP\":"); Serial1.print(readInput(IN_C2_HP, POL_C2_HP)); Serial1.print(',');
  Serial1.print("\"C2LP\":"); Serial1.print(readInput(IN_C2_LP, POL_C2_LP)); Serial1.print(',');
  Serial1.print("\"C2OL\":"); Serial1.print(readInput(IN_C2_OL, POL_C2_OL)); Serial1.print(',');

  Serial1.print("\"HF1P\":"); Serial1.print(readInput(IN_HF1_P, POL_HF1_P)); Serial1.print(',');
  Serial1.print("\"HF2P\":"); Serial1.print(readInput(IN_HF2_P, POL_HF2_P)); Serial1.print(',');

  Serial1.print("\"F1P\":"); Serial1.print(readInput(IN_FAN1_P, POL_FAN1_P)); Serial1.print(',');
  Serial1.print("\"F2P\":"); Serial1.print(readInput(IN_FAN2_P, POL_FAN2_P)); Serial1.print(',');

  Serial1.print("\"HT1P\":"); Serial1.print(readInput(IN_HT1_P, POL_HT1_P)); Serial1.print(',');
  Serial1.print("\"HT1OL\":"); Serial1.print(readInput(IN_HT1_OL, POL_HT1_OL)); Serial1.print(',');
  Serial1.print("\"HT2P\":"); Serial1.print(readInput(IN_HT2_P, POL_HT2_P)); Serial1.print(',');
  Serial1.print("\"HT2OL\":"); Serial1.print(readInput(IN_HT2_OL, POL_HT2_OL)); Serial1.print(',');

  Serial1.print("\"DR\":"); Serial1.print(readInput(IN_DOOR, POL_DOOR)); Serial1.print(',');
  Serial1.print("\"PH\":"); Serial1.print(readInput(IN_PHASE, POL_PHASE)); Serial1.print(',');

  Serial1.print("\"R\":[");
  for (int i = 0; i < RELAY_COUNT; i++) {
    Serial1.print(digitalRead(relay_pins[i]));
    if (i < RELAY_COUNT - 1) Serial1.print(',');
  }
  Serial1.print("],");

  Serial1.print("\"ST\":\"");
  Serial1.print(stateStr);
  Serial1.println("\"}");
}
