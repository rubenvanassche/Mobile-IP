//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_VISITORLIST_HH
#define MOBILE_IP_LISTS_VISITORLIST_HH

#include <click/ipaddress.hh>
#include <vector>
#include <string>
#include <sstream>

struct VisitorListItem{
  IPAddress MNhome;
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

  void add(VisitorListItem item){
    this->visitors.push_back(item);
  }

  // Sets the lifetime that is counted down by the value in the visitor list
  bool resetVisitorRemainingLifetime(IPAddress homeAddress, IPAddress homeAgentAddress){
    for(auto it = this->visitors.begin();it != this->visitors.end();it++){
      if(it->MNhome == homeAddress and it->MNhomeAgent == homeAgentAddress){
        it->remainingLifetime = it->requestedLifetime;
      }
    }
  }

  // Sets the default requested lifetime for an visitor
  bool setVisitorRequestedLifetime(IPAddress homeAddress, IPAddress homeAgentAddress, unsigned int lifetime){
    for(auto it = this->visitors.begin();it != this->visitors.end();it++){
      if(it->MNhome == homeAddress and it->MNhomeAgent == homeAgentAddress){
        it->requestedLifetime = lifetime;
      }
    }
  }

  void decreaseLifetime(){
    for(auto it = this->visitors.begin();it != this->visitors.end();){
      if(it->requestedLifetime == 65535){
        // Infinty lifetime, do not decrease
        continue;
      }

      // Do not remove requests with zero lifetime because they are deregisters
      it->remainingLifetime -= 1;

      if(it->remainingLifetime != 0){
        it++;
      }else{
        it = this->visitors.erase(it);
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

  std::string print(){
    std::stringstream out;
    out << "--------------------------------\n";
    for(auto it = this->visitors.begin();it != this->visitors.end();it++){
      out << "MN: " << it->MNhome.s().c_str();
      out <<  "   ";
      out << "HA: " << it->MNhomeAgent.s().c_str();
      out <<  "   ";
      out <<  it->remainingLifetime;
      out << "\n";
    }
    out << "--------------------------------\n";

    return out.str();
  }

  bool isVisitor(IPAddress homeAddress){
    for(auto it = this->visitors.begin();it != this->visitors.end();it++){
      if(it->MNhome == homeAddress){
        return true;
      }
    }

    return false;
  }
private:
  std::vector<VisitorListItem> visitors;
};

#endif //MOBILE_IP_LISTS_VISITORLIST_HH
