#ifndef MOBILE_IP_EXCEPTION_REQUESTNOTFOUND_HH
#define MOBILE_IP_EXCEPTION_REQUESTNOTFOUND_HH

#include <exception>
#include <string>

class RequestNotFoundException : public std::exception{
public:
  std::string identifier;
  RequestNotFoundException(std::string identifier){
    this->identifier = identifier;
  };
  virtual const char* what(){
    std::string output = "Request [" + this->identifier + "] was not found";
      return output.c_str();
  };
};

#endif
