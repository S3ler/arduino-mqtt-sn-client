// RF95Sniffer RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example RF95Transmitter

#include <SPI.h>
#include <RH_RF95.h>
#include <Console.h>

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
#define TX_POWER  13
#define MODEM_CONFIG_CHOICE Bw125Cr48Sf4096


// Singleton instance of the radio driver
RH_RF95 rf95;

// Blinky on receipt
#define LED 13

//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01.
#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines.
//#define BAUDRATE 250000

void setup()
{
  pinMode(LED, OUTPUT);

  // Initialize Console and wait for port to open:
  Bridge.begin(BAUDRATE);
  Console.begin();

  // Wait for Console port to connect
  while (!Console);

  Console.println("RX Test!");


  while (!rf95.init()) {
    Console.println("LoRa radio init failed");
    while (1);
  }
  Console.println("LoRa radio init OK!");

  // Configure RH_RF95 driver after init
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  if (!rf95.setFrequency(FREQUENCY)) {
    Console.print(F("setFrequency failed\n"));
    while (true) {
      Console.println(".");
      delay(1000);
    }
  }
  Console.print("Set Freq to: "); Console.println(FREQUENCY);

  if (!rf95.setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE)) {
    Console.print(F("setModemConfig failed\n"));
    while (true) {
      Console.println(".");
      delay(1000);
    }
  }
  Console.print("Set Modem Config to: "); Console.println(RH_RF95::MODEM_CONFIG_CHOICE);

  rf95.setTxPower(TX_POWER);
  Console.print("Set TX Power to: "); Console.println(TX_POWER);

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
      Console.print("Got: ");
      Console.println((char*)buf);
      Console.print("RSSI: ");
      Console.println(rf95.lastRssi(), DEC);
      digitalWrite(LED, LOW);
    }
    else
    {
      Console.println("Receive failed");
    }
    }else{
    Console.println("Nothing");
    }
}
