//
// Created by ruben on 13/11/17.
//
#include <click/config.h>
#include "PacketProcessors.hh"

CLICK_DECLS

PacketType getPacketType(Packet* packet){
    if(packet->has_network_header() == false){
      return UNKOWN;
    }

    click_ip* format = (click_ip*)(packet->network_header());

    if(format->ip_p == IP_PROTO_ICMP){
      unsigned int offset = sizeof(click_ip);
      routerAdvertisementMessage* format = (routerAdvertisementMessage*)(packet->data() + offset);

      if(format->type == 9 and format->type2 == 16){
        return ADVERTISEMENT;
      }else if(format->type == 10){
        return SOLICITATION;
      }else{
        return UNKOWN;
      }
    }else if(format->ip_p == IP_PROTO_IPIP){
      return IPINIP;
    }else if(format->ip_p == IP_PROTO_UDP){
      unsigned int offset = sizeof(click_ip) + sizeof(click_udp);
      registrationReplyPacket* format = (registrationReplyPacket*)(packet->data() + offset);

      if(format->type == 1){
        return REGISTRATION;
      }else if(format->type == 3){
        return REPLY;
      }else{
        return UNKOWN;
      }
    }else{
      return UNKOWN;
    }

}

UDPHeader processUDPHeader(Packet* packet){
    UDPHeader structure;
    click_udp* format = (click_udp*)(packet->network_header() + sizeof(click_ip));

    if(packet->has_network_header() == false || packet->ip_header()->ip_p != IP_PROTO_UDP){
      return structure;
    }

    structure.sourcePort = ntohs(format->uh_sport);
    structure.destinationPort = ntohs(format->uh_dport);

    // Check checksum
    int length = ntohs(format->uh_ulen);
    if(format->uh_sum != 0) {
      unsigned csum = click_in_cksum((unsigned char *)packet->udp_header(), length);
      if (click_in_cksum_pseudohdr(csum, packet->ip_header(), length) != 0)
        throw  InvalidChecksumException("UDP Header");
    }


    return structure;
}

IPHeader processIPHeader(Packet* packet){
  IPHeader structure;

  if(packet->has_network_header() == false){
    return structure;
  }

  click_ip* format = (click_ip*)(packet->network_header());

  structure.source = IPAddress(format->ip_src);
  structure.destination = IPAddress(format->ip_dst);

  if(format->ip_sum == 0){
    throw  ZeroChecksumException("IP Header");
  }

  // Checksum
  unsigned int checksum = format->ip_sum;
  format->ip_sum = 0;
  unsigned int verifiedChecksum = click_in_cksum((unsigned char *)format, sizeof(click_ip));
  if(verifiedChecksum != checksum){
    throw InvalidChecksumException("IP Header");
  }
  format->ip_sum = checksum;

  return structure;
}

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


routerAdvertisement processRouterAdvertisementMessage(Packet* packet){
  routerAdvertisement structure;

  unsigned int offset = sizeof(click_ip);
  routerAdvertisementMessage* format = (routerAdvertisementMessage*)(packet->data() + offset);

  structure.lifetime = ntohs(format->lifetime);
  structure.sequenceNumber = ntohs(format->sequenceNumber);
  structure.registrationLifetime = ntohs(format->registrationLifetime);
  structure.homeAgent = (bool) format->H;
  structure.foreignAgent = (bool) format->H;
  structure.careOfAddress = IPAddress(format->careOfAddresses[0]);

  structure.IP = processIPHeader(packet);

  return structure;
}

routerSolicitation processRouterSolicitationMessage(Packet* packet){
  routerSolicitation structure;

  unsigned int offset = sizeof(click_ip);
  routerSolicitationMessage* format = (routerSolicitationMessage*)(packet->data() + offset);

  structure.IP = processIPHeader(packet);

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

    structure.UDP = processUDPHeader(packet);
    structure.IP = processIPHeader(packet);

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

  structure.UDP = processUDPHeader(packet);
  structure.IP = processIPHeader(packet);

  return structure;
}


CLICK_ENDDECLS
ELEMENT_PROVIDES(PacketProcessors)
