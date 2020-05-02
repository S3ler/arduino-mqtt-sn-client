/*
The MIT License (MIT)

Copyright (C) 2014 John Donovan heavily edited by Gabriel Nikol

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef ARDUINO_MQTTSN_CLIENT_MQTTSN_MESSAGES_H
#define ARDUINO_MQTTSN_CLIENT_MQTTSN_MESSAGES_H

#include "global_defines.h"
#include <stdint.h>

#define PROTOCOL_ID 0x01

#define FLAG_NO_FLAGS 0x00
#define FLAG_DUP 0x80
#define FLAG_QOS_0 0x00
#define FLAG_QOS_1 0x20
#define FLAG_QOS_2 0x40
#define FLAG_QOS_M1 0x60
#define FLAG_RETAIN 0x10
#define FLAG_WILL 0x08
#define FLAG_CLEAN 0x04
#define FLAG_TOPIC_NAME 0x00
#define FLAG_TOPIC_PREDEFINED_ID 0x01
#define FLAG_TOPIC_SHORT_NAME 0x02

#define QOS_MASK (FLAG_QOS_0 | FLAG_QOS_1 | FLAG_QOS_2 | FLAG_QOS_M1)
#define TOPIC_MASK (FLAG_TOPIC_NAME | FLAG_TOPIC_PREDEFINED_ID | FLAG_TOPIC_SHORT_NAME)

// Recommended values for timers and counters. All timers are in seconds.
#define T_ADV 960
#define N_ADV 3
#define T_SEARCH_GW 5
#define T_GW_INFO 5
#define T_WAIT 360
#define T_RETRY 15
#define N_RETRY 5

enum return_code_t : uint8_t {
    ACCEPTED = 0x00,
    REJECTED_CONGESTION = 0x01,
    REJECTED_INVALID_TOPIC_ID = 0x02,
    REJECTED_NOT_SUPPORTED = 0x03
};

enum message_type : uint8_t {
    MQTTSN_ADVERTISE,
    MQTTSN_SEARCHGW,
    MQTTSN_GWINFO,
    MQTTSN_CONNECT = 0x04,
    MQTTSN_CONNACK,
    MQTTSN_WILLTOPICREQ,
    MQTTSN_WILLTOPIC,
    MQTTSN_WILLMSGREQ,
    MQTTSN_WILLMSG,
    MQTTSN_REGISTER,
    MQTTSN_REGACK,
    MQTTSN_PUBLISH,
    MQTTSN_PUBACK,
    MQTTSN_PUBCOMP,
    MQTTSN_PUBREC,
    MQTTSN_PUBREL,
    MQTTSN_SUBSCRIBE = 0x12,
    MQTTSN_SUBACK,
    MQTTSN_UNSUBSCRIBE,
    MQTTSN_UNSUBACK,
    MQTTSN_PINGREQ = 0x16,
    MQTTSN_PINGRESP,
    MQTTSN_DISCONNECT,
    MQTTSN_WILLTOPICUPD = 0x1a,
    MQTTSN_WILLTOPICRESP,
    MQTTSN_WILLMSGUPD,
    MQTTSN_WILLMSGRESP
};

#ifndef MQTT_SNRF24L01P_NRF24MESSAGES_H
#pragma pack(push, 1)

struct message_header {
    uint8_t length;
    uint8_t type;

    void to_pingreq() {
        length = 2;
        type = MQTTSN_PINGREQ;
    }

    void to_disconnect() {
        length = 2;
        type = MQTTSN_DISCONNECT;
    }
};

#pragma pack(pop)
#endif

#pragma pack(push, 1)

struct msg_advertise : public message_header {
    uint8_t gw_id;
    uint16_t duration;

    msg_advertise(uint8_t gw_id, uint16_t duration) : gw_id(gw_id), duration(duration) {
        length = 5;
        type = MQTTSN_ADVERTISE;
    }
};

#pragma pack(pop)

struct msg_searchgw : public message_header {
    uint8_t radius;

    msg_searchgw(uint8_t radius) : radius(radius) {
        length = 3;
        type = MQTTSN_SEARCHGW;
    }
};

struct msg_gwinfo : public message_header {
    uint8_t gw_id;
    uint8_t gw_address[sizeof(device_address)];

    msg_gwinfo(uint8_t gw_id, uint8_t *gw_add) : gw_id(gw_id) {
        length = sizeof(msg_gwinfo);
        type = MQTTSN_GWINFO;
        memcpy(gw_address, gw_add, sizeof(device_address));
    }
};

struct msg_connect : public message_header {
    uint8_t flags;
    uint8_t protocol_id;
    uint16_t duration;
    char client_id[24];

    msg_connect(bool will, bool clean_session, uint8_t protocol_id, uint16_t duration, const char *client_id) {

        uint8_t client_id_length = (uint8_t) strlen(client_id);
        if (client_id_length > 22) {
            client_id_length = 23;
        }
        length = ((uint8_t) (6 + 1)) + client_id_length;
        type = MQTTSN_CONNECT;
        flags = 0x0;
        if (will) {
            flags |= FLAG_WILL;
        }
        if (clean_session) {
            flags |= FLAG_CLEAN;
        }
        this->protocol_id = protocol_id;
        this->duration = duration;
        memset(this->client_id, 0x0, 24);
        memcpy(this->client_id, client_id, client_id_length);
    }

};
#pragma pack(push, 1)

struct msg_connack : public message_header {
    return_code_t return_code;

    msg_connack(return_code_t return_code) : return_code(return_code) {
        length = sizeof(msg_connack);
        type = MQTTSN_CONNACK;
    }
};

#pragma pack(pop)



struct msg_willtopic : public message_header {
    uint8_t flags;
    char will_topic[252];

    msg_willtopic(const char *willtopic, int8_t qos, bool retain) {
        memset(this, 0, sizeof(*this));
        length = 3 + strlen(willtopic) + 1;
        type = MQTTSN_WILLTOPIC;
        if (retain) {
            this->flags |= FLAG_RETAIN;
        }
        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        }
    }
};

struct msg_willmsg : public message_header {
    uint8_t willmsg[253];

    msg_willmsg(const uint8_t *s_data, uint8_t s_data_len) {
        memset(this, 0, sizeof(*this));
        this->length = ((uint8_t) 2) + s_data_len;
        memcpy(&willmsg, s_data, s_data_len);
    }
};

/*
struct msg_willtopicreq : public message_header {
    msg_willtopicreq(){
        length = 2;
        type = MQTTSN_WILLTOPICREQ;
    }
};

struct msg_willmsgreq : public message_header {
    msg_willmsgreq(){
        length = 2;
        type = MQTTSN_WILLMSGREQ;
    }
};
*/
#pragma pack(push, 1)

struct msg_register : public message_header {
    uint16_t topic_id;
    uint16_t message_id;
    char topic_name[UINT8_MAX - 6];

    msg_register(uint16_t topic_id, uint16_t message_id, const char *topic_name) :
            topic_id(topic_id), message_id(message_id) {
        length = (uint8_t) (6 + strlen(topic_name) + 1);
        type = MQTTSN_REGISTER;
        strcpy(this->topic_name, topic_name);
        //memcpy(this->topic_name, topic_name, strlen(topic_name) + 1);
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct msg_regack : public message_header {
    uint16_t topic_id;
    uint16_t message_id;
    return_code_t return_code;

    msg_regack(uint16_t topic_id, uint16_t message_id, return_code_t return_code) :
            topic_id(topic_id), message_id(message_id), return_code(return_code) {
        length = 7;
        type = MQTTSN_REGACK;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)

struct msg_publish : public message_header {
    uint8_t flags;
    uint16_t topic_id;
    uint16_t message_id;
    uint8_t data[UINT8_MAX - 7];

    msg_publish(bool dup, int8_t qos, bool retain, bool short_topic, uint16_t topic_id, uint16_t msg_id,
                const uint8_t *s_data, uint8_t s_data_len) : topic_id(topic_id), message_id(msg_id) {
        memset(this, 0, sizeof(*this));
        this->length = ((uint8_t) 7) + s_data_len;
        this->type = MQTTSN_PUBLISH;
        this->flags = 0x0;
        if (dup) {
            this->flags |= FLAG_DUP;
        }
        if (retain) {
            this->flags |= FLAG_RETAIN;
        }
        if (short_topic) {
            this->flags |= FLAG_TOPIC_SHORT_NAME;
        } else {
            this->flags |= FLAG_TOPIC_PREDEFINED_ID;
        }
        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        } else if (qos == -1) {
            this->flags |= FLAG_QOS_M1;
        }
        this->topic_id = topic_id;
        this->message_id = msg_id;
        memcpy(this->data, s_data, s_data_len);
    }
};

#pragma pack(pop)

struct msg_publish_send : public message_header {
    uint8_t flags;
    uint8_t topic_id_msb;
    uint8_t topic_id_lsb;
    uint8_t message_id_msb;
    uint8_t message_id_lsb;
    uint8_t data[0];
};


struct msg_puback : public message_header {
    uint16_t topic_id;
    uint16_t message_id;
    return_code_t return_code;


    msg_puback(uint16_t topic_id, uint16_t msg_id, return_code_t return_code) :
            topic_id(topic_id), message_id(msg_id), return_code(return_code) {
        length = 7;
        type = MQTTSN_PUBACK;
    }
};


struct msg_pubqos2 : public message_header {
    uint16_t message_id;
};

struct msg_subscribe : public message_header {
    uint8_t flags;
};


struct msg_subscribe_shorttopic : public msg_subscribe {
    uint16_t message_id;
    uint16_t topic_id;

    msg_subscribe_shorttopic(bool short_topic, uint16_t topic_id, uint16_t msg_id, uint8_t qos, bool dup) {
        memset(this, 0, sizeof(*this));

        this->length = 7;
        this->type = MQTTSN_SUBSCRIBE;

        if (dup) {
            this->flags |= FLAG_DUP;
        }
        if (short_topic) {
            this->flags |= FLAG_TOPIC_SHORT_NAME;
        } else {
            this->flags |= FLAG_TOPIC_PREDEFINED_ID;
        }

        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        }
        this->message_id = msg_id;
        this->topic_id = topic_id;
    }
};

#pragma pack(push, 1) // exact fit - no padding

struct msg_subscribe_topicname : public msg_subscribe {
    uint16_t message_id;
    char topic_name[250];

    msg_subscribe_topicname(const char *topic_name, uint16_t msg_id, uint8_t qos, bool dup) {
        memset(this, 0, sizeof(*this));
        this->length = (uint8_t) (5 + strlen(topic_name) + 1);
        this->type = MQTTSN_SUBSCRIBE;
        if (dup) {
            this->flags |= FLAG_DUP;
        }
        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        }
        this->message_id = msg_id;
        strcpy(this->topic_name, topic_name);
    }
};

#pragma pack(pop) //back to whatever the previous packing mode was

#pragma pack(push, 1)

struct msg_suback : public message_header {
    uint8_t flags;
    uint16_t topic_id;
    uint16_t message_id;
    return_code_t return_code;

    msg_suback(uint8_t qos, uint16_t topic_id, uint16_t msg_id, return_code_t return_code) {
        memset(this, 0, sizeof(msg_suback));
        length = 8;
        type = MQTTSN_SUBACK;
        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        }
        this->topic_id = topic_id;
        this->message_id = msg_id;
        this->return_code = return_code;
    }
};
#pragma pack(pop) //back to whatever the previous packing mode was

struct msg_unsubscribe : public message_header {
    uint8_t flags;
    uint16_t message_id;
    union {
        char topic_name[0];
        uint16_t topic_id;
    };
};

struct msg_unsubscribe_send : public message_header {
    uint8_t flags;
    uint8_t message_id_msb;
    uint8_t message_id_lsb;
    uint8_t topic_id_msb;
    uint8_t topic_id_lsb;
};


struct msg_unsuback : public message_header {
    uint16_t message_id;

    msg_unsuback(uint16_t msg_id) : message_id(msg_id) {
        length = 4;
        type = MQTTSN_UNSUBACK;
    }
};

struct msg_pingreq : public message_header {
    char client_id[0];

    void init_msg_pingreq(msg_pingreq *pingreq, const char *client_id) {
        uint8_t client_id_length = (uint8_t) strlen(client_id);
        if (client_id_length > 22) {
            client_id_length = 23;
        }
        pingreq->length = ((uint8_t) (2 + 1)) + client_id_length;
        pingreq->type = MQTTSN_PINGREQ;
        memset(pingreq->client_id, 0x0, 24);
        memcpy(pingreq->client_id, client_id, client_id_length);
    };
};

struct msg_disconnect : public message_header {
    uint16_t duration;
};

struct msg_willtopicresp : public message_header {
    uint8_t return_code;
};

struct msg_willmsgresp : public message_header {
    uint8_t return_code;
};

struct msg_pubrec : public message_header {
    uint16_t message_id;

    msg_pubrec(uint16_t msg_id) : message_id(msg_id) {
        length = 4;
        type = MQTTSN_PUBREC;
    }
};

struct msg_pubrel : public message_header {
    uint16_t message_id;

    msg_pubrel(uint16_t msg_id) : message_id(msg_id) {
        length = 4;
        type = MQTTSN_PUBREL;
    }
};

struct msg_pubcomp : public message_header {
    uint16_t message_id;

    msg_pubcomp(uint16_t msg_id) : message_id(msg_id) {
        length = 4;
        type = MQTTSN_PUBCOMP;
    }
};


#endif //ARDUINO_MQTTSN_CLIENT_MQTTSN_MESSAGES_H
