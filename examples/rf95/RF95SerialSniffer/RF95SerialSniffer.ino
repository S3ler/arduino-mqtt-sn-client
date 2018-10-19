/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#if defined(ARDUINO_AVR_FEATHER32U4)
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#elif (ESP8266)
#define RFM95_CS 2
#define RFM95_INT 15
#elif (ARDUINO_AVR_UNO)
#define RFM95_CS 10
#define RFM95_RST 9
//#define RFM95_INT 7
#define RFM95_INT 2
#elif (ARDUINO_AVR_YUN)
#define RFM95_CS 10
#define RFM95_RST 9
//#define RFM95_INT 7
#define RFM95_INT 2
#else

#endif


/* for feather m0  
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
*/

/* for shield 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 7
*/


/* for ESP w/featherwing 
#define RFM95_CS  2    // "E"
#define RFM95_RST 16   // "D"
#define RFM95_INT 15   // "B"
*/

/* Feather 32u4 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
*/

/* Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"
*/

/* Teensy 3.x w/wing 
#define RFM95_RST     9   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     4    // "C"
*/

// Change to 434.0 or other frequency, must match RX's freq!
//#define FREQUENCY 868.0
#define FREQUENCY 434.0
#define TX_POWER  13
#define MODEM_CONFIG_CHOICE RH_RF95::Bw125Cr48Sf4096

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

void setup() 
{
  pinMode(LED, OUTPUT);     

  Serial.begin(115200);
  //while (!Serial);
  delay(100);

  Serial.println("RX Test!");
  

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Configure RH_RF95 driver after init
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  if (!rf95.setFrequency(FREQUENCY)) {
    Serial.print(F("setFrequency failed\n"));
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }
  Serial.print("Set Freq to: "); Serial.println(FREQUENCY);

  if (!rf95.setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE)) {
    Serial.print(F("setModemConfig failed\n"));
    while (true) {
      Serial.println(".");
      delay(1000);
    }
  }
  Serial.print("Set Modem Config to: "); Serial.println(RH_RF95::MODEM_CONFIG_CHOICE);

  rf95.setTxPower(TX_POWER);
  Serial.print("Set TX Power to: "); Serial.println(TX_POWER);

}

void loop()
{
  if (rf95.waitAvailableTimeout(10000))
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }else{
    Serial.println("Nothing");
  }
}

