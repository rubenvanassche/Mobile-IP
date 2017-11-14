#ifndef CLICK_PACKETS_HH
#define CLICK_PACKETS_HH

struct registrationRequest{
  uint8_t type; // Should be 1
  unsigned int S : 1; // do not use : 0
  unsigned int B : 1; // Broadcast diagrams
  unsigned int D : 1; // do not use : 0
  unsigned int M : 1; // do not use : 0
  unsigned int G : 1; // do not use : 0
  unsigned int r : 1; // should be 0
  unsigned int T : 1; // do not use : 0
  unsigned int x : 1; // should be 0
  uint16_t lifetime; // Number of seconds before registration is expired, 0 = request for deregistration, 0xfffff = infinity
  uint32_t homeAddress; // IP Adress of mobile node
  uint32_t homeAgent; // IP adress of mobile node's home agent
  uint32_t careOfAddress; // Ip address of tunnel
};

struct registrationReply{
  uint8_t type; // should be 3
  uint8_t code; // result of registration request
  uint16_t lifetime; // see page 36
  uint32_t homeAddress; // Ip adrress of mobile node
  uint32_t homeAgent; // Ip adress of mobile node's agent
};


#endif
