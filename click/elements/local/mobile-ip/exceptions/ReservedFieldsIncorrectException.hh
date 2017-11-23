#ifndef MOBILE_IP_EXCEPTION_RESERVEDFIELDS_HH
#define MOBILE_IP_EXCEPTION_RESERVEDFIELDS_HH

#include <exception>
#include <string>

class ReservedFieldsIncorrectException : public std::exception{
public:
  std::string identifier;
  ReservedFieldsIncorrectException(std::string identifier){
    this->identifier = identifier;
  };
  virtual const char* what(){
    std::string output = "Reserved fields were not zero [" + this->identifier + "]";
      return output.c_str();
  };
};

#endif
