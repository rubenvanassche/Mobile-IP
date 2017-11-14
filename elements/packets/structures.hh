#ifndef CLICK_PACKETS_STRUCTURES_HH
#define CLICK_PACKETS_STRUCTURES_HH

#include <click/ipaddress.hh>

/*
  Structures for easy using of data in packets
 */

struct tunnelIP{
    IPAddress source;
    IPAddress destination;
    unsigned int ttl;
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
};

struct registrationReply{
    unsigned int code;
    unsigned int lifetime;
    IPAddress home;
    IPAddress homeAgent;
};

#endif
