#include "web/Connection.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

namespace web
{
  Connection::Connection(const char* port)
  {
    struct addrinfo hints, *myAddress;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; //TCP-IP like the internet as opposed to UDP
    hints.ai_flags = AI_PASSIVE; //Fill in with my own IP address
  
    int status;
    if((status = getaddrinfo(nullptr, port, &hints, &myAddress)))
    {
      std::cerr << "getaddrinfo: " << gai_strerror(status);
    }
  
    //getaddrinfo() returns lots of addresses that may or may not work in a linked list.
    //Pick the first one that can be used.
    fListenSocket = -1;
    constexpr int yes = 1;
    for(;myAddress != nullptr; myAddress = myAddress->ai_next)
    {
      fListenSocket = socket(myAddress->ai_family, myAddress->ai_socktype, myAddress->ai_protocol);
      if(fListenSocket < 0) continue; //Try the next address
  
      //My prototype reset the status of this socket here.  I left that out
      //so users on the same GPVM don't stomp on each other.  Try https://threejs.org/examples/webgl_lines_fat.html.
      //TODO: Pick another port number if this socket is already in use.
      //Reuse this socket if there's another process already using it
      if(setsockopt(fListenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
      {
        std::cerr << "setsockopt: " << strerror(errno);
      }

      if(bind(fListenSocket, myAddress->ai_addr, myAddress->ai_addrlen) < 0)
      {
        close(fListenSocket);
        std::cerr << "bind: " << strerror(errno);
        continue; //Try another address
      }
  
      //If I got this far, then this address works!  Use it.
      break;
    }
    if(myAddress == nullptr) //If the loop never got to "break"
    {
      std::cerr << "Failed to get my own address info.";
    }
  
    //Wait for a web browser to contact this process over fSocketNumber, then accept() the connection
    if(listen(fListenSocket, 10) < 0)
    {
      std::cerr << "listen: " << strerror(errno);
    }
  
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size = sizeof(their_addr);
    fMessageSockets.push_back(accept(fListenSocket, (sockaddr*)&their_addr, &their_addr_size));
    if(fMessageSockets[0] < 0) std::cerr << "accept: " << strerror(errno);
  }

  int Connection::sendString(const std::string& toSend, const int messageSocket) const
  {
    int bytesSent = 0;
    int offset = 0;
    do
    {
      bytesSent = send(messageSocket, toSend.c_str() + offset, toSend.length() - offset, 0);
      if(bytesSent < 0)
      {
        std::cerr << "send: " << strerror(errno);
        return 2;
      }
      offset += bytesSent;
    }
    while(bytesSent > 0);

    return 0;
  }

  int Connection::sendFile(const std::string& fileFullPath, const int messageSocket) const
  {
    //std::cout << "Sending file named " << fileName << "...\n";
    constexpr int fileChunkSize = 512;
    char fileData[fileChunkSize];
  
    FILE* file = fopen(fileFullPath.c_str(), "rb");
    if(!file)  throw std::runtime_error("No such file or directory: " + fileFullPath);
  
    int bytesRead = 0;
    int bytesSent = 0;
    do
    {
      //std::cout << "Keep reading...\n";
      bytesRead = fread(fileData, sizeof(char), fileChunkSize, file);
      int bytesLeftThisChunk = bytesRead;
      int offset = 0;
      do
      {
        //std::cout << "Keep sending.  offset is " << offset << "...\n";
        bytesSent = send(messageSocket, fileData + offset, bytesRead, 0);
        if(bytesSent < 0)
        {
          std::cerr << "send: " << strerror(errno) << "\n";
          return 1;
        }
  
        offset += bytesSent;
        bytesLeftThisChunk -= bytesSent;
      }
      while(bytesLeftThisChunk > 0);
    } while(bytesRead > 0);
    //std::cout << "Done sending " << fileFullPath << ".\n";
  
    return 0;
  }

  Connection::~Connection()
  {
    close(fListenSocket);
    for(auto conn: fMessageSockets) close(conn);
  }
}
