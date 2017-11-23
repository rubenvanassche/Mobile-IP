//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_REQUESTLIST_HH
#define MOBILE_IP_LISTS_REQUESTLIST_HH

#include <click/ipaddress.hh>
#include "../packets/structures.hh"
#include <vector>
#include <iostream>

struct RequestListItem{
  IPAddress destination;
  IPAddress careOfAddress;

  // Only for FA
  registrationRequest rr;

  unsigned int requestedLifetime;
  unsigned int remainingLifetime;
  unsigned int waitingTime = 0;
};

class RequestList{
public:
  RequestList(){};
  ~RequestList(){};

  bool remove(IPAddress homeAddress, IPAddress homeAgentAddress){
    for(auto it = this->requests.begin();it != this->requests.end();){
      if(it->rr.home == homeAddress and it->rr.homeAgent == homeAgentAddress){
        it = this->requests.erase(it);
      }else{
        it++;
      }
    }
  }

  void decreaseLifetime(){
    for(auto it = this->requests.begin();it != this->requests.end();){
      // Do not remove requests with zero lifetime because they are deregisters
      if((*it).requestedLifetime != 0){
        if((*it).remainingLifetime != 0){
          (*it).remainingLifetime -= 1;

          ++it;
        }else{
            it = this->requests.erase(it);
        }
      }
    }
  }

  std::vector<RequestListItem> removeTimedOutRequests(unsigned int seconds){
    std::vector<RequestListItem> requests;

    for(auto it = this->requests.begin();it != this->requests.end();){
      it->waitingTime += 1;

      if(it->waitingTime <= seconds){
          it++;
      }else{
          requests.push_back(*it);
          it = this->requests.erase(it);
      }
    }

    return requests;
  }

  int size(){
    return this->requests.size();
  }

  void add(RequestListItem item){
    this->requests.push_back(item);
  }

private:
  std::vector<RequestListItem> requests;
};

#endif //MOBILE_IP_LISTS_REQUESTLIST_HH
