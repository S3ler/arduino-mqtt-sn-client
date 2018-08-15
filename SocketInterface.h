
#ifndef ARDUINO_MQTTSN_CLIENT_SOCKETINTERFACE_H
#define ARDUINO_MQTTSN_CLIENT_SOCKETINTERFACE_H


#include "MqttSnMessageHandler.h"
#include "global_defines.h"

class SocketInterface
{
public:
    SocketInterface(){}
    virtual ~SocketInterface(){}

    /**
     * Set the MqttSnMessageHandler the receiveData method shall be called.
     * @tparam T type of SocketInterface implementation
     * @param mqttSnMessageHandler
     */
    template <class SocketInterfaceImplementation>
    void setMqttSnMessageHandler(MqttSnMessageHandler<SocketInterfaceImplementation> *mqttSnMessageHandler);

    /**
     * Initialize the network stack below, with what ever you have to do to establish the connection to the network.
     * @return true if the connection is successfully established, false in any other case.
     */
    virtual bool begin() =0 ;

    /**
     * Get the abstract broadcast address to send pakets to.
     * Implementation Note:
     * - If your network stack does not provide any broadcast address, map it to a address you save as not-to-send-address.
     * - If your network stack has a procedure to broadcast packets, map it to a address you save as to-broadcast-address.
     * @return the abstract broadcast address as device address from of the network stack below.
     */
    virtual device_address*  getBroadcastAddress() = 0;

    /**
     * Get your own address converted to an abstract device address.
     * This is used in the GWINFO messages, despite the mqtt-sn standard does not tell to.
     * // TODO maybe we can change this, depend on the Mqtt-Sn client implementation
     * @return the abstract own device address provided by the network stack below.
     */
    virtual device_address*  getAddress() = 0;

    /**
     * Get the maximum count of bytes the network stack you use can provide as payload in a single message.
     * The minimum value you should provide is 8 bytes and the maximum the mqtt-sn standard supports are 255 bytes.
     * If you support more then 255 bytes payload in a message return 255.
     * Implementation Note:
     * - The absolute minimum are 8 bytes, but can only publish 1 byte raw data.
     *   7 bytes are needed by the header, you have only 1 byte data left.
     *   If this makes sense is up to you.
     * - The maximum are 255 bytes. because the mqtt-sn standard message length up to 255 bytes.
     *   The Length field in each Mqtt-Sn message is only 1 byte long.
     *   Supporting longer message are out of the standard and will break the gateway implementation.
     * @return the maximum count of bytes the network stack can send
     */
    virtual uint8_t getMaximumMessageLength() = 0;

    /**
     * Abstract and simple message to send the bytes of the mqtt-sn message
     * @param destination is the abstract destination address to send the payload
     * @param bytes is the pointer to the bytes to be send
     * @param bytes_len is the length of the bytes send to, maximum is 255, minimum 8. See method: getMaximumMessageLength
     * @return true if the connection to the network still exist.
     */
    virtual bool send(device_address* destination, uint8_t* bytes, uint16_t bytes_len) = 0;

    /**
     * Abstract and simple message to send the bytes of the mqtt-sn message with a maximum signal strength parsed out of a SEARCHGW message.
     * Implementation Note:
     * - If your network stack below does not have the ability to change the signal strength or does it automatically, ignore the parameter.
     * @param destination is the abstract destination address to send the payload
     * @param bytes is the pointer to the bytes to be send
     * @param bytes_len is the length of the bytes send to, maximum is 255, minimum 8. See method: getMaximumMessageLength
     * @param signal_strength is the strength of the signal from the radius field received from the SEARCHGW message from the client with the destination_address.
     * @return true if the connection to the network still exist.
     */
    virtual bool send(device_address* destination, uint8_t* bytes, uint16_t bytes_len, uint8_t signal_strength) = 0;

    virtual bool loop() = 0;
};

#endif //ARDUINO_MQTTSN_CLIENT_SOCKETINTERFACE_H
