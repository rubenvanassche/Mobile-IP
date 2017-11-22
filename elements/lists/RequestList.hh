//
// Created by ruben on 13/11/17.
//

#ifndef MOBILE_IP_LISTS_REQUESTLIST_HH
#define MOBILE_IP_LISTS_REQUESTLIST_HH

#include <click/ipaddress.hh>
#include <vector>

struct RequestListItem{
  IPAddress FAAddress; // Only if applicable, needs not to be suported?
  IPAddress destination;
  IPAddress careOfAddress;
  unsigned int requestedLifetime;
  unsigned int remainingLifetime;
};

class RequestList{
public:
  RequestList(){};
  ~RequestList(){};

  bool remove(IPAddress homeAddress, IPAddress homeAgent);
  bool remove(RequestListItem* item);

  void decreaseLifetime(){
    for(auto request : this->requests){
      request.remainingLifetime -= 1;
    }
  }

  RequestListItem* find(IPAddress homeAddress, IPAddress homeAgent);
  RequestListItem* add(RequestListItem item){
    this->requests.push_back(item);
    return &(this->requests.back());
  }

  bool has(RequestListItem* item);

private:
  std::vector<RequestListItem> requests;
};

#endif //MOBILE_IP_LISTS_REQUESTLIST_HH
