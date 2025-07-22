//File parseHTTP.h
//Brief: Parses HTML requests into parts I need for a web-based event display.  Makes no attempt to handle things like cookies or security contexts.
//Author: Andrew Olivier andrew@djoinc.com

#include <iostream>
#include <map>

#pragma once

struct HTTPRequest
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

std::ostream& operator <<(std::ostream& os, const HTTPRequest& request);

HTTPRequest parseHTTP(const std::string requestText);
