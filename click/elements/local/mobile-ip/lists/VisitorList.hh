//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_VISITORLIST_HH
#define MOBILE_IP_LISTS_VISITORLIST_HH

#include <click/ipaddress.hh>

struct VisitorListItem{
  IPAddress MNsource;
  IPAddress MNhome;
  IPAddress MNdestination;
  unsigned int UDPSourcePort;
  IPAddress MNhomeAgent;
  unsigned int requestedLifetime;
  unsigned int remainingLifetime;
};

class VisitorsList{
  VisitorsList(){};
  ~VisitorsList(){};

  bool remove(IPAddress homeAddress, IPAddress homeAgent);
  bool remove(VisitorListItem* item);

  VisitorListItem* find(IPAddress homeAddress, IPAddress homeAgent);
  VisitorListItem* add(VisitorListItem item);

  bool has(VisitorListItem* item);
};

#endif //MOBILE_IP_LISTS_VISITORLIST_HH
