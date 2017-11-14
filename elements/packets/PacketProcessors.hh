//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_PACKET_PROCESSORS_HH
#define MOBILE_IP_PACKET_PROCESSORS_HH

#include "packets.hh"
#include "structures.hh"
#include <click/packet.hh>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include <iostream>

CLICK_DECLS

/**
 * Recieves an IP in IP packet and reads information.
 * When stripIPHeader is set true, the outer header of the IP in IP packet is stripped
 */
tunnelIP processTunnelIPPacket(Packet* packet, bool stripIPHeader = false);

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