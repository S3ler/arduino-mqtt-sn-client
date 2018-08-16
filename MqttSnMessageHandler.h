
#ifndef ARDUINO_MQTTSN_CLIENT_MQTTSNMESSAGEHANDLER_H
#define ARDUINO_MQTTSN_CLIENT_MQTTSNMESSAGEHANDLER_H

#include "MqttSnClient.h"
#include "global_defines.h"
#include "mqttsn_messages.h"
#include <cstdint>

template<class MqttSnClient_SocketInterface>
class MqttSnClient;

template<class MqttSnMessageHandler_SocketInterface>
class MqttSnMessageHandler {
private:
    MqttSnMessageHandler_SocketInterface &socketInterface;
    MqttSnClient<MqttSnMessageHandler_SocketInterface> &mqttSnClient;
public:
    MqttSnMessageHandler(MqttSnMessageHandler_SocketInterface &socketInterface,
                         MqttSnClient<MqttSnMessageHandler_SocketInterface> &mqttSnClient)
            : socketInterface(socketInterface), mqttSnClient(mqttSnClient) {}

    bool begin() {
        return socketInterface.begin();
    }


    void receiveData(device_address *address, uint8_t *bytes) {

        message_header *header = (message_header *) bytes;
        if (header->length < 2) {
            return;
        }
        if (header->type == MQTTSN_ADVERTISE) {
            //TODO
            return;
        }
        if (!mqttSnClient.is_mqttsn_connected()) {
            if (header->type == MQTTSN_CONNACK) {
                parse_connack(address, bytes);
            }
            return;
        }
        if (!mqttSnClient.is_gateway_address(address)) {
            return;
        }
        switch (header->type) {
            case MQTTSN_PINGREQ:
                parse_pingreq(address, bytes);
                break;
            case MQTTSN_PINGRESP:
                parse_pingresp(address, bytes);
                break;
            case MQTTSN_CONNACK:
                parse_connack(address, bytes);
                break;
            case MQTTSN_SUBACK:
                parse_suback(address, bytes);
                break;
            case MQTTSN_REGACK:
                parse_regack(address, bytes);
            case MQTTSN_PUBLISH:
                Serial.println("MQTTSN_PUBLISH");
                parse_publish(address, bytes);
                break;
            default:
                break;
        }
    }

    void parse_pingreq(device_address *address, uint8_t *bytes) {
        msg_pingreq *msg = (msg_pingreq *) bytes;
        if (bytes[0] == 2 && bytes[1] == MQTTSN_PINGREQ) {
            handle_pingreq(address);
        }
    }

    void handle_pingreq(device_address *address) {
        if (mqttSnClient.is_gateway_address(address)) {
            if (mqttSnClient.get_await_message() == MQTTSN_PINGREQ) {
                send_pingresp(address);
            }
            // TODO disconnect
        }
    }

    void send_pingresp(device_address *address) {
        message_header to_send;
        to_send.length = 2;
        to_send.type = MQTTSN_PINGRESP;
        if (!socketInterface.send(address, (uint8_t *) &to_send, sizeof(message_header))) {
            mqttSnClient.notify_socket_disconnected();
        }
    }

    void send_connect(device_address *address, const char *client_id, uint16_t duration) {
        msg_connect to_send(false, true, PROTOCOL_ID, duration, client_id);
        if (!socketInterface.send(address, (uint8_t *) &to_send, to_send.length)) {
            mqttSnClient.notify_socket_disconnected();
        }
    }

    void parse_connack(device_address *pAddress, uint8_t *bytes) {
        msg_connack *msg = (msg_connack *) bytes;
        // TODO check values
        if (bytes[0] == 3 && bytes[1] == MQTTSN_CONNACK) {
            if (msg->type == mqttSnClient.get_await_message()) {
                if (msg->return_code == ACCEPTED) {
                    mqttSnClient.set_await_message(MQTTSN_PINGREQ);
                    mqttSnClient.set_mqttsn_connected();
                    return;
                } else if (msg->return_code == REJECTED_CONGESTION) {
                    return;
                }
            }
            send_disconnect(pAddress);
            mqttSnClient.set_await_message(MQTTSN_DISCONNECT);
        }
    }

    void send_disconnect(device_address *address) {
        message_header to_send;
        to_send.to_disconnect();
        if (!socketInterface.send(address, (uint8_t *) &to_send, sizeof(message_header))) {
            mqttSnClient.notify_socket_disconnected();
        }
    }

    void notify_socket_connected() {
        mqttSnClient.notify_socket_connected();
    }

    void notify_socket_disconnected() {
        mqttSnClient.notify_socket_disconnected();
    }

    void send_pingreq(device_address *address) {
        message_header to_send;
        to_send.to_pingreq();
        if (!socketInterface.send(address, (uint8_t *) &to_send, (uint16_t) to_send.length)) {
            mqttSnClient.notify_socket_disconnected();
        }
    }

    void parse_pingresp(device_address *pAddress, uint8_t *bytes) {
        msg_pingreq *msg = (msg_pingreq *) bytes;
        if (bytes[0] == 2 && bytes[1] == MQTTSN_PINGRESP) {
            if (mqttSnClient.get_await_message() == MQTTSN_PINGRESP) {
                mqttSnClient.notify_pingresponse_arrived();
                mqttSnClient.set_await_message(MQTTSN_PINGREQ);
            }
            //TODO disconnect
        }
    }

    void send_subscribe(device_address *address, const char *topic_name, uint8_t qos) {
        msg_subscribe_topicname to_send(topic_name, mqttSnClient.increment_and_get_msg_id_counter(), qos, false);
        if (!socketInterface.send(address, (uint8_t *) &to_send, (uint16_t) to_send.length)) {
            mqttSnClient.notify_socket_disconnected();
        }

    }

    void parse_suback(device_address *pAddress, uint8_t *bytes) {
        msg_suback *msg = (msg_suback *) bytes;
        if (msg->length == 8 && msg->type == MQTTSN_SUBACK && msg->message_id == mqttSnClient.get_await_message_id()) {
            if (msg->return_code == ACCEPTED) {
                int8_t granted_qos = 0;
                if ((msg->flags & FLAG_QOS_M1) == FLAG_QOS_M1) {
                    granted_qos = -1;
                } else if ((msg->flags & FLAG_QOS_2) == FLAG_QOS_2) {
                    granted_qos = 2;
                } else if ((msg->flags & FLAG_QOS_1) == FLAG_QOS_1) {
                    granted_qos = 1;
                } else {
                    granted_qos = 0;
                }
                if (granted_qos == -1) {
                    //TODO disconnect
                }
                if (mqttSnClient.await_topic_id) {
                    mqttSnClient.set_await_topic_id(msg->topic_id);
                }
                mqttSnClient.set_granted_qos(granted_qos);
                mqttSnClient.set_await_message(MQTTSN_PINGREQ);
                return;
            } else if (msg->return_code == REJECTED_CONGESTION) {
                // TODO try again later
            } else if (msg->return_code == REJECTED_INVALID_TOPIC_ID) {
                // TODO register topic name again!
            }
            /*else if (msg->return_code == REJECTED_NOT_SUPPORTED) {
                // disconnect
            }*/
        }
        //TODO disconnect
    }

    void parse_regack(device_address *pAddress, uint8_t *bytes) {
        msg_regack* msg = (msg_regack *) bytes;
        if(bytes[0] == 7 & bytes[1] == MQTTSN_REGACK){
            mqttSnClient.handle_regack(msg->topic_id, msg->message_id, msg->return_code);
        }
    }

    void parse_publish(device_address *address, uint8_t *bytes) {
        msg_publish *msg = (msg_publish *) bytes;
        if (bytes[0] > 7 && bytes[1] == MQTTSN_PUBLISH) { // 7 bytes header + at least 1 byte data
            bool dup = (msg->flags & FLAG_DUP) != 0;

            int8_t qos = 0;
            if ((msg->flags & FLAG_QOS_M1) == FLAG_QOS_M1) {
                qos = -1;
            } else if ((msg->flags & FLAG_QOS_2) == FLAG_QOS_2) {
                qos = 2;
            } else if ((msg->flags & FLAG_QOS_1) == FLAG_QOS_1) {
                qos = 1;
            } else {
                qos = 0;
            }

            bool retain = (msg->flags & FLAG_RETAIN) != 0;
            bool short_topic = (msg->flags & FLAG_TOPIC_SHORT_NAME) != 0;
            uint16_t data_len = bytes[0] - (uint8_t) 7;
            if (((qos == 0) || (qos == -1)) && msg->message_id != 0x0000) {
                // this can be too strict
                // we can also ignore the message_id for Qos 0 and -1
                return;
            }

            //if (!short_topic && !(msg->flags & FLAG_TOPIC_PREDEFINED_ID != 0)) { // TODO what does this so?! WTF?!
            //    mqttSnClient.handle_publish(address, msg->data, data_len, msg->message_id, msg->topic_id, short_topic, retain, qos, dup);
            //}
            //msg id
            // short topic
            mqttSnClient.handle_publish(address, msg->data, data_len, msg->topic_id, retain, qos);
            return;
        }

        // TODO disconnect
    }


    void send_publish(device_address *address, uint8_t *data, uint8_t data_len, uint16_t msg_id,
                      uint16_t topic_id, bool short_topic, bool retain, uint8_t qos, bool dup) {
        msg_publish to_send(dup, qos, retain, short_topic, topic_id, msg_id, data, data_len);
        if (!socketInterface.send(address, (uint8_t *) &to_send, (uint16_t) to_send.length)) {
            mqttSnClient.notify_socket_disconnected();
        }
    }

    void send_register(device_address *address, uint16_t msg_id, const char *topic_name) {
        msg_register to_send(0x0, msg_id, topic_name);
        if (!socketInterface.send(address, (uint8_t *) &to_send, (uint16_t) to_send.length)) {
            mqttSnClient.notify_socket_disconnected();
        }
    }

};


#endif //ARDUINO_MQTTSN_CLIENT_MQTTSNMESSAGEHANDLER_H
