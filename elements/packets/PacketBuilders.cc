//
// Created by ruben on 13/11/17.
//
#include <click/config.h>
#include "PacketBuilders.hh"

CLICK_DECLS

WritablePacket* UDPIPfy(
  WritablePacket* packet,
  IPAddress source,
  unsigned int sourcePort,
  IPAddress destination,
  unsigned int destinationPort,
  uint8_t ttl
){
  packet->push(sizeof(click_udp) + sizeof(click_ip));
  click_ip *ip = reinterpret_cast<click_ip *>(packet->data());
  click_udp *udp = reinterpret_cast<click_udp *>(ip + 1);

  // set up IP header
  ip->ip_v = 4;
  ip->ip_hl = sizeof(click_ip) >> 2;
  ip->ip_len = htons(packet->length());
  ip->ip_id = htons(0);
  ip->ip_p = IP_PROTO_UDP;
  ip->ip_src = source.in_addr();
  ip->ip_dst = destination.in_addr();
  ip->ip_tos = 0;
  ip->ip_off = 0;
  ip->ip_ttl = ttl;

  ip->ip_sum = 0;
  ip->ip_sum = click_in_cksum((unsigned char *)ip, sizeof(click_ip));

  packet->set_ip_header(ip, sizeof(click_ip));

  // set up UDP header
  uint16_t UDPLength = packet->length() - sizeof(click_ip);
  udp->uh_sport = htons(sourcePort);
  udp->uh_dport = htons(destinationPort);
  udp->uh_ulen = htons(UDPLength);
  udp->uh_sum = 0;
  unsigned csum = click_in_cksum((unsigned char *)udp, UDPLength);
  udp->uh_sum = click_in_cksum_pseudohdr(csum, ip, UDPLength);

  return packet;
}

Packet* buildTunnelIPPacket(
    Packet* originalPacket,
    IPAddress source,
    IPAddress destination
){
  int tailroom = 0;
  int packetsize = originalPacket->length() + sizeof(click_ip);
  int headroom =  sizeof(click_ether);

  WritablePacket *packet = Packet::make(headroom, 0, packetsize, tailroom);
  if (packet == 0){
      click_chatter("cannot make ip tunnel packet!");
      return NULL;
  }

  memcpy(packet->data() + sizeof(click_ip), originalPacket->data(), originalPacket->length());

  click_ip *ip = reinterpret_cast<click_ip *>(packet->data());
  const click_ip *original_ip = originalPacket->ip_header();

  // set up IP header
  ip->ip_v = 4;
  ip->ip_hl = sizeof(click_ip) >> 2;
  ip->ip_len = htons(packet->length());
  ip->ip_id = htons(0);
  ip->ip_p = IP_PROTO_IPIP;
  ip->ip_src = source.in_addr();
  ip->ip_dst = destination.in_addr();
  ip->ip_tos = original_ip->ip_tos;
  ip->ip_off = 0;
  ip->ip_ttl = original_ip->ip_ttl - 1; // Decrease TTL by one

  ip->ip_sum = 0;
  ip->ip_sum = click_in_cksum((unsigned char *)ip, sizeof(click_ip));

  packet->set_ip_header(ip, sizeof(click_ip));

  return packet;
}

// IP source -> MN home adress
// IP destination -> agent adres onbekend? -> 255.255.255.255
//                -> FA gekend d.m.v. agent advertisement -> gebruik source adres van die advertisement
//                -> HA adress gekend -> gebruik HA adress voor connectie met HA
//                -> HA adress ongekend -> sectie 3.6.1.2 , te implementeren?
// TTL -> 1 als 255.255.255.255, anders doe is zot
WritablePacket* buildRegistrationRequestPacket(
        unsigned int lifetime,
        IPAddress home,
        IPAddress homeAgent,
        IPAddress careOf
){
    int tailroom = 0;
    int packetsize = sizeof(registrationRequestPacket);
    int headroom = sizeof(click_udp) + sizeof(click_ip) + sizeof(click_ether);

    WritablePacket *packet = Packet::make(headroom, 0, packetsize, tailroom);
    if (packet == 0){
        click_chatter("cannot make registration request packet!");
        return NULL;
    }

    memset(packet->data(), 0, packet->length());
    registrationRequestPacket* format = (registrationRequestPacket*) packet->data();

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

    return packet;
}

// Codes to send
// FA
// 64 -> reason unspecified
// 69 -> requested lifetime too long
// 70 -> poorly formed request
// 71 -> poorly formed Reply
// 72 -> requested encapsulation unavailable
// HA
// 128 -> reason unspecified
// 134 -> poorly formed request
// 136 -> unkown home adress
WritablePacket* buildRegistrationReplyPacket(
        unsigned int lifetime,
        uint8_t code,
        IPAddress home,
        IPAddress homeAgent
){
  int tailroom = 0;
  int packetsize = sizeof(registrationReplyPacket);
  int headroom = sizeof(click_udp) + sizeof(click_ip) + sizeof(click_ether);

  WritablePacket *packet = Packet::make(headroom, 0, packetsize, tailroom);
  if (packet == 0){
      click_chatter("cannot make registration reply packet!");
      return NULL;
  }

  memset(packet->data(), 0, packet->length());
  registrationReplyPacket* format = (registrationReplyPacket*) packet->data();

  format->type = 3;
  format->code = code;
  format->lifetime = htons(lifetime);
  format->homeAddress = home.in_addr();
  format->homeAgent = homeAgent.in_addr();

  return packet;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(PacketBuilders)
