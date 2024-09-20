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
#include "EventDisplay/WebDisplay/UserHighlight.h"
#include "TGeoToObjFile.h"
#include "TGeoToObjFile.cpp" //TODO: Don't include .cpp files.  How can I include this in the module binary without building a separate library using cet_modules?
#include "EvtDisplayNavigatorService.h"

#include "EventDisplay/WebDisplay/web/Connection.h"
#include "EventDisplay/WebDisplay/web/App.h"
#include "EventDisplay/WebDisplay/web/Response.h"

#include "nlohmann/json.hpp"

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
#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"

//ROOT includes
#include "TVector3.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TDatabasePDG.h"

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
#include <time.h>

//c++ includes
#include <sstream>
#include <array>

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
    fhicl::Atom<art::InputTag> lineSegAssnsLabel{Name("lineSegAssnsLabel"), Comment("Name of the module that produced UserHighlights for rb::LineSegs."), "ssdclusters"};
    fhicl::Atom<art::InputTag> mcPartAssnsLabel{Name("mcPartAssnsLabel"), Comment("Name of the module that produced UserHighlights for sim::Particles."), "geantgen"};
    fhicl::Sequence<std::string> extraGeometryNodes{Name("extraGeometryNodes"), Comment("Node names of extra geometry objcts to draw from the GDML."), std::vector<std::string>()};
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

  web::Connection fBrowserConnection;

  //TODO: Refactor these into lambda functions in emph::WebDisplay::analyze()
  std::string writeGeometryList() const;

  template <class PROD>
  std::ostream& writeUserColors(std::ostream& assnsMap, const art::Event& e, const art::InputTag& label) const;

  TGeoShape* getShape(const std::string& shapeName) const;
  TGeoNode* getNode(const std::string& shapeName) const;
};

evd::WebDisplay::WebDisplay(Parameters const& p)
  : EDAnalyzer{p}, fConfig(p()), fBrowserConnection(std::to_string(p().portNumber()).c_str())
{
  consumes<std::vector<rb::LineSegment>>(p().lineSegLabel());
  consumes<std::vector<sim::Particle>>(p().mcPartLabel());
}

//Set up socket for web communication with a browser.  This will block
//until a browser connects.
void evd::WebDisplay::beginJob()
{
  art::ServiceHandle<emph::EvtDisplayNavigatorService> navigator;

  web::App<> evdApp(&fBrowserConnection);
  evdApp.add("/newEvent", web::Request::Method::POST, [&navigator](const std::smatch& matches, const web::Request& request) -> web::Response*
  {
    //Parse message body
    nlohmann::json body = nlohmann::json::parse(request.body);
    const int targetRun = body["run"];
    const int targetSubrun = body["subrun"];
    const int targetEvent = body["event"];

    if(targetRun < 0 || targetSubrun < 0 || targetEvent < 0)
    {
      mf::LogError("GoToEvent") << "Got invalid event number from Javascript: run = "
                                << targetRun << " targetSubrun = " << targetSubrun << " targetEvent = " << targetEvent
                                << "\nMessage from browser was:\n" << request.body;
      return new web::BadRequestResponse();
    }
    else
    {
      //Response to this request will be send at the beginning of the next analyze() function
      //so that it can report the event number actually loaded.
      navigator->setTarget(targetRun, targetSubrun, targetEvent);
      return new web::EndAppResponse();
    }
  });

  evdApp.add("/", web::Request::Method::GET, [](const std::smatch& matches, const web::Request& request)
  {
    return new web::FileResponse("webDisplay_v2.html", "text/html");
  });

  evdApp.add("/EMPHATICLogo.png", web::Request::Method::GET, [](const std::smatch& matches, const web::Request& request)
  {
    return new web::FileResponse("EMPHATICLogo.png", "image/png");
  });

  evdApp.run();
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
    //TODO: I think we technically need to add dZ from both this magnet segment and the previous one to get the right answer.  Right now, we're just getting lucky that the magnet segments are all the same length.
    const double magnetHalfLength = static_cast<TGeoBBox*>(static_cast<TGeoNode*>(node)->GetVolume()->GetShape())->GetDZ()*2./10.;
    TGeoTranslation magnetMatrix(0, 0, nextMagnetBeginZ + magnetHalfLength);
    double hMatrix[16];
    magnetMatrix.GetHomogenousMatrix(hMatrix);
    geometryList << "  {\n"
                 << "    \"name\": \"magnet\",\n"
                 << "    \"volumeName\": \"" << static_cast<TGeoNode*>(node)->GetVolume()->GetName() << ".obj\",\n"
                 << "    \"isDetector\": false,\n"
                 << "    \"matrix\": [";
    for(int whichElem = 0; whichElem < 15; ++whichElem) geometryList << hMatrix[whichElem] << ", ";
    geometryList << hMatrix[15] << "]\n"
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
        double angleDeg = sensor->Rot()*180./M_PI;
        if(sensor->IsFlip()) angleDeg = -angleDeg;
        TGeoRotation ssdRot("ssdRotation", angleDeg, 0., 0.);
        TGeoTranslation ssdPos(sensor->Pos().X()/10., sensor->Pos().Y()/10., (sensor->Pos().Z() + station->Pos().Z())/10.);
        TGeoCombiTrans ssdMatrix(ssdPos, ssdRot);
        double hMatrix[16];
        ssdMatrix.GetHomogenousMatrix(hMatrix);

        geometryList << "  {\n"
                     << "    \"name\": \"station " << whichStation << " sensor " << whichSensor << " plane " << whichPlane << "\",\n"
                     << "    \"volumeName\": \"ssdsensor_" << whichStation << "_" << whichPlane << "_" << whichSensor << "_vol.obj\",\n"
                     << "    \"isDetector\": true,\n"
                     << "    \"matrix\": [";
        for(int whichElem = 0; whichElem < 15; ++whichElem) geometryList << hMatrix[whichElem] << ", ";
        geometryList << hMatrix[15] << "]\n"
                     << "  },\n";
      }
    }
  }

  //Walk the GDML hierarchy looking for Nodes in fConfig.extraGeometryNodes.
  //Accumulate the world transformation matrix along the way.  This is the
  //only way to get the full transformation matrix because ROOT doesn't
  //provide a way to find a TGeoNode's mother!
  if(!fConfig.extraGeometryNodes().empty())
  {
    const auto nodesAndMatrices = searchGeometryTree(*geom->Geo()->ROOTGeoManager(), fConfig.extraGeometryNodes());

    for(const auto& nodeAndMatrix: nodesAndMatrices)
    {
      const auto node = nodeAndMatrix.first;
      auto matrix = nodeAndMatrix.second;

      std::array<double, 16> hMatrix;
      matrix->GetHomogenousMatrix(hMatrix.data());

      //Transform mm from GDML into cm for event display.  See https://root.cern.ch/doc/master/classTGeoMatrix.html
      hMatrix[12] /= 10.;
      hMatrix[13] /= 10.;
      hMatrix[14] /= 10.;

      std::array<double, 16> transposed = hMatrix;
      for(int col = 0; col < 3; ++col)
      {
        for(int row = 0; row < 3; ++row) transposed[row + col*4] = hMatrix[col + row*4];
      }

      geometryList << "  {\n"
                   << "    \"name\": \"" << node->GetName() << "\",\n"
                   << "    \"volumeName\": \"" << node->GetVolume()->GetName() << ".obj\",\n"
                   << "    \"isDetector\": false,\n"
                   << "    \"matrix\": [";
      for(int whichElem = 0; whichElem < 15; ++whichElem) geometryList << transposed[whichElem] << ", ";
      geometryList << transposed[15] << "]\n"
                   << "  },\n";

      //Clean up memory so this doesn't leak slowly over many events.
      //I think I'm forced to allocated on the heap because I need to use TGeoHMatrix
      //to apply translations in searchGeometryTree(), but I need to return a TGeoMatrix.
      //If I allocated TGeoHMatrix on the stack, putting it into the vector would slice
      //off some member data.
      delete matrix;
      matrix = nullptr;
    }
  }

  //Target
  double hMatrix[16];
  TGeoTranslation targetMatrix(0, 0, (geom->Geo()->TargetUSZPos() + geom->Geo()->TargetDSZPos())/2./10.);
  targetMatrix.GetHomogenousMatrix(hMatrix);
  geometryList << "  {\n"
               << "    \"name\": \"target\",\n"
               << "    \"volumeName\": \"target_vol.obj\",\n"
               << "    \"isDetector\": false,\n"
               << "    \"matrix\": ["; 
  for(int whichElem = 0; whichElem < 15; ++whichElem) geometryList << hMatrix[whichElem] << ", ";
  geometryList << hMatrix[15] << "]\n"
               << "  }\n"; //TODO: Making sure there's not a comma after this last entry is tricky and important!  It will be a bigger problem when I generate the full list of geometry shapes.

  geometryList << "]\n";

  return geometryList.str();
}

//Color and name overrides for objects selected in a different module.
//Use this feature to debug your new reconstruction technique by creating
//a UserHighlight for each object that doesn't work quite right!

//Under the hood, this creates an entry in a nested map for Javascript that looks like:
//{
//  123456789: {
//    0: {
//      color: 0xdeadbeef,
//      name: "someName"
//    },
//    1: {
//      ...
//    },
//  },
//  ...
//  314592653: {
//    ...
//  }
//}

//TLDR: Just call it like the examples and don't try to change it unless you REALLY have to.
template <class PROD>
std::ostream& evd::WebDisplay::writeUserColors(std::ostream& assnsMap, const art::Event& e, const art::InputTag& label) const
{
  art::Handle<art::Assns<PROD, evd::UserHighlight>> assns;
  e.getByLabel(label, assns);
  if(assns && assns->size() > 0)
  {
    assnsMap << "\"" << (*assns)[0].first.id().value() << "\": {\n"; //TODO: Make sure there aren't Assns<> to PRODs from multiple different modules

    auto serializeAssn = [&assns, &assnsMap](const size_t whichAssn) -> std::ostream&
    {
      const auto userData = (*assns)[whichAssn].second;
      assnsMap << "  \"" << (*assns)[whichAssn].first.key() << "\": {\n"
               << "    \"color\": " << userData->Color() << ",\n"
               << "    \"name\": \"" << userData->Comment() << "\"\n"
               << "  }";
      return assnsMap;
    };

    for(size_t whichAssn = 0; whichAssn < assns->size()-1; ++whichAssn)
    {
      serializeAssn(whichAssn) << ",\n";
    }
    serializeAssn(assns->size()-1) << "\n";

    assnsMap << "}\n";
  }

  return assnsMap;
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

//A WebHelper adapts functions like
//  nlohmann::json writeLineSegs(const std::vector<rb::LineSeg>&)
//to work with evd::App<const art::Event&>.  It's a
//web::App<const art::Event&>::handler_t that retrieves information from ART and
//adds ProductID information to your web::Repsonse.
template <class PROD>
class WebHelper
{
  public:
    WebHelper(std::function<nlohmann::json(const std::vector<PROD>&)> func, art::InputTag label): fFunc(func), fLabel(label) {}

    web::Response* operator ()(const std::smatch& matches, const web::Request& req, const art::Event& e)
    {
      art::Handle<std::vector<PROD>> handle;
      e.getByLabel(fLabel, handle);

      if(handle)
      {
        nlohmann::json respBody = fFunc(*handle);

        art::PtrMaker<PROD> makePtr(e, handle.id());
        for(size_t whichProd = 0; whichProd < handle->size(); ++whichProd)
        {
          const auto ptr = makePtr(whichProd);
          respBody[whichProd]["Ptr"]["id"] = ptr.id().value();
          respBody[whichProd]["Ptr"]["key"] = ptr.key();
        }

        return new web::StringResponse(respBody.dump());
      }
      else return new web::BadRequestResponse();
    }

  private:
    std::function<nlohmann::json(const std::vector<PROD>&)> fFunc;
    art::InputTag fLabel;
};

void evd::WebDisplay::analyze(art::Event const& e)
{
  //This event was requested by some POST request.  So respond with the actual event number loaded first.
  {
    nlohmann::json postResponse;
    char timeBuffer[128];
    const long int timestamp = e.time().value();
    const struct tm* calendarTime = localtime(&timestamp); //TODO: Is this a UNIX timestamp, or do I need to take the upper 32 bits?  art::Timestamp has a function for that.
    strftime(timeBuffer, 128, "%c", calendarTime);

    postResponse["run"] = e.id().run();
    postResponse["subrun"] = e.id().subRun();
    postResponse["event"] = e.id().event();
    postResponse["timestamp"] = timeBuffer;
    postResponse["isRealData"] = (e.isRealData()?"Data":"Simulation");

    web::StringResponse newEventResp(postResponse.dump(), "application/json", 201);
    newEventResp.resolve(fBrowserConnection, fBrowserConnection.fNextEventSocket);
  }

  web::App<const art::Event&> evdApp(&fBrowserConnection);
  art::ServiceHandle<emph::EvtDisplayNavigatorService> navigator;

  evdApp.add("/newEvent", web::Request::Method::POST, [&navigator](const std::smatch& matches, const web::Request& request, const art::Event& e) -> web::Response*
  {
    //Parse message body
    nlohmann::json body = nlohmann::json::parse(request.body);
    const int targetRun = body["run"];
    const int targetSubrun = body["subrun"];
    const int targetEvent = body["event"];

    if(targetRun < 0 || targetSubrun < 0 || targetEvent < 0)
    {
      mf::LogError("GoToEvent") << "Got invalid event number from Javascript: run = "
                                << targetRun << " targetSubrun = " << targetSubrun << " targetEvent = " << targetEvent
                                << "\nMessage from browser was:\n" << request.body;
      return new web::BadRequestResponse();
    }
    else
    {
      //Response to this request will be send at the beginning of the next analyze() function
      //so that it can report the event number actually loaded.
      navigator->setTarget(targetRun, targetSubrun, targetEvent);
      return new web::EndAppResponse();
    }
  });

  evdApp.add("/", web::Request::Method::GET, [](const std::smatch& matches, const web::Request& request, const art::Event& e)
  {
    return new web::FileResponse("webDisplay_v2.html", "text/html");
  });

  evdApp.add("/EMPHATICLogo.png", web::Request::Method::GET, [](const std::smatch& matches, const web::Request& request, const art::Event& e)
  {
    return new web::FileResponse("EMPHATICLogo.png", "image/png");
  });

  //Geometry
  evdApp.add("/geometry/index.json", web::Request::Method::GET, [this](const std::smatch& matches, const web::Request& req, const art::Event& e)
  {
    return new web::StringResponse(this->writeGeometryList(), "text/plain"); //TODO: Move this code directly into analyze()
  });

  evdApp.add("/geometry/(.*)\\.obj", web::Request::Method::GET, [this](const std::smatch& matches, const web::Request& req, const art::Event& e)
  {
    std::stringstream geomObjFile;
    assert(matches.size() == 2);
    const TGeoShape* shape = this->getShape(matches[1]);
    TGeoToObjFile(*shape, geomObjFile, 10.);
    return new web::StringResponse(geomObjFile.str(), "text/plain", 200);
  });

  //Simulation
  evdApp.add("/MC/trajs.json", web::Request::Method::GET, WebHelper<sim::Particle>([](const std::vector<sim::Particle>& simParts)
  {
    TDatabasePDG& pdgDB = *TDatabasePDG::Instance();
  
    nlohmann::json trajList;
    for(const auto& traj: simParts)
    {
      std::string partName = std::to_string(traj.fpdgCode);
      const auto partData = pdgDB.GetParticle(traj.fpdgCode);
      if(partData) partName = partData->GetName(); //This should almost always happen

      nlohmann::json entry;
      entry["name"] = std::to_string(traj.ftrajectory.Momentum(0).Vect().Mag()/1000.) + "GeV/c " + partName;
      entry["pdgCode"] = traj.fpdgCode;
      auto& points = entry["points"];
      for(size_t whichTrajPoint = 0; whichTrajPoint < traj.ftrajectory.size()-1; ++whichTrajPoint)
      {
        const auto& point = traj.ftrajectory.Position(whichTrajPoint);
        points.push_back({point.X()/10., point.Y()/10., point.Z()/10.});
      }

      trajList.push_back(entry);
    }

    return trajList;
  }, fConfig.mcPartLabel()));
  
  //Reconstruction
  evdApp.add("/reco/LineSegs.json", web::Request::Method::GET, WebHelper<rb::LineSegment>([](const std::vector<rb::LineSegment>& segs)
  {
    nlohmann::json result;

    for(const auto& seg: segs)
    {
      nlohmann::json entry;
      TVector3 x0 = seg.X0(),
               x1 = seg.X1();
      const auto diff = x1 - x0;
      const double length = diff.Mag()/10.; //Convert mm to cm for graphics reasons
      const double ssdWidth = 0.1;
      const auto center = (x0 + x1)*0.5*0.1; //Convert mm to cm for graphics reasons
      const double phi = diff.Phi();

      entry["center"] = {center.X(), center.Y(), center.Z()};
      entry["length"] = length;
      entry["phi"] = phi;

      result.push_back(entry);
    }

    return result;
  }, fConfig.lineSegLabel()));

  //TODO: Convert writeUserColors to use nlohmann::json.
  //      Can I also make it automatic when a new WebHelper is add()ed?
  evdApp.add("/assnsMap.json", web::Request::Method::GET, [this](const std::smatch& match, const web::Request& req, const art::Event& e){
    std::stringstream assnsMap;
    assnsMap << "{\n";
    writeUserColors<sim::Particle>(assnsMap, e, this->fConfig.mcPartAssnsLabel());
    writeUserColors<rb::LineSegment>(assnsMap, e, this->fConfig.lineSegAssnsLabel());
    assnsMap << "}\n";

    return new web::StringResponse(assnsMap.str());
  });

  evdApp.run(e);
}

void evd::WebDisplay::endJob()
{
}

DEFINE_ART_MODULE(evd::WebDisplay)
