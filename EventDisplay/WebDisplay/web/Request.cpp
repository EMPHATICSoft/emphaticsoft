//File: parseHTTP.cpp
//Brief: Parses HTTP requests into a few parts I need for a web-based event display.  Does not attempt to handle things like cookies or security contexts.
//Author: andrew@djoinc.com

#include "web/Request.h"
#include <sstream>

namespace web
{
  std::ostream& operator <<(std::ostream& os, const Request& request)
  {
    os << "Method: " << request.method << "\n"
       << "URI: " << request.uri << "\n"
       << "flags:\n";
    for(const auto& entry: request.flags)
    {
      os << "     " << entry.first << ": " << entry.second << "\n";
    }
  
    os << "body:\n" << request.body << "\n";
    return os;
  }
  
  //TODO: std::string_view would make me feel better about the memory usage of this function.
  Request parseHTTP(const std::string requestText)
  {
    Request parsed;
  
    std::istringstream requestStream(requestText);
  
    //Figure out the method and URI using the first line
    std::string line;
    std::getline(requestStream, line);
  
    std::istringstream lineStream(line);
  
    std::string token;
    std::getline(lineStream, token, ' ');
    if(!token.compare("GET"))
    {
      parsed.method = Request::Method::GET;
    }
    else if(!token.compare("POST"))
    {
      parsed.method = Request::Method::POST;
    }
    else
    {
      parsed.method = Request::Method::UNSUPPORTED;
    }
  
    std::getline(lineStream, token, ' ');
    parsed.uri = token;
  
    //Pull protocol off the first line
    std::getline(lineStream, line);
  
    while(std::getline(requestStream, line) && line != "\r")
    {
      const size_t colonPos = line.find(':');
      parsed.flags[line.substr(0, colonPos)] = line.substr(colonPos+2, std::string::npos);
    }
    
    while(std::getline(requestStream, line))
    {
      parsed.body += line + "\n";
    }
  
    return parsed;
  }
}