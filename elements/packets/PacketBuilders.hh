//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_PACKET_BUILDERS_HH
#define MOBILE_IP_PACKET_BUILDERS_HH

#include "packets.hh"
#include <click/packet.hh>
#include <clicknet/udp.h>

CLICK_DECLS

/**
 * Sets an ip header to a packet
 */
WritablePacket* IPfy(

        IPAddress source,
        IPAddress destination,
        unsigned int ttl
);

WritablePacket* UDPfy(
        WritablePacket* packet,
        unsigned int sourcePort,
        unsigned int destinationPort
);

Packet* buildRegistrationRequestPacket(
        unsigned int lifetime,
        IPAddress home,
        IPAddress homeAgent,
        IPAddress careOf
);


CLICK_ENDDECLS
#endif //MOBILE_IP_PACKET_BUILDERS_HH
