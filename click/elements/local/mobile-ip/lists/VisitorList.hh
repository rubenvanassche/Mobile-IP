//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_VISITORLIST_HH
#define MOBILE_IP_LISTS_VISITORLIST_HH

#include <click/ipaddress.hh>
#include <vector>

struct VisitorListItem{
  IPAddress MNsource;
  IPAddress MNhome;
  IPAddress MNdestination;
  unsigned int UDPSourcePort;
  IPAddress MNhomeAgent;
  unsigned int requestedLifetime;
  unsigned int remainingLifetime;
};

class VisitorList{
public:
  VisitorList(){};
  ~VisitorList(){};

  bool remove(IPAddress homeAddress, IPAddress homeAgentAddress){
    for(auto it = this->visitors.begin();it != this->visitors.end();){
      if(it->MNhome == homeAddress and it->MNhomeAgent == homeAgentAddress){
        it = this->visitors.erase(it);
      }else{
        it++;
      }
    }
  }

  VisitorListItem* find(IPAddress homeAddress, IPAddress homeAgent);
  void add(VisitorListItem item);

  bool setVisitorRemainingLifetime(IPAddress homeAddress, IPAddress homeAgentAddress, unsigned int lifetime){
    for(auto it = this->visitors.begin();it != this->visitors.end();){
      if(it->MNhome == homeAddress and it->MNhomeAgent == homeAgentAddress){
        it->remainingLifetime = lifetime;
      }else{
        it++;
      }
    }
  }

  bool has(IPAddress homeAddress, IPAddress homeAgentAddress){
    for(auto it = this->visitors.begin();it != this->visitors.end();){
      if(it->MNhome == homeAddress and it->MNhomeAgent == homeAgentAddress){
        return true;
      }else{
        it++;
      }
    }

    return false;
  }
private:
  std::vector<VisitorListItem> visitors;
};

#endif //MOBILE_IP_LISTS_VISITORLIST_HH
