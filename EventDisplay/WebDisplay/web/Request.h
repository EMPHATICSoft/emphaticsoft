//File: Request.h
//Brief: An HTTP Request that came from the front end.
//       Contains HTTP status code, message body, and headers
//       that may contain things like cookies.
//Author: Andrew Olivier aolivie4@nd.edu

#include <string>
#include <map>
#include <iostream>

namespace web
{
  struct Request
  {
    enum Method
    {
      GET,
      POST,
      UNSUPPORTED
    };
  
    Method method;
    std::string uri;
    std::map<std::string, std::string> flags;
    std::string body;
  };

  std::ostream& operator <<(std::ostream& os, const Request& request);

  Request parseHTTP(const std::string requestText);
}
