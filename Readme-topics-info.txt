Topics and Topic IDs - Excerpts from the MQTT-SN standard.  
--------------------

TopicIdType:indicates whether the field TopicId or TopicName included in this
message contains a:

 + a normal topic id (set to “0b00”),
 + pre-defined topic id (set to “0b01”),
 + short topic name (set to “0b10”).

The value “0b11” is reserved.

Refer to sections 3 and 6.7 for the definition of the various types of topic ids.

“Pre-defined” topic ids and “short” topic names are introduced, for which no
registration is required.  Pre-defined topic ids are also a two-byte long
replacement of the topic name, their mapping to the topic names is however
known in advance by both the client’s application and the
gateway/server. Therefore both sides can start using pre-defined topic ids;
there is no need for a registration as in the case of “normal” topic ids
mentioned above.

Short topic names are topic names that have a fixed length of two octets.
They are short enough for being carried together with the data within PUBLISH
messages. As for pre-defined topic ids, there is also no need for a
registration for short topic names

6.7 Pre-defined topic ids and short topic names. As described in Section 6.5,
a topic id is a two-byte long replacement of the string-based topic name.  A
client needs to use the REGISTER procedure to inform the gateway about the
topic name it wants to employ and gets from the gateway the corresponding
topic id.  It then will use this topic id in the PUBLISH messages it sends to
the gateway.  In the opposite direction, the PUBLISH messages also contain a
2-byte topic id (instead of the string-based topic name). The client is
informed about the relation between topic id and topic name by means of
either a former SUBSCRIBE procedure or a REGISTER procedure started by the
gateway

A “pre-defined” topic id is a topic id whose mapping to a topic name is known
in advance by both the client’s application and the gateway. This is
indicated in the Flags field of the message. When using pre-defined topic ids
both sides can start immediately with the sending of PUBLISH messages; there
is no need for the REGISTER procedure as in the case of ”normal” topic
ids. When receiving a PUBLISH message with a pre-defined topic id of which
the mapping to a topic name is unknown, the receiver should return a PUBACK
with the ReturnCode = “Rejection: invalid topic Id”. Note that this error
situation cannot be resolved by means of re-registering as in the case of
normal topic id. A client is still required to subscribe to a pre-defined
topic id, if it wants to receive PUBLISH messages relating to that topic id.
To avoid confusion between a pre-defined topic id and a two-byte short topic
name, the SUBSCRIBE message contains a flag indicating whether it is
subscribing for a short topic name or a pre-defined topic id. A “short” topic
name is a topic name that has a fixed length of two octets. It could be
carried together with the data within a PUBLISH message, thus no REGISTER
procedure is needed for a short topic name. Otherwise, all rules that apply
to normal topic names also apply to short topic names. Note however that it
does not make sense to do wildcarding in subscriptions to short topic names,
because it is not possible to define a meaningful name hierarchy with only
two characters.

