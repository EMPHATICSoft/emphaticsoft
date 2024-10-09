//File: web/Connection.h
//Brief: A web::Connection is how a web::App communicates with a Javascript
//       program from c++.  web::Connection formats messages for HTTP,
//       persists communication with a web browser across web::Apps, and
//       manages ephemeral ports that transfer individual resources within
//       a web page.
//Author: Andrew Olivier aolivie4@nd.edu

#ifndef WEB_CONNECTION_H
#define WEB_CONNECTION_H

#include <string>
#include <vector>

namespace web
{
  class Connection
  {
    public:
      Connection(const char* port);
      virtual ~Connection();

      //TODO: Add optional arguments for more header details
      int sendFile(const std::string& fileName, const int messageSocket) const;
      int sendString(const std::string& content, const int messageSocket) const;

    //protected:
      //Linux file descriptors for TCP sockets
      int fListenSocket;
      std::vector<int> fMessageSockets;
      int fNextEventSocket;
  };
}

#endif //WEB_CONNECTION_H
