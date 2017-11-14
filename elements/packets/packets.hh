#ifndef CLICK_PACKETS_HH
#define CLICK_PACKETS_HH

#include <clicknet/ip.h>

struct registrationRequestPacket{
  uint8_t type; // Should be 1
  unsigned int x : 1; // should be 0
  unsigned int T : 1; // do not use : 0
  unsigned int r : 1; // should be 0
  unsigned int G : 1; // do not use : 0
  unsigned int M : 1; // do not use : 0
  unsigned int D : 1; // do not use : 0
  unsigned int B : 1; // Broadcast diagrams
  unsigned int S : 1; // do not use : 0
  uint16_t lifetime; // Number of seconds before registration is expired, 0 = request for deregistration, 0xfffff = infinity
  in_addr homeAddress; // IP Adress of mobile node
  in_addr homeAgent; // IP adress of mobile node's home agent
  in_addr careOfAddress; // Ip address of tunnel
  uint32_t identification1 = 0x0; // Ignored
  uint32_t identification2 = 0x0; // Ignored
};

struct registrationReplyPacket{
  uint8_t type; // should be 3
  uint8_t code; // result of registration request
  uint16_t lifetime; // see page 36
  in_addr homeAddress; // Ip adrress of mobile node
  in_addr homeAgent; // Ip adress of mobile node's agent
  uint32_t identification1 = 0x0; // Ignored
  uint32_t identification2 = 0x0; // Ignored
};


#endif
