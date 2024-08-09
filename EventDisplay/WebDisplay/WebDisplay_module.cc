////////////////////////////////////////////////////////////////////////
// Class:       WebDisplay
// Plugin Type: analyzer
// File:        WebDisplay_module.cc
//
// Generated at Fri Aug  9 12:40:36 2024 by Andrew Olivier using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////

//An ART module that reads SSD LineSegments from an ART-ROOT file and
//serves a web page that displays them.  This module is a trivial web
//server!  To use it, you have to log in to the GPVM running this module
//with the right port forwarded: ssh -L 9999:localhost:9999 <username>@emphaticgpvm99.fnal.gov

//TODO: Customize the port number used so multiple users on the same GPVM don't clash!
//      Right now, that "clash" would look like getting an error message early in the job.

//NB: Only works on POSIX-compliant platforms because of socket interface.
//    That's all we use in reality anyway.  Try the Boost sockets library
//    if you need a different platform one day.  Or better yet, use a
//    real web server.

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//POSIX includes for sockets
#include <string.h> //memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

namespace evd {
  class WebDisplay;
}

namespace
{
  constexpr int bufferSize = 1024; //Size of HTML chunks read into memory.
                                   //Make it large compared to anything
                                   //we'll reasonably encounter.
}

class evd::WebDisplay : public art::EDAnalyzer {
public:
  struct Config
  {
    using Name=fhicl::Name;
    using Comment=fhicl::Comment;
    fhicl::Atom<int> portNumber{Name("portNumber"), Comment("Port forwarded to web broswer"), 3490};
  };

  typedef art::EDAnalyzer::Table<Config> Parameters;

  explicit WebDisplay(Parameters const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  WebDisplay(WebDisplay const&) = delete;
  WebDisplay(WebDisplay&&) = delete;
  WebDisplay& operator=(WebDisplay const&) = delete;
  WebDisplay& operator=(WebDisplay&&) = delete;

  // Optional functions
  void beginJob() override;
  void endJob() override;

  // Required functions.
  void analyze(art::Event const& e) override;

private:

  // Declare member data here.
  Config fConfig;
  int fPortNumber; //Port you need to forward and point your browser to!
                   //Can be configured by a FHICL parameter.
  int fMessageSocket; //File descriptor HTTP response socket

  int sendEvent(const art::Event& e) const;
  int sendFile(const char* fileName) const;
  int sendString(const std::string& toSend) const;

  char fBuffer[bufferSize];
};

evd::WebDisplay::WebDisplay(Parameters const& p)
  : EDAnalyzer{p}, fConfig(p()), fPortNumber(p().portNumber()), fMessageSocket(-1)
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
  //consumes<recob::SSDLineSegment>();
}

//Set up socket for web communication with a browser.  This will block
//until a browser connects.
//TODO: Move the blocking part into analyze() so reconstruction could
//      in principle run while I'm setting up the web browser.
void evd::WebDisplay::beginJob()
{
  struct addrinfo hints, *myAddress;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM; //TCP-IP like the internet as opposed to UDP
  hints.ai_flags = AI_PASSIVE; //Fill in with my own IP address

  int status;
  const auto portString = std::to_string(fPortNumber);
  if((status = getaddrinfo(nullptr, portString.c_str(), &hints, &myAddress)))
  {
    mf::LogError("PortSetup") << "getaddrinfo: " << gai_strerror(status);
  }

  //getaddrinfo() returns lots of addresses that may or may not work in a linked list.
  //Pick the first one that can be used.
  int listenSocket = -1;
  for(;myAddress != nullptr; myAddress = myAddress->ai_next)
  {
    listenSocket = socket(myAddress->ai_family, myAddress->ai_socktype, myAddress->ai_protocol);
    if(listenSocket < 0) continue; //Try the next address

    //My prototype reset the status of this socket here.  I left that out
    //so users on the same GPVM don't stomp on each other.
    //TODO: Pick another port number if this socket is already in use.

    if(bind(listenSocket, myAddress->ai_addr, myAddress->ai_addrlen) < 0)
    {
      close(listenSocket);
      mf::LogWarning("PortSetup") << "bind: " << strerror(errno);
      continue; //Try another address
    }

    //If I got this far, then this address works!  Use it.
    break;
  }
  if(myAddress == nullptr) //If the loop never got to "break"
  {
    mf::LogError("PortSetup") << "Failed to get my own address info.";
  }

  //Wait for a web browser to contact this process over fPortNumber, then accept() the connection
  if(listen(listenSocket, 10) < 0)
  {
    mf::LogError("PortSetup") << "listen: " << strerror(errno);
  }

  struct sockaddr_storage their_addr;
  socklen_t their_addr_size = sizeof(their_addr);
  fMessageSocket = accept(listenSocket, (sockaddr*)&their_addr, &their_addr_size);
  if(fMessageSocket < 0) mf::LogError("PortSetup") << "accept: " << strerror(errno);

  mf::LogInfo("PortSetup") << "Got a connection!";
  close(listenSocket);
  //TODO: Clean up addrinfo structs
  //TODO: Send a "waiting" screen here, possibly with a logo
}

void evd::WebDisplay::analyze(art::Event const& e)
{
  //Serve this event, then wait for the web browser to send
  //ANY request before moving on to the next event.
  int bytesRead = 0;
  mf::LogInfo("Server") << "Waiting for request from browser...";
  bytesRead = recv(fMessageSocket, fBuffer, bufferSize, 0); //Blocks until receives a request from the browser
  //TODO: recv() doesn't block sometimes.  I get empty messages when this happends.  Am I connecting the browser too early?  Is this a container problem?  Am I triggering some redundancy behavior in HTML?
  if(bytesRead < 0) mf::LogError("Server") << "recv: " << strerror(errno);
  mf::LogInfo("Server") << "Got a message from browser with size of " << bytesRead << ":\n" << fBuffer;
  if(bytesRead == 0)
  {
    mf::LogError("Server") << "Browser disconnected.";
  }
  sendEvent(e);
  memset(fBuffer, '\0', bufferSize);
}

int evd::WebDisplay::sendEvent(const art::Event& e) const
{
  const int eventNumber = e.id().event();

  //TODO: Dummy event setup just to show that this works.  Replace with real SSD LineSegment positions.
  //Generate source code for variables that set up the cubes to draw
  std::string cubeSetup = "const cubeXPositions = [\n";
  for(int whichEvent = 0; whichEvent < eventNumber; ++whichEvent) cubeSetup += std::to_string(whichEvent*2) + ",\n";
  cubeSetup += "0];\n";

  const int contentLength = 630 + cubeSetup.size() + 3215; //TODO: hard-coded script sizes from wc -c.  Replace with c++ check of file size.  I'm already assuming a POSIX platform by using send() and recv(), so stat() should do the job.
  //const std::string simpleTestSource = "<!DOCTYPE html>\n<h1>Hello, world!</h1>";
  //const int contentLength = simpleTestSource.size();
  const std::string requestHeader = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string(contentLength) + "\n\n";
  //sendString(requestHeader);
  //return sendString(simpleTestSource); //TODO: Remove this simple test after I get it working

  //First, send the beginning of an HTML response
  int returnCode = sendString(requestHeader);
  if(returnCode != 0) return returnCode;

  //Next, send beginning of file
  returnCode = sendFile("header.html");
  if(returnCode != 0) return returnCode;

  //Next, send cube positions
  returnCode = sendString(cubeSetup);
  if(returnCode != 0) return returnCode;

  //Last, send the script that draws the cubes
  return sendFile("footer.html");
}

void evd::WebDisplay::endJob()
{
  close(fMessageSocket);
}

int evd::WebDisplay::sendString(const std::string& toSend) const
{
  int bytesSent = 0;
  int offset = 0;
  do
  {
    bytesSent = send(fMessageSocket, toSend.c_str() + offset, toSend.size() - offset, 0);
    if(bytesSent < 0)
    {
      std::cerr << "send: " << strerror(errno) << "\n";
      return 2;
    }
    offset += bytesSent;
  }
  while(bytesSent > 0);

  return 0;
}

int evd::WebDisplay::sendFile(const char* fileName) const
{
  std::cout << "Sending file named " << fileName << "...\n";
  constexpr int fileChunkSize = 512;
  char fileData[fileChunkSize];

  FILE* file = fopen(fileName, "rb");

  int bytesRead = 0;
  int bytesSent = 0;
  do
  {
    std::cout << "Keep reading...\n";
    bytesRead = fread(fileData, sizeof(char), fileChunkSize, file);
    int bytesLeftThisChunk = bytesRead;
    int offset = 0;
    do
    {
      std::cout << "Keep sending.  offset is " << offset << "...\n";
      bytesSent = send(fMessageSocket, fileData + offset, bytesRead, 0);
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
  std::cout << "Done sending " << fileName << ".\n";

  return 0;
}

DEFINE_ART_MODULE(evd::WebDisplay)
