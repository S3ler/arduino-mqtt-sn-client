/*
* The MIT License (MIT)
*
* Copyright (C) 2018 Gabriel Nikol
*/

#include "RHGenericDriverSocket.h"
#include "MqttSnClient.h"
#include <RH_RF95.h>
#include <Console.h>

#define RFM95_CS 10
#define RFM95_RST 9
//#define RFM95_INT 7
#define RFM95_INT 2

#define FREQUENCY 434.0
#define TX_POWER  13
#define MODEM_CONFIG_CHOICE Bw125Cr48Sf4096

#define OWN_ADDRESS 2

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHGenericDriverSocket rhGenericDriverSocket(&rf95, OWN_ADDRESS);
TransmissionProtocolUartBridge <RHGenericDriverSocket> transmissionProtocolUartBridge(&Console, rhGenericDriverSocket);

//If you use Dragino IoT Mesh Firmware, uncomment below lines.
//For product: LG01.
#define BAUDRATE 115200

//If you use Dragino Yun Mesh Firmware , uncomment below lines.
//#define BAUDRATE 250000

void setup() {
  // Initialize Console and wait for port to open:
  Bridge.begin(BAUDRATE);
  Console.begin();

  // Wait for Console port to connect
  while (!Console);

  if (!transmissionProtocolUartBridge.begin()) {
    Console.print("Could not initialize RHRF95DatagramUartBridge ");
    while (true) {
      Console.println(".");
      delay(1000);
    }
  }

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

  Console.println("RF95GenericDriverSocketConsoleBridge v0.1 ready!");
}

void loop() {
  if (Console.available() > 0) {
    char c = Console.read();
    transmissionProtocolUartBridge.putChar(c);
  }
  transmissionProtocolUartBridge.loop();
}