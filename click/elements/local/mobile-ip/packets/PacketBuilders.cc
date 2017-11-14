//
// Created by ruben on 13/11/17.
//
#include <click/config.h>
#include "PacketBuilders.hh"

CLICK_DECLS

WritablePacket* IPfy(
        WritablePacket* packet,
        IPAddress source,
        IPAddress destination,
        unsigned int ttl
){
    click_ip header;

    header.ip_v = 4;
    header.ip_hl = sizeof(click_ip) >> 2;
    header.ip_ttl = htons(ttl);
    header.ip_p = 17;
    header.ip_dst = destination.in_addr();
    header.ip_src = source.in_addr();
    header.ip_sum = click_in_cksum((unsigned char *) &header, sizeof(click_ip));
    header.ip_len = htons(packet->length());
    header.ip_id = htons(1);

    //packet->set_ip_header(&header, sizeof(click_ip));

    return packet;
}

WritablePacket* UDPfy(
        WritablePacket* packet,
        unsigned int sourcePort,
        unsigned int destinationPort
){
    click_udp header;

    header.uh_sport = htonl(sourcePort);
    header.uh_dport = htonl(destinationPort);
    header.uh_ulen = sizeof(click_udp) + packet->length();
    header.uh_sum = 0;
}

Packet* buildRegistrationRequestPacket(
        unsigned int lifetime,
        IPAddress home,
        IPAddress homeAgent,
        IPAddress careOf
){
    int tailroom = 0;
    int packetsize = sizeof(registrationRequest);
    int headroom = sizeof(click_ip);

    WritablePacket *packet = Packet::make(headroom, 0, packetsize, tailroom);
    if (packet == 0){
        click_chatter("cannot make packet!");
        return NULL;
    }

    memset(packet->data(), 0, packet->length());
    registrationRequest* format = (registrationRequest*) packet->data();

    format->type = 1;
    format->S = 0;
    format->B = 0;
    format->D = 0;
    format->M = 0;
    format->G = 0;
    format->r = 0;
    format->T = 0;
    format->x = 0;
    format->lifetime = htons(lifetime);
    format->homeAddress = home.in_addr();
    format->homeAgent = homeAgent.in_addr();
    format->careOfAddress = careOf.in_addr();

    IPfy(packet, home, homeAgent, 1);

    return packet;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(PacketBuilders)
