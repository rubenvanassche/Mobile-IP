//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_BINDINGLIST_HH
#define MOBILE_IP_LISTS_BINDINGLIST_HH

#include <click/ipaddress.hh>

struct BindingListItem{
  IPAddress MNsource;
  IPAddress MNhome;
  IPAddress MNdestination;
  unsigned int UDPSourcePort;
  IPAddress MNhomeAgent;
  unsigned int requestedLifetime;
  unsigned int remainingLifetime;
};

class BindingList{
  BindingList(){};
  ~VisitBindingListorsList(){};

  bool remove(IPAddress homeAddress, IPAddress homeAgent);
  bool remove(BindingListItem* item);

  BindingListItem* find(IPAddress homeAddress, IPAddress homeAgent);
  BindingListItem* add(BindingListItem item);

  bool has(BindingListItem* item);
};

#endif //MOBILE_IP_LISTS_BINDINGLIST_HH
