#ifndef MOBILE_IP_EXCEPTION_ZEROCHECKSUM_HH
#define MOBILE_IP_EXCEPTION_ZEROCHECKSUM_HH

#include <exception>
#include <string>

class ZeroChecksumException : public std::exception{
public:
  std::string identifier;
  ZeroChecksumException(std::string identifier){
    this->identifier = identifier;
  };
  virtual const char* what(){
    std::string output = "Checksum was zero [" + this->identifier + "]";
      return output.c_str();
  };
};

#endif
