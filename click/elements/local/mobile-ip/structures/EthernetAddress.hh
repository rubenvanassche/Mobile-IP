#ifndef CLICK_STRUCTURES_ETHERNET
#define CLICK_STRUCTURES_ETHERNET

#include <clicknet/ether.h>
#include <string>
#include <sstream>

class EthernetAddress{
  public:
    EthernetAddress(){
      this->eth = new uint8_t[6];
      for(int i = 0;i < 6;i++){
        this->eth[i] = 0;
      }
    };

    EthernetAddress(uint8_t* data){
      this->eth = new uint8_t[6];
      for(int i = 0;i < 6;i++){
        this->eth[i] = data[i];
      }
    };

    std::string s(){
      std::stringstream out;

      for(int i = 0;i < 6;i++){
        out << std::to_string(this->eth[i]);

        if(i != 5){
          out << " - ";
        }
      }

      return out.str();
    };

    uint8_t* data(){
      return this->eth;
    };

    bool isZero(){
      for(int i = 0;i < 6;i++){
        if(this->eth[i] != 0){
          return false;
        }
      }

      return true;
    };

    bool isBroadcast(){
      for(int i = 0;i < 6;i++){
        if(this->eth[i] != 255){
          return false;
        }
      }

      return true;
    };

    uint8_t* eth;
};

#endif
