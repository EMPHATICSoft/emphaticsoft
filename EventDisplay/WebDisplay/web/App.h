//File: App.h
//Brief: A web::App interacts with a Javascript program from c++.
//       It has zero or more user-provided handler functions that
//       each handle a category of HTTP requests.  It communicates
//       with a web browser using its parent web::Connection.
//
//       Its first priority is to MAKE analyze() SIMPLER and EASIER
//       TO WRITE.  Performance is a secondary concern for what I
//       plan to do with web::App.
//
//       Notice that web::App<> is a class template that generates
//       classes.  It passes arbitrary parameters to its handler
//       functions when run() is called.
//
//       A concrete example: web::App<const art::Event&>
//       Each handler is a function that looks like:
//       std::unique_ptr<web::Repsonse> func(const art::Event&)
//       The web::App<const art::Event&> will be used in an ART
//       module like this:
//
//       void evd::WebDisplay::analyze(const art::Event& e)
//       {
//         fConnections.sendString(makeNewEvent(e));
//
//         web::App<const art::Event&> app(fConnections);
//         app.add([](const art::Event& e) { return new web::StringResponse(e.id().event()); }); //A trivial web::App handler
//         //Other handlers...
//
//         app.run(e);
//         return;
//       }
//Author: Andrew Olivier aolivie4@nd.edu

#include "web/Connection.h"
#include "web/Request.h"
#include "web/Response.h"

#include <regex>

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

#ifndef WEB_APP_HPP
#define WEB_APP_HPP

namespace web
{
  class Connection;
  class Response;
  struct Request;

  template <class ...ARGS>
  class App
  {
    public:
      App(web::Connection* parent);
      ~App() = default;

      //A handler_t holds any callable object.  That can be a simple function (pointer),
      //a lambda function (including with capture), or any object that implements
      //web::Reponse* operator ()(const web::Request&, ARGS...).
      //Accepting objects with a function call operator lets us write adapters like
      //web::Response* emph::WebAdapter(std::function<std::string, const std::vector<PROD>&)
      //that do a lot of work for us under the hood.
      using handler_t = std::function<web::Response*(const std::smatch&, const web::Request&, ARGS...)>;

      void add(const std::string urlMatcher, const Request::Method method, handler_t&& handler);

      //Wait on web::Requests until one of fHandlers returns web::EndAppResponse
      int run(ARGS... args);

    private:
      web::Connection* fParent; //Non-owned pointer to communication protocol
      std::unordered_map<Request::Method, std::vector<std::pair<std::regex, handler_t>>> fHandlers;

      static constexpr int BUFFER_SIZE=2048;
      char fBuffer[BUFFER_SIZE];
  };

  template <class ...ARGS>
  App<ARGS...>::App(web::Connection* parent): fParent(parent), fHandlers()
  {
    memset(fBuffer, '\0', BUFFER_SIZE);
  }

  template <class ...ARGS>
  void App<ARGS...>::add(const std::string urlMatcher, const Request::Method method, handler_t&& handler)
  {
    fHandlers[method].push_back(std::make_pair(std::regex(urlMatcher), handler));
  }

  template <class ...ARGS>
  int App<ARGS...>::run(ARGS... args)
  {
    std::cout << "Starting web::App::run()\n";
    std::vector<struct pollfd> pollFDs;
    {
      struct pollfd listenEntry;
      listenEntry.fd = fParent->fListenSocket;
      listenEntry.events = POLLIN;
      pollFDs.push_back(listenEntry);
    }
    for(const int socket: fParent->fMessageSockets)
    {
      struct pollfd messageEntry;
      messageEntry.fd = socket;
      messageEntry.events = POLLIN;
      pollFDs.push_back(messageEntry);
    }

    std::cout << "Before starting poll() loop, pollFDs.size() is " << pollFDs.size() << "\n";

    std::vector<int> socketsToRemove;
    do
    {
      const int nEvents = poll(pollFDs.data(), pollFDs.size(), 100); //100ms polling interval
      if(nEvents > 0)
      {
        std::cout << "Got a non-listen event!\n";
        for(size_t whichSocket = 1; whichSocket < pollFDs.size(); ++whichSocket)
        {
          if(pollFDs[whichSocket].revents & POLLIN)
          {
            const int messageSocket = pollFDs[whichSocket].fd;
            const int bytesRead = recv(messageSocket, fBuffer, BUFFER_SIZE, 0); //Blocks until receives a request from the browser
            if(bytesRead < 0) std::cerr << "recv: " << strerror(errno);
            //mf::LogInfo("Server") << "Got a message from browser with size of " << bytesRead << ":\n" << fBuffer;

            //TODO: Don't necessarily quit the loop if bytesRead is 0.  That might just mean that an ephemeral socket has closed.
            //      Is this necessary for talking to Chrome?
            if(bytesRead == 0)
            {
              socketsToRemove.push_back(whichSocket);
            }

            const Request request = parseHTTP(fBuffer); //TODO: Check for EAGAIN in case body is incomplete?

            for(const auto& handler: fHandlers[request.method])
            {
              std::smatch matchResults;
              if(std::regex_match(request.uri, matchResults, handler.first))
              {
                std::unique_ptr<web::Response> response(handler.second(matchResults, request, args...));
                if(response->resolve(*fParent, messageSocket) > 0) //TODO: How does EndAppResponse communicate that it's time to return?
                {
                  memset(fBuffer, '\0', BUFFER_SIZE);
                  fParent->fNextEventSocket = messageSocket;
                  return 0;
                }
              }
            }

            memset(fBuffer, '\0', BUFFER_SIZE);
          } //If pollFDs[whichSocket] has input
        } //Loop over sockets

        //Handle "ephemeral ports" that web browsers open to load resources in parallel
        if(pollFDs[0].revents & POLLIN)
        {
          std::cout << "Got a new port request!\n";
          struct sockaddr_storage their_addr;
          socklen_t their_addr_size = sizeof(their_addr);
          const int newConnection = accept(fParent->fListenSocket, (sockaddr*)&their_addr, &their_addr_size);
          if(newConnection < 0) std::cerr << "accept: " << strerror(errno);
          else
          {
            fParent->fMessageSockets.push_back(newConnection);
  
            struct pollfd messageEntry;
            messageEntry.fd = newConnection;
            messageEntry.events = POLLIN;
            pollFDs.push_back(messageEntry);
          }
        } //If the listen socket has an event
      } //If there were socket events

      //Remove all sockets that the client closed
      for(const int whichSocket: socketsToRemove)
      {
        fParent->fMessageSockets.erase(fParent->fMessageSockets.begin()+whichSocket-1);
      }
  
      pollFDs.clear();
      struct pollfd listenFD;
      listenFD.fd = fParent->fListenSocket;
      listenFD.events = POLLIN;
      pollFDs.push_back(listenFD);
  
      for(const int fd: fParent->fMessageSockets)
      {
        struct pollfd messageFD;
        messageFD.fd = fd;
        messageFD.events = POLLIN;
        pollFDs.push_back(messageFD);
      }
  
      socketsToRemove.clear();
    }
    while(!fParent->fMessageSockets.empty());

    return 0;
  } //web::App::run()
}

#endif //WEB_APP_HPP
