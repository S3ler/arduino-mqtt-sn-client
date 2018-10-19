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
| Arduino ESP8266 	| &#x2705;  	| &#x274E;  	|         	| &#x2705;  	| &#x274E;\*  	| &#x274E;\*  	|       	|
| Arduino ESP32 	| &#x274E;  	| &#x274E;  	|         	| &#x2705;  	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*	|
| Arduino Mega 	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*  	|           	| &#x274E;\*  	| &#x274E;\*  	|         	|
| Arduino Uno 	| &#x274E;\*  	| &#x274E;\*  	| &#x274E;\*  	|           	| &#x274E;\*  	| &#x2705;\*  	|         	|

\* needs additional transmission hardware

##### Legend: 
* &#x2705; implemented and tested
* &#x274E; not implemented yet

## Getting Started

### ESP8266 (UDP)
For using the `WiFiUdpMqttSnClient` with a `ESP8266` and `UDP`as Transmission Protocol perform the following steps:
* [Download the arduino-mqtt-sn-client library](https://github.com/S3ler/arduino-mqtt-sn-client/archive/master.zip) and copy it into your Arduino Library directory.
* Unzip the archive and rename the folder to `arduino-mqtt-sn-client`.
* Restart your Arduino IDE if open.
* Select your ESP8266 board in the Arduino IDE.
* Open example `examples -> arduino-mqtt-sn-client -> esp8266 -> WiFiUdpMqttSnClient`.
* Compile and upload.

### Arduino Uno (LoRa)
For using the `RF95DatagramMqttSnClient` with a `Arduino Uno` and `LoRa` on the physical layer and the `RHDatagram` as data link layer.
* [Download the arduino-mqtt-sn-client library](https://github.com/S3ler/arduino-mqtt-sn-client/archive/master.zip) and copy it into your Arduino Library directory.
* [Download the airspayce RadioHead library](https://www.airspayce.com/mikem/arduino/RadioHead/) from homepage. I tested it with [version 1.87]( http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.87.zip). Copy it into your Arduino Library directory.
* Unzip the archive and rename the folter to `RadioHead`.
* Unzip the archive and rename the folder to `arduino-mqtt-sn-client`.
* Restart your Arduino IDE if open.
* Select the Arduino Uno board in the Arduino IDE.
* Open example `examples -> arduino-mqtt-sn-client -> rf95 -> RF95DatagramMqttSnClient`.
* Compile and upload.
