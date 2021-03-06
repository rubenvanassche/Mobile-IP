//
// Created by ruben on 13/11/17.
//
#include <click/config.h>
#include "PacketProcessors.hh"

CLICK_DECLS

PacketType getPacketType(Packet* packet){
    int etherOffset = 0; // Will be set to size of ethernet header when there is one

    click_ether* ether = (click_ether*)(packet->data());
    if(ether->ether_type == htons(ETHERTYPE_IP)){
      //std::cout << "ETH";
      etherOffset = sizeof(click_ether);
    }

    if(packet->has_network_header() == false){
      //std::cout << " - NO NETWORK HEADER" << std::endl;
      return UNKOWN;
    }

    click_ip* ipType = (click_ip*)(packet->network_header());

    if(ipType->ip_p == IP_PROTO_ICMP){
      unsigned int offset = sizeof(click_ip);
      routerAdvertisementMessage* format = (routerAdvertisementMessage*)(packet->data() + offset + etherOffset);

      if(format->type == 9 and (format->code == 16 or format->code == 0)){
        //std::cout << " - ADV" << std::endl;
        return ADVERTISEMENT;
      }else if(format->type == 10){
        //std::cout << " - SOL" << std::endl;
        return SOLICITATION;
      }else{
        //std::cout << " - ICMP" << std::endl;
        return UNKOWN;
      }
    }else if(ipType->ip_p == IP_PROTO_IPIP){
      //std::cout << " - IPINIP" << std::endl;
      return IPINIP;
    }else if(ipType->ip_p == IP_PROTO_UDP){
      unsigned int offset = sizeof(click_ip) + sizeof(click_udp);
      registrationReplyPacket* format = (registrationReplyPacket*)(packet->data() + offset + etherOffset);

      if(format->type == 1){
        //std::cout << " - REG" << std::endl;
        return REGISTRATION;
      }else if(format->type == 3){
        //std::cout << " - REPLY" << std::endl;
        return REPLY;
      }else{
        //std::cout << " - IP" << std::endl;
        return UNKOWN;
      }
    }else{
      //std::cout << " - UNKW" << std::endl;
      return UNKOWN;
    }

}

EtherHeader processEtherHeader(Packet* packet, bool stripHeader){
    EtherHeader structure;
    click_ether* format = (click_ether*)(packet->data());

    structure.source = EthernetAddress(format->ether_shost);
    structure.destination = EthernetAddress(format->ether_dhost);

    if(stripHeader == true){
      packet->pull(sizeof(click_ether));
    }

    return structure;
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
    click_ip* ipinip = (click_ip*)(packet->data());
    click_ip* ip = (click_ip*)(packet->data() + sizeof(click_ip));

    structure.source = IPAddress(ipinip->ip_src);
    structure.destination = IPAddress(ipinip->ip_dst);
    structure.ttl = ipinip->ip_ttl;

    structure.originalIP.source = IPAddress(ip->ip_src);
    structure.originalIP.destination = IPAddress(ip->ip_dst);

    if(stripIPHeader == true){
      packet->pull(sizeof(click_ip));
      ip = (click_ip*)(packet->data());

      // Set ttl
      ip->ip_ttl = structure.ttl;

      // Recalculate the Checksum
      ip->ip_sum = 0;
      ip->ip_sum = click_in_cksum((unsigned char *)ip, sizeof(click_ip));

      packet->set_ip_header(ip, sizeof(click_ip));
      packet->set_dst_ip_anno(structure.originalIP.destination.in_addr());
    }

    return structure;
}


routerAdvertisement processRouterAdvertisementMessage(Packet* packet){
  routerAdvertisement structure;

  unsigned int offset = sizeof(click_ip);
  routerAdvertisementMessage* format = (routerAdvertisementMessage*)(packet->data() + offset);

  structure.lifetime = ntohs(format->lifetime);
  structure.code = format->code;
  structure.sequenceNumber = ntohs(format->sequenceNumber);
  structure.registrationLifetime = ntohs(format->registrationLifetime);
  structure.homeAgent = (bool) format->H;
  structure.foreignAgent = (bool) format->H;
  structure.careOfAddress = IPAddress(format->careOfAddresses[0]);

  // Checksum
  unsigned int checksum = format->checksum;
  format->checksum = 0;
  unsigned int verifiedChecksum = click_in_cksum((unsigned char *)format, sizeof(routerAdvertisementMessage));
  if(verifiedChecksum != checksum){
    throw InvalidChecksumException("Advertisement");
  }
  format->checksum = checksum;

  structure.IP = processIPHeader(packet);

  return structure;
}

routerSolicitation processRouterSolicitationMessage(Packet* packet){
  routerSolicitation structure;

  unsigned int offset = sizeof(click_ip);
  routerSolicitationMessage* format = (routerSolicitationMessage*)(packet->data() + offset);

  // Checksum
  unsigned int checksum = format->checksum;
  format->checksum = 0;
  unsigned int verifiedChecksum = click_in_cksum((unsigned char *)format, sizeof(routerSolicitationMessage));
  if(verifiedChecksum != checksum){
    throw InvalidChecksumException("Advertisement");
  }
  format->checksum = checksum;

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
    structure.identification = ntohl(format->identification2);

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
  structure.identification = ntohl(format->identification2);

  structure.UDP = processUDPHeader(packet);
  structure.IP = processIPHeader(packet);

  return structure;
}


CLICK_ENDDECLS
ELEMENT_PROVIDES(PacketProcessors)
