//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_BINDINGLIST_HH
#define MOBILE_IP_LISTS_BINDINGLIST_HH

#include <click/ipaddress.hh>

struct MobilityBindingListItem{
  IPAddress MNhome;
  IPAddress MNcareOf;
  unsigned int remainingLifetime;
};

class MobilityBindingList{
  MobilityBindingList(){};
  ~MobilityBindingList(){};

  bool remove(IPAddress homeAddress, IPAddress homeAgent);
  bool remove(MobilityBindingListItem* item);

  MobilityBindingListItem* find(IPAddress homeAddress, IPAddress homeAgent);
  MobilityBindingListItem* add(MobilityBindingListItem item);

  bool has(MobilityBindingListItem* item);
};

#endif //MOBILE_IP_LISTS_BINDINGLIST_HH
