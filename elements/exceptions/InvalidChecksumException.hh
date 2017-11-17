#ifndef MOBILE_IP_EXCEPTION_INVALIDCHECKSUM_HH
#define MOBILE_IP_EXCEPTION_INVALIDCHECKSUM_HH

#include <exception>
#include <string>

class InvalidChecksumException : public std::exception{
public:
  std::string identifier;
  InvalidChecksumException(std::string identifier){
    this->identifier = identifier;
  };
  virtual const char* what(){
    std::string output = "Checksum was invalid [" + this->identifier + "]";
      return output.c_str();
  };
};

#endif
