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

//emphaticsoft includes
#include "RecoBase/LineSegment.h"
#include "Geometry/service/GeometryService.h"
#include "Simulation/Particle.h"
#include "TGeoToObjFile.h"
#include "TGeoToObjFile.cpp" //TODO: Don't include .cpp files.  How can I include this in the module binary without building a separate library using cet_modules?
#include "parseHTTP.cpp" //TODO: Don't include .cpp files.  How do I explain to ART that I want to build the object file from compiling this file into the same libraryas this module?

//ART includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//ROOT includes
#include "TVector3.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"

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
#include <sys/stat.h>

//c++ includes
#include <sstream>

namespace evd {
  class WebDisplay;
}

namespace
{
  constexpr int bufferSize = 2048; //Size of HTML chunks read into memory.
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
    fhicl::Atom<art::InputTag> lineSegLabel{Name("lineSegLabel"), Comment("Name of the module that produced SSD LineSegments.  Usually the cluster module."), "ssdclusts"};
    fhicl::Atom<art::InputTag> mcPartLabel{Name("mcPartLabel"), Comment("Name of the module that produced sim::Particles.  Usually the GEANT simulation."), "geantgen"};
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

  int sendFile(const char* fileName) const;
  int sendString(std::string toSend, const std::string contentType = "text") const; //Adds an appropriate header for a stand-alone message
  int sendRawString(const std::string& toSend) const; //Requires you to write your own header.  Used to implement sendFile() and sendString()
  int sendBadRequest() const;

  std::string writeGeometryList() const;
  std::string writeMCTrajList(const std::vector<sim::Particle>& trajs) const;
  std::string writeLineSegList(const std::vector<rb::LineSegment>& segs) const;

  TGeoShape* getShape(const std::string& shapeName) const;
  TGeoNode* getNode(const std::string& shapeName) const;

  char fBuffer[bufferSize];
};

evd::WebDisplay::WebDisplay(Parameters const& p)
  : EDAnalyzer{p}, fConfig(p()), fPortNumber(p().portNumber()), fMessageSocket(-1)
{
  consumes<std::vector<rb::LineSegment>>(p().lineSegLabel());
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
  constexpr int yes = 1;
  for(;myAddress != nullptr; myAddress = myAddress->ai_next)
  {
    listenSocket = socket(myAddress->ai_family, myAddress->ai_socktype, myAddress->ai_protocol);
    if(listenSocket < 0) continue; //Try the next address

    //My prototype reset the status of this socket here.  I left that out
    //so users on the same GPVM don't stomp on each other.  Try https://threejs.org/examples/webgl_lines_fat.html.
    //TODO: Pick another port number if this socket is already in use.
    //Reuse this socket if there's another process already using it
    if(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
      mf::LogError("PortSetup") << "setsockopt: " << strerror(errno);
    }

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

  //Wait for the browser to request the main webpage
  //TODO: Check that browser is actually requesting "/" with a GET.  Maybe even add "GET /" as a case in main recv() loop below in case the browser does things I don't expect?
  const int bytesRead = recv(fMessageSocket, fBuffer, bufferSize, 0); //Blocks until receives a request from the browser
  if(bytesRead < 0) mf::LogError("Server") << "recv: " << strerror(errno);
  mf::LogInfo("Server") << "Got initial message from browser with size of " << bytesRead << ":\n" << fBuffer;
  memset(fBuffer, '\0', bufferSize); //Clear the buffer so that it doesn't contain this message anymore!

  //Now, send the main display code itself.  This should trigger some requests from the browser.
  sendFile("webDisplay_v2.html");

  //Send a "waiting" screen while things like the geometry load
  //TODO: Add a logo?
  /*std::string welcomeScreen = "<!DOCTYPE html>\n";
  welcomeScreen += "<html>\n";
  welcomeScreen += "<link rel=\"shortcut icon\" href=\"data:image/x-icon;,\" type=\"image/x-icon\">\n"; //Needed to skip sending Chrome a favicon per https://stackoverflow.com/questions/1321878/how-to-prevent-favicon-ico-requests
  welcomeScreen += "  <head>\n";
  welcomeScreen += "    <title> Loading... </title>\n";
  welcomeScreen += "  </head>\n";
  welcomeScreen += "  <body>\n";
  welcomeScreen += "    <text> Loading geometry and first event... </text>\n";
  welcomeScreen += "  </body>\n";
  welcomeScreen += "  <meta http-equiv=\"refresh\" content=\"1\">\n";
  welcomeScreen += "</html>";
  welcomeScreen.insert(0, "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string(welcomeScreen.length()) + "\n\n");
  sendString(welcomeScreen);*/
}

//Summarize detectors to draw as a JSON list
std::string evd::WebDisplay::writeGeometryList() const
{
  //TODO: Only support a fixed list of volumes for now to get this thing working.  I already know how to get positions for the magnet, target, and SSDs

  std::stringstream geometryList;
  geometryList << "[\n";
  
  //Magnet
  art::ServiceHandle<emph::geo::GeometryService> geom;
  double nextMagnetBeginZ = geom->Geo()->MagnetUSZPos()/10.;
  const auto magnetNode = getNode("magnet_phys");
  for(const auto node: *magnetNode->GetNodes())
  {
    const double magnetHalfLength = static_cast<TGeoBBox*>(static_cast<TGeoNode*>(node)->GetVolume()->GetShape())->GetDZ()/10.;
    geometryList << "  {\n"
                 << "    \"name\": \"" << static_cast<TGeoNode*>(node)->GetVolume()->GetName() << "\",\n"
                 << "    \"isDetector\": false,\n"
                 << "    \"position\": [0, 0, " << nextMagnetBeginZ + magnetHalfLength << "],\n"
                 << "    \"phi\": 0\n"
                 << "  },\n";
    nextMagnetBeginZ += magnetHalfLength;
  }

  //Outlines of SSDs
  for(int whichStation = 0; whichStation < geom->Geo()->NSSDStations(); ++whichStation)
  {
    const auto station = geom->Geo()->GetSSDStation(whichStation);
    for(int whichPlane = 0; whichPlane < station->NPlanes(); ++whichPlane)
    {
      const auto plane = station->GetPlane(whichPlane);
      for(int whichSensor = 0; whichSensor < plane->NSSDs(); ++whichSensor)
      {
        const auto sensor = plane->SSD(whichSensor);
        geometryList << "  {\n"
                     << "    \"name\": \"ssdsensor_" << whichStation << "_" << whichPlane << "_" << whichSensor << "_vol\",\n"
                     << "    \"isDetector\": true,\n"
                     << "    \"position\": [" << sensor->Pos().X()/10. << ", " << sensor->Pos().Y()/10. << ", " << sensor->Pos().Z()/10. << "],\n"
                     << "    \"phi\": " << -sensor->Rot() << "\n"
                     << "  },\n";
      }
    }
  }

  //TODO: Names of target and SSD sensors must match some volume in the GDML file.  Go look up the names of their GDML volumes.
  //      I'd rather be able to give them useful names than whatever the shape is called in GDML
  //Target
  geometryList << "  {\n"
               << "    \"name\": \"target_vol\",\n"
               << "    \"isDetector\": false,\n"
               << "    \"position\": [0, 0, " << (geom->Geo()->TargetUSZPos() + geom->Geo()->TargetDSZPos())/2./10. << "],\n"
               << "    \"phi\": 0\n"
               << "  }\n"; //TODO: Making sure there's not a comma after this last entry is tricky and important!  It will be a bigger problem when I generate the full list of geometry shapes.

  /*for(const auto& detName: fGeoDetNames)
  {
    const TGeoNode* node = getNode(detName);
    //TODO: How to get full translation and rotation matrix from geometry hierarchy?
    //      I can GetMotherVolume() repeatedly, but that never gives me the mother node's matrix
    //      Go all the way up the hierarchy until I get to the world volume?  Doesn't it have some non-trivial center too, or is its center assumed to be the origin?
    //      Maybe I had better cache these?
    //      How do I handle volumes like the SSDs that have alignment data?
    geometryList << "{\n"
                 << "  \"name\": " << detName << ",\n"
                 << "  \"isDetector\": true,\n"
                 << "  \"position\": [" << << ", " << << ", " << << "]\n"
                 //TODO: Add support for rotations
                 << "}\n";
  }

  for(const auto& passiveName: fGeoPassiveNames)
  {
    const TGeoNode* node = getNode(detName);
    geometryList << "{\n"
                 << "  \"name\": " << detName << ",\n"
                 << "  \"isDetector\": false,\n"
                 << "  \"position\": [" << << ", " << << ", " << << "\n"
                 << "}\n";
  }*/

  geometryList << "]\n";

  return geometryList.str();
}

std::string evd::WebDisplay::writeMCTrajList(const std::vector<sim::Particle>& trajs) const
{
  std::stringstream trajList;
  trajList << "[\n";
  auto writeTraj = [&trajList](const auto& traj)
  {
    trajList << "{\n"
             << "  \"name\": \"" << traj.ftrajectory.Momentum(0).Vect().Mag()/1000. << "GeV/c" << traj.fpdgCode << "\",\n" //TODO: convert PDG code to LaTeX name using TDatabasePDG
             << "  \"pdgCode\": " << traj.fpdgCode << ",\n"
             << "  \"points\": [\n";
    for(size_t whichTrajPoint = 0; whichTrajPoint < traj.ftrajectory.size()-1; ++whichTrajPoint)
    {
      const auto& point = traj.ftrajectory.Position(whichTrajPoint);
      trajList << "[" << point.X()/10. << ", " << point.Y()/10. << ", " << point.Z()/10. << "],\n"; //Convert mm to cm
    }
    if(!traj.ftrajectory.empty())
    {
      const auto& lastPoint = traj.ftrajectory.Position(traj.ftrajectory.size()-1);
      trajList << "[" << lastPoint.X()/10. << ", " << lastPoint.Y()/10. << ", " << lastPoint.Z()/10. << "]\n";
    }
    trajList << "          ]\n"
             << "}";
  };

  if(!trajs.empty())
  {
    for(auto whichTraj = trajs.begin(); whichTraj < std::prev(trajs.end()); ++whichTraj)
    {
      writeTraj(*whichTraj);
      trajList << ",\n";
    }
    writeTraj(*std::prev(trajs.end()));
    trajList << "\n"; //Don't put a comma on end of last trajectory!  Very important for JSON apparently.
  }

  trajList << "]\n";
  return trajList.str();
}

std::string evd::WebDisplay::writeLineSegList(const std::vector<rb::LineSegment>& segs) const
{
  std::stringstream segList;
  segList << "[\n";

  auto writeSeg = [&segList](const auto& seg)
  {
    TVector3 x0 = seg.X0(),
             x1 = seg.X1();
    const auto diff = x1 - x0;
    const double length = diff.Mag()/10.; //Convert mm to cm for graphics reasons
    const double ssdWidth = 0.1;
    const auto center = (x0 + x1)*0.5*0.1; //Convert mm to cm for graphics reasons
    //const double theta = diff.Theta();
    const double phi = diff.Phi();

    segList << "{\n"
            << "  \"center\": [" << center.X() << ", " << center.Y() << ", " << center.Z() << "],\n"
            << "  \"length\": " << length << ",\n"
            << "  \"phi\": " << phi << "\n"
            << "}";
  };

  if(!segs.empty())
  {
    for(auto whichSeg = segs.begin(); whichSeg != std::prev(segs.end()); ++whichSeg)
    {
      writeSeg(*whichSeg);
      segList << ",\n";
    }
    writeSeg(*std::prev(segs.end()));
    segList << "\n";
  }

  segList << "]\n";
  return segList.str();
}

TGeoNode* evd::WebDisplay::getNode(const std::string& shapeName) const
{
  art::ServiceHandle<emph::geo::GeometryService> geom;
  const TGeoNode* world = geom->Geo()->ROOTGeoManager()->GetTopNode();
  if(!world) throw cet::exception("RequestedGeometry") << "Failed to find the geometry world volume!";

  TGeoNode* targetVol = world->GetVolume()->GetNode(shapeName.c_str());

  return targetVol;
}

TGeoShape* evd::WebDisplay::getShape(const std::string& shapeName) const
{
  //TODO: getNode() only searches the world node.  Replace or get rid of it.
  /*const auto node = getNode(shapeName);
  if(!node) throw cet::exception("RequestedGeometry") << "Failed to find " << shapeName << " node in the geometry!";

  return static_cast<TGeoNode*>(node)->GetVolume()->GetShape();*/
  
  art::ServiceHandle<emph::geo::GeometryService> geom;
  const auto found = geom->Geo()->ROOTGeoManager()->FindVolumeFast(shapeName.c_str());
  if(!found) throw cet::exception("Requested Geometry") << "Failed to find " << shapeName << " volume in the geometry!";
  return found->GetShape();
}

void evd::WebDisplay::analyze(art::Event const& e)
{
  //Respond to two types of requests from browser: POST and GET
  //First, wait for a POST request with the new event number.  Return either code 200 or some failure code based on whether or not this module can load that event.
  //Then, continue responding to GET requests for different resources:
  //geometry/index.json: list of geometry volumes to draw
  //geometry/<name of physvol>.obj: Convert TGeoShape to a .obj file and send it
  //MC/trajs.json: list of MC trajectories
  //reco/LineSegs.json: list of line segments
  int bytesRead = 0;
  mf::LogInfo("Server") << "Waiting for request from browser...";
  do
  {
    bytesRead = recv(fMessageSocket, fBuffer, bufferSize, 0); //Blocks until receives a request from the browser
    if(bytesRead < 0) mf::LogError("Server") << "recv: " << strerror(errno);
    mf::LogInfo("Server") << "Got a message from browser with size of " << bytesRead << ":\n" << fBuffer;

    const HTTPRequest request = parseHTTP(fBuffer); //TODO: Check for EAGAIN in case body is incomplete?
    if(request.method == HTTPRequest::Method::POST)
    {
      //TODO: Seek to requested event first.  Possibly send HTTP response with "failed" status if not possible.
      //sendBadRequest();
      //sendString("");
      sendRawString("HTTP/1.1 201 Created\nContent-Type:text/html\nContent-Length:0\n\n");
      break; //TODO: Can I rewrite the logic of this loop to remove break?
    }
    else if(request.method == HTTPRequest::Method::GET)
    {
      if(request.uri == "/geometry/index.json")
      {
        sendString(writeGeometryList(), "application/json");
      }
      else if(request.uri.find("geometry") != std::string::npos)
      {
        std::stringstream geomObjFile;
        const std::string geometryStem = "/geometry/";
        const TGeoShape* shape = getShape(request.uri.substr(request.uri.find(geometryStem) + geometryStem.length(), request.uri.find(".obj")));
        TGeoToObjFile(*shape, geomObjFile, 10.);
        sendString(geomObjFile.str(), "text/obj");
      }
      else if(request.uri.find("/LineSegs.json") != std::string::npos)
      {
        art::Handle<std::vector<rb::LineSegment>> segs;
        e.getByLabel(fConfig.lineSegLabel(), segs);
        if(segs)
        {
          sendString(writeLineSegList(*segs), "application/json");
        }
        else sendBadRequest(); //The frontend can ignore this request and keep going without showing LineSegments
      }
      else if(!strcmp(request.uri.c_str(), "/MC/trajs.json"))
      {
        art::Handle<std::vector<sim::Particle>> mcParts;
        e.getByLabel(fConfig.mcPartLabel(), mcParts);
        if(mcParts)
        {
          sendString(writeMCTrajList(*mcParts), "application/json");
        }
        else sendBadRequest(); //The frontend can ignore this request and keep going without showing MC trajectories.  Handling this is important for viewing data!
      }
      else
      {
        sendBadRequest();
      }
    }
    else //if(request.method == HTTPRequest::Method::UNSUPPORTED)
    {
      sendBadRequest();
    }
    memset(fBuffer, '\0', bufferSize); //Clear the buffer so that it doesn't contain this message anymore!
  } while(bytesRead > 0);
  if(bytesRead == 0)
  {
    mf::LogError("Server") << "Browser disconnected.";
  }
  memset(fBuffer, '\0', bufferSize); //Clear the buffer so that it doesn't contain this message anymore!
}

std::string getFullPath(const std::string& fileName)
{
  //Find the file by looking at the source code directory
  //TODO: I don't think this will work if we install on /cvmfs.  How do we look in the install directory instead?
  const char* fileLoc = getenv ("CETPKG_SOURCE");
  if(fileLoc == nullptr) mf::LogError("sendFile") << "Failed to find " << fileName << " at CETPKG_SOURCE (source directory) because CETPKG_SOURCE is not set!";
  return fileLoc + std::string("/EventDisplay/WebDisplay/") + fileName;
}

int evd::WebDisplay::sendBadRequest() const
{
  return sendRawString("HTTP/1.1 404 Not found\n\n"); //Is it appropriate to include more information in the response body for debugging?
}

void evd::WebDisplay::endJob()
{
  close(fMessageSocket);
}

int evd::WebDisplay::sendString(std::string toSend, const std::string contentType) const
{
  //Add an HTTP header
  toSend.insert(0, "HTTP/1.1 200 OK\nContent-Type:" + contentType + "\nContent-Length:" + std::to_string(toSend.length()) + "\n\n");
  return sendRawString(toSend);
}


int evd::WebDisplay::sendRawString(const std::string& toSend) const
{
  mf::LogWarning("HTTP Sent") << "Sending HTTP request:\n" << toSend;

  int bytesSent = 0;
  int offset = 0;
  do
  {
    bytesSent = send(fMessageSocket, toSend.c_str() + offset, toSend.length() - offset, 0);
    if(bytesSent < 0)
    {
      mf::LogError("WebServer") << "send: " << strerror(errno);
      return 2;
    }
    offset += bytesSent;
  }
  while(bytesSent > 0);

  return 0;
}

int evd::WebDisplay::sendFile(const char* fileName) const
{
  const std::string fileFullPath = getFullPath(fileName);

  struct stat fileInfo;
  stat(fileFullPath.c_str(), &fileInfo);
  const int contentLength = fileInfo.st_size;

  const std::string requestHeader = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string(contentLength) + "\n\n";
  sendRawString(requestHeader);

  std::cout << "Sending file named " << fileName << "...\n";
  constexpr int fileChunkSize = 512;
  char fileData[fileChunkSize];

  FILE* file = fopen(fileFullPath.c_str(), "rb");

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
