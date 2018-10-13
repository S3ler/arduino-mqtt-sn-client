// RF95Transmitter TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example RF95Sniffer

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
//#define RF95_FREQ 868.0
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define MODEM_CONFIG_CHOICE Bw31_25Cr48Sf512

void setup()
{

  pinMode(RFM95_INT, INPUT);

  Serial.begin(115200);
  while (!Serial);

  delay(100);

  Serial.println("TX Test!");

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  if (!rf95.setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE)) {
    Serial.println("Error initializing MODEM_CONFIG_CHOICE");
  }
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server

  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket + 13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[19] = 0;

  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  delay(1000);
}

