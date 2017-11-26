//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_BINDINGLIST_HH
#define MOBILE_IP_LISTS_BINDINGLIST_HH

#include <click/ipaddress.hh>
#include <vector>

struct MobilityBindingListItem{
  IPAddress homeAddress;
  IPAddress careOfAddress;
  unsigned int remainingLifetime;
};

class MobilityBindingList{
public:
  MobilityBindingList(){};
  ~MobilityBindingList(){};

  // Remove one binding
  bool remove(IPAddress homeAddress, IPAddress careOfAddress){
    for(auto it = this->mobilityBindings.begin();it != this->mobilityBindings.end();){
      if(it->homeAddress == homeAddress and it->careOfAddress == careOfAddress){
        it = this->mobilityBindings.erase(it);
      }else{
        it++;
      }
    }
  }

  // Remove all bindings
  bool remove(IPAddress homeAddress){
    for(auto it = this->mobilityBindings.begin();it != this->mobilityBindings.end();){
      if(it->homeAddress == homeAddress){
        it = this->mobilityBindings.erase(it);
      }else{
        it++;
      }
    }
  }

  void add(MobilityBindingListItem item){
    this->mobilityBindings.push_back(item);
  }

  bool has(MobilityBindingListItem* item);
private:
  std::vector<MobilityBindingListItem> mobilityBindings;
};

#endif //MOBILE_IP_LISTS_BINDINGLIST_HH
