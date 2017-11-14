//
// Created by ruben on 13/11/17.
//
#include <click/config.h>
#include "PacketProcessors.hh"

CLICK_DECLS

tunnelIP processTunnelIPPacket(Packet* packet, bool stripIPHeader){
    tunnelIP structure;
    click_ip* format = (click_ip*)(packet->data());

    structure.source = IPAddress(format->ip_src);
    structure.destination = IPAddress(format->ip_dst);
    structure.ttl = format->ip_ttl;

    if(stripIPHeader == true){
      packet->pull(sizeof(click_ip));
    }

    return structure;
}

registrationRequest processRegistrationRequestPacket(Packet* packet){
    registrationRequest structure;

    unsigned int offset = sizeof(click_ip) + sizeof(click_udp);
    registrationRequestPacket* format = (registrationRequestPacket*)(packet->data() + offset);

    structure.S  = (bool)format->S;
    structure.B  = (bool)format->B;
    structure.D  = (bool)format->D;
    structure.M  = (bool)format->M;
    structure.G  = (bool)format->G;
    structure.r  = (bool)format->r;
    structure.T  = (bool)format->T;
    structure.x  = (bool)format->x;

    structure.lifetime = ntohs(format->lifetime);
    structure.home = IPAddress(format->homeAddress);
    structure.homeAgent = IPAddress(format->homeAgent);
    structure.careOf = IPAddress(format->careOfAddress);


    return structure;
}

registrationReply processRegistrationReplyPacket(Packet* packet){
  registrationReply structure;

  unsigned int offset = sizeof(click_ip) + sizeof(click_udp);
  registrationReplyPacket* format = (registrationReplyPacket*)(packet->data() + offset);

  structure.lifetime = ntohs(format->lifetime);
  structure.code = format->code;
  structure.home = IPAddress(format->homeAddress);
  structure.homeAgent = IPAddress(format->homeAgent);

  return structure;
}


CLICK_ENDDECLS
ELEMENT_PROVIDES(PacketProcessors)
