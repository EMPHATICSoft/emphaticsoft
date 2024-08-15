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

  int sendEvent(const art::Event& e) const;
  int sendFile(const char* fileName) const;
  int sendString(const std::string& toSend) const;

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

  //Send a "waiting" screen while things like the geometry load
  //TODO: Add a logo?
  std::string welcomeScreen = "<!DOCTYPE html>\n";
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
  sendString(welcomeScreen);
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

std::string getFullPath(const std::string& fileName)
{
  //Find the file by looking at the source code directory
  //TODO: I don't think this will work if we install on /cvmfs.  How do we look in the install directory instead?
  const char* fileLoc = getenv ("CETPKG_SOURCE");
  if(fileLoc == nullptr) mf::LogError("sendFile") << "Failed to find " << fileName << " at CETPKG_SOURCE (source directory) because CETPKG_SOURCE is not set!";
  return fileLoc + std::string("/EventDisplay/WebDisplay/") + fileName;
}

int evd::WebDisplay::sendEvent(const art::Event& e) const
{
  //Render reconstruction
  const auto lineSegs = e.getValidHandle<std::vector<rb::LineSegment>>(fConfig.lineSegLabel());

  std::string cubeSetup;
  for(const auto& line: *lineSegs)
  {
    TVector3 x0 = line.X0(),
             x1 = line.X1();
    const auto diff = x1 - x0;
    const double length = diff.Mag()/10.; //Convert mm to cm for graphics reasons
    const double ssdWidth = 0.1;
    const auto center = (x0 + x1)*0.5*0.1; //Convert mm to cm for graphics reasons
    //const double theta = diff.Theta();
    const double phi = diff.Phi(); 

    //TODO: Replace these string concatenation operators with stringstream or something better
    cubeSetup += "  {\n";
    cubeSetup += "    const ssdGeom = new THREE.BoxGeometry(ssdWidth, " + std::to_string(length) + ", ssdWidth);\n";
    cubeSetup += "    const ssdBox = new THREE.Mesh(ssdGeom, ssdMaterial);\n";
    cubeSetup += "    ssdBox.position.set("+ std::to_string(center.X()) +", "+ std::to_string(center.Y()) +", "+ std::to_string(center.Z()) +");\n";
    cubeSetup += "    ssdBox.rotation.z =" + std::to_string(-phi) + "\n;"; //Angle convention seems to be opposite between ROOT and Three.js based on run 1c data
    cubeSetup += "    ssdBox.name = \"LineSegment\";\n";
    cubeSetup += "    scene.add(ssdBox);\n";
    cubeSetup += "  }\n";
    //TODO: Change box colors here
  }
  cubeSetup += "}\n";

  //Render simulation
  cubeSetup += "function setupMCTrajectories(scene) {\n";

  //Set up table of materials based on PDG code
  //Solid lines for charged particles
  //Dashed lines for neutral particles and unknowns
  cubeSetup += "  const mcLineWidth = 3;\n"; //This is supposed to always be stuck at 1 on most browsers.  For portable wide lines, try https://threejs.org/examples/webgl_lines_fat.html
  cubeSetup += "  const pdgToMaterialMap = new Map();\n";
  cubeSetup += "  pdgToMaterialMap.set(2212, new THREE.LineBasicMaterial({ color: 0x00ff00, linewidth: mcLineWidth }));\n";
  cubeSetup += "  pdgToMaterialMap.set(2112, new THREE.LineDashedMaterial({ color: 0x808080, linewidth: mcLineWidth, dashSize: 1, gapSize: 2 }));\n";
  cubeSetup += "  pdgToMaterialMap.set(211, new THREE.LineBasicMaterial({ color: 0xff0000, linewidth: mcLineWidth }));\n";
  cubeSetup += "  pdgToMaterialMap.set(111, new THREE.LineDashedMaterial({ color: 0xff0000, linewidth: mcLineWidth, dashSize: 1, gapSize: 2 }));\n";
  cubeSetup += "  pdgToMaterialMap.set(321, new THREE.LineBasicMaterial({ color: 0xffff00, linewidth: mcLineWidth }));\n";
  cubeSetup += "  pdgToMaterialMap.set(311, new THREE.LineDashedMaterial({ color: 0xffff00, linewidth: mcLineWidth, dashSize: 1, gapSize: 2 }));\n";
  cubeSetup += "  pdgToMaterialMap.set(13, new THREE.LineBasicMaterial({ color: 0x00ffff, linewidth: mcLineWidth }));\n";
  cubeSetup += "  pdgToMaterialMap.set(11, new THREE.LineBasicMaterial({ color: 0xff00ff, linewidth: mcLineWidth }));\n";
  //TODO: Photon
  cubeSetup += "  const unknownPDGMaterial = new THREE.LineDashedMaterial({ color: 0xffffff, linewidth: mcLineWidth, dashSize: 1, gapSize: 2 });\n";

  art::Handle<std::vector<sim::Particle>> mcParts;
  e.getByLabel(fConfig.mcPartLabel(), mcParts);
  if(mcParts.isValid()) //A simple way to skip this step for data and recover gracefully for simulation configuration problems
  {
    mf::LogInfo("MC Drawing") << "Drawing " << mcParts->size() << " MC trajectories.";
    for(const auto& part: *mcParts)
    {
      cubeSetup += "  {\n";
      cubeSetup += "    const points = [];\n";
      for(size_t whichTrajPoint = 0; whichTrajPoint < part.ftrajectory.size(); ++whichTrajPoint)
      {
        const auto& pos = part.ftrajectory.Position(whichTrajPoint);
        cubeSetup += "    points.push(new THREE.Vector3(" + std::to_string(pos.X()/10.) + ", " + std::to_string(pos.Y()/10.) + ", " + std::to_string(pos.Z()/10.) + "));\n"; //Convert mm to cm
      }
      cubeSetup += "    const geometry = new THREE.BufferGeometry().setFromPoints(points);\n";
      cubeSetup += "    let pdgMaterial = pdgToMaterialMap.get(" + std::to_string(part.fpdgCode) + ");\n";
      cubeSetup += "    if(!pdgMaterial) { pdgMaterial = unknownPDGMaterial; }\n";
      cubeSetup += "    const line  = new THREE.Line(geometry, pdgMaterial);\n";
      cubeSetup += "    line.computeLineDistances();\n"; //Necessary for dashed lines to work
      cubeSetup += "    line.name = \"" + std::to_string(part.ftrajectory.Momentum(0).Vect().Mag()/1000.) + " GeV/c " + part.fpdgCode + "\";\n"; //TODO: Get name from PDG code and confirm momentum units
      cubeSetup += "    scene.add(line);\n";
      cubeSetup += "  }\n";
    }
  }
  else mf::LogInfo("MC Drawing") << "No sim::Particles to draw.";
  cubeSetup += "}\n";

  //Render static geometry
  art::ServiceHandle<emph::geo::GeometryService> geom;
  cubeSetup += "  function setupReferenceGeometry(scene) {\n";
  cubeSetup += "    const referenceMaterial = new THREE.MeshPhongMaterial({\n";
  cubeSetup += "      color: 0xff0000,\n";
  cubeSetup += "      opacity: 0.2,\n";
  cubeSetup += "      transparent: true,\n";
  cubeSetup += "      side: THREE.DoubleSide,\n";  //There's more transparency details that I'm omitting.  See https://threejs.org/manual/#en/transparency
  cubeSetup += "    });\n";

  if(geom->Geo()->MagnetLoad()) //Only show the magnet if MagnetLoad() is true
  {
    cubeSetup += "    const magnetGeom = new THREE.CylinderGeometry(10, 10, " + std::to_string((geom->Geo()->MagnetDSZPos() - geom->Geo()->MagnetUSZPos())/10.) + ", 12);\n"; //Converting from mm to cm for graphics reasons
    cubeSetup += "    const magnetCylinder = new THREE.Mesh(magnetGeom, referenceMaterial);\n";
    cubeSetup += "    magnetCylinder.position.set(0, 0, " + std::to_string((geom->Geo()->MagnetUSZPos() + geom->Geo()->MagnetDSZPos())/2./10.) + ");\n"; //Converting from mm to cm for graphics reasons
    cubeSetup += "    magnetCylinder.rotation.x = 3.1415926535897932384626433832/2; //TODO: Remember the Javascript name for pi\n";
    cubeSetup += "    magnetCylinder.name = \"magnet\";\n";
    cubeSetup += "    scene.add(magnetCylinder);\n";
  }
  cubeSetup += "    const targetGeom = new THREE.BoxGeometry(5, 5, " + std::to_string((geom->Geo()->TargetDSZPos() - geom->Geo()->TargetUSZPos())/10.) + ");\n"; //Converting from mm to cm for graphics reasons
  cubeSetup += "    const targetBox = new THREE.Mesh(targetGeom, referenceMaterial);\n";
  cubeSetup += "    targetBox.position.set(0, 0, " + std::to_string((geom->Geo()->TargetUSZPos() + geom->Geo()->TargetDSZPos())/2./10.) + ");\n"; //Converting from mm to cm for graphics reasons
  std::string totalTargetName = "`target\\n";
  const auto target = geom->Geo()->GetTarget();
  for(int whichElem = 0; whichElem < target->NEl(); ++whichElem)
  {
    totalTargetName += target->El(whichElem) + " : " + std::to_string(target->Frac(whichElem)) + "\\n";
  }
  cubeSetup += "    targetBox.name = " + totalTargetName + "`;\n";
  cubeSetup += "    scene.add(targetBox);\n";

  //Add transparent boxes for SSD positions.  Don't show SSDs that aren't loaded for this run.
  //TODO: These aren't visible right now because the geometry is returning 0s for their parameters
  cubeSetup += "  const ssdReferenceMaterial = new THREE.MeshPhongMaterial({\n";
  cubeSetup += "    color: 0x0000ff,\n";
  cubeSetup += "    opacity: 0.2,\n";
  cubeSetup += "    transparent: true,\n";
  cubeSetup += "    side: THREE.DoubleSide,\n";  //There's more transparency details that I'm omitting.  See https://threejs.org/manual/#en/transparency
  cubeSetup += "  });\n";
  for(int whichStation = 0; whichStation < geom->Geo()->NSSDStations(); ++whichStation)
  {
    const auto station = geom->Geo()->GetSSDStation(whichStation);
    for(int whichPlane = 0; whichPlane < station->NPlanes(); ++whichPlane)
    {
      const auto plane = station->GetPlane(whichPlane);
      for(int whichSensor = 0; whichSensor < plane->NSSDs(); ++whichSensor)
      {
        const auto sensor = plane->SSD(whichSensor); //geom->Geo()->GetSSDSensor(whichSensor);
        cubeSetup += "  {\n";
        cubeSetup += "    const sensorGeom = new THREE.BoxGeometry(" + std::to_string(sensor->Width()/10.) + ", " + std::to_string(sensor->Height()/10.) + ", " + std::to_string(sensor->Dz()/10.) + ");\n"; //Converting mm to cm for graphics reasons
        cubeSetup += "    const sensorBox = new THREE.Mesh(sensorGeom, ssdReferenceMaterial);\n";
        cubeSetup += "    sensorBox.position.set(" + std::to_string(sensor->Pos().X()/10.) + ", " + std::to_string(sensor->Pos().Y()/10.) + ", " + std::to_string((sensor->Pos().Z() + station->Pos().Z())/10.) + ");\n";
        cubeSetup += "    sensorBox.rotation.z = " + std::to_string(-sensor->Rot()) + ";\n";
        cubeSetup += "    sensorBox.name = \"station " + std::to_string(whichStation) + " plane " + std::to_string(whichPlane) + " sensor " + std::to_string(whichSensor) + "\";\n";
        cubeSetup += "    scene.add(sensorBox);\n";
        cubeSetup += "  }\n";
      }
    }
  }  

  cubeSetup += "return [referenceMaterial, ssdReferenceMaterial];\n";
  cubeSetup += "  }\n";
  cubeSetup += "var eventID = {\n";
  cubeSetup += "                run: " + std::to_string(e.id().run()) + ",\n";
  cubeSetup += "                subrun: " + std::to_string(e.id().subRun()) + ",\n";
  cubeSetup += "                event: " +  std::to_string(e.id().event()) + "\n";
  cubeSetup += "              }\n";

  //mf::LogInfo("WebDisplay") << "cubeSetup:\n" << cubeSetup;

  //Use the POSIX stat() API to get file sizes.  Since we're using socket() anyway, I can assume a POSIX operating system.
  int contentLength = cubeSetup.size();
  struct stat fileInfo;
  stat(getFullPath("header.html").c_str(), &fileInfo);
  contentLength += fileInfo.st_size;
  
  stat(getFullPath("footer.html").c_str(), &fileInfo);
  contentLength += fileInfo.st_size;

  const std::string requestHeader = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:" + std::to_string(contentLength) + "\n\n";

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
  const std::string fileFullPath = getFullPath(fileName);

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
