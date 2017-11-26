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

  // Adds or updates an entry
  void add(IPAddress homeAddress, IPAddress careOfAddress, unsigned int lifetime){
    for(auto it = this->mobilityBindings.begin();it != this->mobilityBindings.end();){
      if(it->homeAddress == homeAddress and it->careOfAddress == careOfAddress){
        it->remainingLifetime = lifetime;
        return;
      }else{
        it++;
      }
    }

    MobilityBindingListItem m;
    m.homeAddress = homeAddress;
    m.careOfAddress = careOfAddress;
    m.remainingLifetime = lifetime;

    this->mobilityBindings.push_back(m);
  }

  void decreaseLifetime(){
    for(auto it = this->mobilityBindings.begin();it != this->mobilityBindings.end();){
      // Do not remove requests with zero lifetime because they are deregisters
      it->remainingLifetime -= 1;

      if(it->remainingLifetime != 0){
        it++;
      }else{
        it = this->mobilityBindings.erase(it);
      }
    }
  }

  bool has(MobilityBindingListItem* item);
private:
  std::vector<MobilityBindingListItem> mobilityBindings;
};

#endif //MOBILE_IP_LISTS_BINDINGLIST_HH
