//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_PACKET_BUILDERS_HH
#define MOBILE_IP_PACKET_BUILDERS_HH

#include "packets.hh"
#include <click/packet.hh>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include <iostream>

CLICK_DECLS
/**
 * Adds an ip and udp header to the packet
 */
WritablePacket* IPfy(
  WritablePacket* packet,
  IPAddress source,
  unsigned int sourcePort,
  IPAddress destination,
  unsigned int destinationPort,
  unsigned int ttl
);

Packet* buildTunnelIPPacket(
    Packet* packet,
    IPAddress source,
    IPAddress destination
);

Packet* buildRegistrationRequestPacket(
    unsigned int lifetime,
    IPAddress home,
    IPAddress homeAgent,
    IPAddress careOf
);

Packet* buildRegistrationReplyPacket(
    unsigned int lifetime,
    uint8_t code,
    IPAddress home,
    IPAddress homeAgent
);

CLICK_ENDDECLS
#endif //MOBILE_IP_PACKET_BUILDERS_HH
