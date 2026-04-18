#include <Arduino.h>

// UART3  ->  PB11 = RX , PB10 = TX
HardwareSerial Serial3(PB11, PB10);

String rxBuffer = "";

void setup()
{
  Serial.begin(115200);     // USB debug monitor
  Serial3.begin(9600);      // UART3 TTL

  Serial.println("UART3 RX/TX TEST READY");
}

void loop()
{
  // ---- Read from UART3 ----
  while (Serial3.available())
  {
    char c = Serial3.read();

    // echo to USB monitor
    Serial.write(c);

    // build line
    if (c == '\n')
    {
      Serial3.print("STM32 RECEIVED: ");
      Serial3.println(rxBuffer);   // send back response

      rxBuffer = "";
    }
    else
    {
      rxBuffer += c;
    }
  }
}



//continues give data code 
//#include <Arduino.h>
//
//// -------- UART3 on PB10 / PB11 --------
//// TX = PB10
//// RX = PB11
//HardwareSerial Serial3(PB11, PB10);  
//// NOTE: constructor = (RX, TX)
//
//void setup()
//{
//  // USB debug (optional)
//  Serial.begin(115200);
//
//  // Start UART3
//  Serial3.begin(9600);
//
//  Serial.println("UART3 TEST START");
//}
//
//void loop()
//{
//  // Send test string every 1 second
//  Serial3.println("STM32 UART3 WORKING OK");
//
//  Serial.println("Sent test packet on UART3");
//
//  delay(1000);
//}
