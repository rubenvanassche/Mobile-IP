//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_PACKET_PROCESSORS_HH
#define MOBILE_IP_PACKET_PROCESSORS_HH

#include "packets.hh"
#include "structures.hh"
#include "../exceptions/ZeroChecksumException.hh"
#include "../exceptions/InvalidChecksumException.hh"
#include <click/packet.hh>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include <iostream>

CLICK_DECLS

enum PacketType {IPINIP, ADVERTISEMENT, SOLICITATION, REGISTRATION, REPLY, UNKOWN};

/**
 * Gets the type of packet that's being processed
 */
PacketType getPacketType(Packet* packet);

/**
 * Get information from the Ethernet Header
 */
EtherHeader processEtherHeader(Packet* packet);

/**
 * Get information from the UDP Header
 */
UDPHeader processUDPHeader(Packet* packet);

/**
 * Get information from the IP Header
 */
IPHeader processIPHeader(Packet* packet);

/**
 * Recieves an IP in IP packet and reads information.
 * When stripIPHeader is set true, the outer header of the IP in IP packet is stripped
 */
tunnelIP processTunnelIPPacket(Packet* packet, bool stripIPHeader = false);

/**
 * Recieves an router advertisment message and extracts the information
 */
routerAdvertisement processRouterAdvertisementMessage(Packet* packet);

/**
 * Recieves an router advertisment message and extracts the information
 */
routerSolicitation processRouterSolicitationMessage(Packet* packet);


/**
 * Recieves an Registration Request package and extracts the information
 */
registrationRequest processRegistrationRequestPacket(Packet* packet);

/**
 * Recieves an Registration Reply package and extracts the information
 */
registrationReply processRegistrationReplyPacket(Packet* packet);

CLICK_ENDDECLS
#endif //MOBILE_IP_PACKET_PROCESSORS_HH
