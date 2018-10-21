# arduino-mqtt-sn-client
Arduino based MQTT-SN Client

### Models
 * ESP8266  &#x2705;
 * ESP32 &#x274E;
 * Arduino Mega &#x274E;
 * Arduino Uno &#x2705;

### Transmission Technology to Platform Matrix
|   	| UDP  	| TCP  	| Ethernet  	| WiFi  	| ZigBee  	| LoRa  	| BLE  	|
|---	|---	|---	|---	|---	|---	|---	|---	|
| Arduino ESP8266 	| &#x2705;  	| &#x2705;  	|         	| &#x2705;  	| &#x274E;\*  	| &#x274E;\*  	|       	|
| Arduino ESP32 	| &#x274E;  	| &#x274E;  	|         	| &#x2705;  	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*	|
| Arduino Mega 	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*  	|           	| &#x274E;\*  	| &#x274E;\*  	|         	|
| Arduino Uno 	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*  	|           	| &#x274E;\*  	| &#x2705;\*  	|         	|

\* needs additional transmission hardware

##### Legend: 
* &#x2705; implemented and tested
* &#x274E; not implemented yet

## Getting Started

### ESP8266 (UDP)
For UDP we use the `WiFiUdpMqttSnClient` example with a `ESP8266` (e.g. NodeMCU v1.0).
#### Hardware
You do not need additional hardware as mentioned in [Transmission Technology to Platform Matrix](#transmission-technology-to-platform-matrix).
We tested it with the following Hardware:
 * NodeMCU v1.0
#### Libraries
We only need the `arduino-mqtt-sn-client` library.
Perform the following steps for the ESP8266 UDP MQTT-SN Client:
 * [Download this library](https://github.com/S3ler/arduino-mqtt-sn-client/archive/master.zip).
 * Extract the archive and rename the folder to `arduino-mqtt-sn-client` then copy it to your Arduino library directory.
 * Restart your Arduino IDE if open.
 * Select a ESP8266 board in the Arduino IDE.
 * Open the example in `examples -> arduin-mqtt-sn-client -> esp8266 -> WiFiUdpMqttSnClient`.
 * Adapt `ssid` and `password` to your WiFi network.
 * Change `gatewayIPAddress` and `localUdpPort` to your MQTT-SN gateway's IPAddress and UDP port
 * Upload and try :)
 
### ESP8266 (TCP)
For TCP we use the `TcpMqttSnClient` example with a `ESP8266` (e.g. NodeMCU v1.0).
#### Hardware
You do not need additional hardware as mentioned in [Transmission Technology to Platform Matrix](#transmission-technology-to-platform-matrix).
We tested it with the following Hardware:
 * NodeMCU v1.0
#### Libraries
We only need the `arduino-mqtt-sn-client` library.
Perform the following steps for the ESP8266 TCP MQTT-SN Client:
 * [Download this library](https://github.com/S3ler/arduino-mqtt-sn-client/archive/master.zip).
 * Extract the archive and rename the folder to `arduino-mqtt-sn-client` then copy it to your Arduino library directory.
 * Restart your Arduino IDE if open.
 * Select a ESP8266 board in the Arduino IDE.
 * Open the example in `examples -> arduin-mqtt-sn-client -> esp8266 -> TcpMqttSnClient`.
 * Adapt `ssid` and `password` to your WiFi network.
 * Change `gatewayIPAddress` and `localTcpPort` to your MQTT-SN gateway's IPAddress and TCP port
 * Upload and try :)

### Arduino Uno (LoRa)
For LoRa we use the `RF95DatagramMqttSnClient` example with a `Arduino Uno`. `LoRa` on the physical layer and the `RHDatagram` on the data link layer.
#### Hardware
As mentioned in the [Transmission Technology to Platform Matrix](#transmission-technology-to-platform-matrix) you need additional Hardware. We tested it with the following Hardware:
 * Arduino Uno + Dragino LoRa Shield
 * Arduino Uno + Dragino LoRa/GPS Shield
#### Libraries
We only need the `arduino-mqtt-sn-client` library and the [airspayce RadioHead library](https://www.airspayce.com/mikem/arduino/RadioHead/).
Perform the following steps for the Arduino LoRa MQTT-SN Client:
 * [Download this library](https://github.com/S3ler/arduino-mqtt-sn-client/archive/master.zip).
 * Extract the archive and rename the folder to `arduino-mqtt-sn-client` then copy it to your Arduino library directory.
 * [Download the airspayce RadioHead library](https://www.airspayce.com/mikem/arduino/RadioHead/) from homepage. I tested it with [version 1.87]( http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.87.zip). Copy it into your Arduino Library directory.
 * Extract the archive, then copy `RadioHead-1.87/RadioHead` into to your Arduino library directory.
 * Restart your Arduino IDE if open.
 * Select the `Arduino/Genuino Uno` board in the Arduino IDE.
 * Open the example in `examples -> arduino-mqtt-sn-client -> rf95 -> RF95DatagramMqttSnClient`.
 * Adapt `OWN_ADDRESS` to your your MQTT-SN client's `RHDatagram` address.
 * Configuration LoRa by changing `FREQUENCY`, `TX_POWER` and `MODEM_CONFIG_CHOICE`
 * Change `MqttSnGateway_Address` to your MQTT-SN gateway's `RHDatagram` address.
 * Upload and try :)
