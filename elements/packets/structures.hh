#ifndef CLICK_PACKETS_STRUCTURES_HH
#define CLICK_PACKETS_STRUCTURES_HH

#include <click/ipaddress.hh>

/*
  Structures for easy using of data in packets
 */

struct UDPHeader{
  unsigned int sourcePort;
  unsigned int destinationPort;
};

struct IPHeader{
  IPAddress source;
  IPAddress destination;
};

struct tunnelIP{
    IPAddress source;
    IPAddress destination;
    unsigned int ttl;
};

struct routerAdvertisement{
    unsigned int lifetime;
    unsigned int sequenceNumber;
    unsigned int registrationLifetime;
    bool homeAgent;
    bool foreignAgent;
    IPAddress careOfAddress;
    IPHeader IP;
};

struct routerSolicitation{
    IPHeader IP;
};

struct registrationRequest{
    bool S;
    bool B;
    bool D;
    bool M;
    bool G;
    bool r;
    bool T;
    bool x;
    unsigned int lifetime;
    IPAddress home;
    IPAddress homeAgent;
    IPAddress careOf;

    UDPHeader UDP;
    IPHeader IP;
    uint32_t identification;
};

struct registrationReply{
    unsigned int code;
    unsigned int lifetime;
    IPAddress home;
    IPAddress homeAgent;

    UDPHeader UDP;
    IPHeader IP;
    uint32_t identification;
};

#endif
