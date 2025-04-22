////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create single-particle/event 
///          tracks based on the Kalman algorithm
///       
/// \author  $Author: Jon Paley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// EMPHATICSoft includes
#include "Geometry/service/GeometryService.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/Track.h"
#include "KalmanReco/KalmanAlg.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class SingleTrackKalmanReco : public art::EDProducer {
  public:
    explicit SingleTrackKalmanReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~SingleTrackKalmanReco() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    //    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run&  run);
    void beginJob();
    void endJob();

  private:
    std::string fLineSegmentLabel;
    
    kalman::KalmanAlg* fKAlg;

  };

  //.......................................................................
  
  emph::SingleTrackKalmanReco::SingleTrackKalmanReco(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fLineSegmentLabel      (pset.get< std::string >("LineSegmentLabel"))
    {
      this->produces< std::vector<rb::Track> >();      
      fKAlg = NULL;
    }
  
  //......................................................................
   
  void emph::SingleTrackKalmanReco::beginRun(art::Run& run)
  {
    if (fKAlg) delete fKAlg;
    
    art::ServiceHandle<emph::geo::GeometryService> geo;
    art::ServiceHandle<emph::MagneticFieldService> mag;
    fKAlg = new kalman::KalmanAlg(geo->Geo(),mag->Field());

  }
  //......................................................................
   
  void emph::SingleTrackKalmanReco::beginJob()
  {

    /*
    std::cerr<<"Starting SingleTrackKalmanReco"<<std::endl;

    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");  
    */
  }
 
  //......................................................................
  
  void emph::SingleTrackKalmanReco::endJob()
  {
    /*
       std::cout<<"SingleTrackKalmanReco: Number of events with one cluster per sensor: "<<goodclust<<std::endl;
       std::cout<<"SingleTrackKalmanReco: Number of available clusters: "<<badclust+goodclust<<std::endl;
    */

  }

  //......................................................................

  void emph::SingleTrackKalmanReco::produce(art::Event& evt)
  {

    std::unique_ptr< std::vector<rb::Track> > trackv(new std::vector<rb::Track>);

    
    art::Handle< std::vector<rb::LineSegment> > lsH;

    try {
      evt.getByLabel(fLineSegmentLabel,lsH);
    } 
    catch(...) {
      std::cerr << "WARNING: No rb::LineSegments found!" << std::endl;
    }

    // count the number of line segments per plane, etc.
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();

    std::unordered_map<int,int> lsCount;
    bool isOk = true;
    for (size_t i=0; i<lsH->size(); ++i) {
      auto & ls = (*lsH)[i];
      int id = ls.SSDStation()*10+ls.SSDPlane();
      lsCount[id]++;
      if (lsCount[id] > 1) {
	isOk = false;
	break;
      }
    }

    if (isOk) {
      // no more than 1 line segment per plane, now check that we have at least 2 views per station
      // in general, there are three sections of interest: upstream of the target, between the target and the magnet, and downstream of the target.  Check that we have enough measurements in each section.
      int nPlanesInSection[3][4] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
      for (size_t i=0; i<lsH->size(); ++i) {
	auto & ls = (*lsH)[i];
	auto station = emgeo->GetSSDStation(ls.SSDStation());
	// determine the section
	int section=3;
	if (station->Pos()[2] < emgeo->GetTarget()->Pos()[2])
	  section = 0;
	else if (station->Pos()[2] > emgeo->MagnetDSZPos())
	  section = 2;
	else
	  section = 1;
	auto plane = station->GetPlane(ls.SSDPlane());
	int view = plane->View()-1;
	nPlanesInSection[section][view]++;
      }
      for (int i=0; i<3; ++i) 
	if (! ( (nPlanesInSection[i][0] && (nPlanesInSection[i][1] ||
					    nPlanesInSection[i][2] ||
					    nPlanesInSection[i][3])) ||
		(nPlanesInSection[i][1] && (nPlanesInSection[i][2] ||
					    nPlanesInSection[i][3])) ||
		(nPlanesInSection[i][2] && nPlanesInSection[i][2]) ) )
	  isOk = false;
    }

    if (isOk) {
      // now create vector of linesegments to reconstruct a track.  Will ignore linesegments from SSDs upstream of the target
      std::vector<rb::LineSegment> lsV;
      for (size_t i=0; i<lsH->size(); ++i) {
	auto & ls = (*lsH)[i];
	auto station = emgeo->GetSSDStation(ls.SSDStation());
	// check that measurements is downstream of the target
	if (station->Pos()[2] > emgeo->GetTarget()->Pos()[2])
	  lsV.push_back(ls);
      }

      // now do the fit... fingers crossed!
      rb::Track track = fKAlg->Fit(lsV);
      trackv->push_back(track);
    }
 
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackKalmanReco)
