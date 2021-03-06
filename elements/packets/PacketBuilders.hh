//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_PACKET_BUILDERS_HH
#define MOBILE_IP_PACKET_BUILDERS_HH

#include "packets.hh"
#include <click/packet.hh>
#include <click/timestamp.hh>
#include "../structures/EthernetAddress.hh"
#include <sys/time.h>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include <iostream>

CLICK_DECLS

/**
 * Adds an ethernet Header to the packet
 */
WritablePacket* Etherfy(
  WritablePacket* packet,
  EthernetAddress source,
  EthernetAddress destination
);

/**
 * Adds an ip hrader to the packet
 */
WritablePacket* ICMPIPfy(
  WritablePacket* packet,
  IPAddress source,
  IPAddress destination,
  uint8_t ttl
);

/**
 * Adds an ip and udp header to the packet
 */
WritablePacket* UDPIPfy(
  WritablePacket* packet,
  IPAddress source,
  unsigned int sourcePort,
  IPAddress destination,
  unsigned int destinationPort,
  uint8_t ttl
);

/**
 * Removes the IP and UDP header
 */
WritablePacket* StripUDPIPHeader(WritablePacket* packet);

/**
 * Adds an tunnel for an given IP packet, creates a new packet
 */
WritablePacket* buildTunnelIPPacket(
    Packet* packet,
    IPAddress source,
    IPAddress destination
);

/**
 * Build an Router Advertisment Message
 */
WritablePacket* buildRouterAdvertisementMessage(
    unsigned int lifetime,
    unsigned int registrationLifetime,
    IPAddress careOfAddress,
    unsigned int sequenceNumber,
    bool homeAgent,
    bool foreignAgent
);

/**
 * Build an Router Solicitation Message
 */
WritablePacket* buildRouterSolicitationMessage();

/**
 * Builds an Mobile IP Registration request
 */
WritablePacket* buildRegistrationRequestPacket(
    unsigned int lifetime,
    IPAddress home,
    IPAddress homeAgent,
    IPAddress careOf,
    uint32_t identification = 0
);

/**
 * Builds an Mobile IP Registration Reply
 */
WritablePacket* buildRegistrationReplyPacket(
    unsigned int lifetime,
    uint8_t code,
    IPAddress home,
    IPAddress homeAgent,
    uint32_t identification
);

CLICK_ENDDECLS
#endif //MOBILE_IP_PACKET_BUILDERS_HH
