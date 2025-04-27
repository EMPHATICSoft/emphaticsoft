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
    fKAlg->SetVerbosity(100);
    std::cout << "Created new instance of KalmanAlg!" << std::endl;
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
    art::Handle< std::vector<rb::TrackSegment> > trkSegH;

    try {
      evt.getByLabel(fLineSegmentLabel,lsH);
    } 
    catch(...) {
      std::cerr << "WARNING: No rb::LineSegments found!" << std::endl;
    }

    std::cout << "Found " << (int)lsH->size() << " linesegments" << std::endl;

    try {
      evt.getByLabel(fLineSegmentLabel,trkSegH);
    }
    catch(...) {
      std::cerr << "WARNING: No rb::TrackSegments found, will default to initial state with large uncertainties." << std::endl;
    }

    // count the number of line segments per plane, etc.
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();

    std::unordered_map<int,int> lsCount;
    lsCount.clear();
    bool isOk = true;
    for (size_t i=0; i<lsH->size(); ++i) {
      const rb::LineSegment& ls = (*lsH)[i];
      int id = ls.SSDStation()*10 + ls.SSDPlane();
      lsCount[id]++;
      if (lsCount[id] > 1) {
	isOk = false;
      }
    }
    
    /*    for (auto i=lsCount.begin(); i != lsCount.end(); ++i) {
	  std::cout << "N[" << i->first/10 << "][" << i->first%10 << "] = " << i->second << std::endl;
	  }
    */
    
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

      // require exactly 2 (x,y) measurements upstream of the target
      if (nPlanesInSection[0][0] != 2 ||
	  nPlanesInSection[0][1] != 2)
	isOk = false;
      // require at least 2 (x,y) measurements in the mid-section
      if (nPlanesInSection[1][0] < 2 &&
	  nPlanesInSection[1][1] < 2)
	isOk = false;
      // require at least 2 (x,y) measurements downstream of the magnet
      if (nPlanesInSection[2][0] < 2 &&
	  nPlanesInSection[2][1] < 2)
	isOk = false;
      
      if (isOk) {
	// now create vector of linesegments to reconstruct a track.  Will ignore linesegments from SSDs upstream of the target
	std::vector<rb::LineSegment> lsV;
	for (size_t i=0; i<lsH->size(); ++i) {
	  auto & ls = (*lsH)[i];
	  auto station = emgeo->GetSSDStation(ls.SSDStation());
	  // check that measurements is downstream of the target
	  if (station->Pos()[2] > emgeo->GetTarget()->Pos()[2] &&
	      station->Pos()[2] < emgeo->MagnetUSZPos()) {
	    //	  std::cout << ls << std::endl;
	    lsV.push_back(ls);
	  }
	}

	KPar par;
	K5x5 parCov;
	par[0] = 0.1;
	par[1] = 0.1;
	par[2] = 0.01;
	par[3] = 0.01;
	par[4] = 0.2; // 5 GeV/c
	parCov[0][0] = 16.;
	parCov[1][1] = 16.;
	parCov[2][2] = 0.09;
	parCov[3][3] = 0.09;
	parCov[4][4] = 0.0001;	
	art::ServiceHandle<emph::geo::GeometryService> geo;

	double startz = geo->Geo()->TargetDSZPos();
	
	std::cout << "Found " << trkSegH->size() << " tracksegments" << std::endl;

	if (trkSegH->size() > 1) {
	  for (size_t i=0; i<trkSegH->size(); ++i) {
	    auto & ts = (*trkSegH)[i];
	    if (ts.A()[2] > emgeo->GetTarget()->Pos()[2] &&
		ts.A()[2] < emgeo->MagnetUSZPos()) {
	      if (ts.NSpacePoints() == 3) {
		std::cout << ts << std::endl;
		double dx = ts.GetSpacePoint(0)->Pos()[0] - ts.GetSpacePoint(2)->Pos()[0];
		double dy = ts.GetSpacePoint(0)->Pos()[1] - ts.GetSpacePoint(2)->Pos()[1];
		double dz = ts.GetSpacePoint(0)->Pos()[2] - ts.GetSpacePoint(2)->Pos()[2];		
		par[2] = dx/dz;
		par[3] = dy/dz;
		const double* tP = ts.GetSpacePoint(0)->Pos();
		dz = tP[2] - startz;
		par[0] = tP[0] - dz*par[2];
		par[1] = tP[1] - dz*par[3];
		parCov[0][0] = 0.5;
		parCov[1][1] = 0.5;
		parCov[2][2] = 0.01;
		parCov[3][3] = 0.01;
	      }
	    }
	  }
	}
	kalman::State state1(startz,par,parCov);
	
	//	now do the fit... fingers crossed!
	rb::Track track = fKAlg->Fit(lsV, state1);
	//      trackv->push_back(track);
      }
    }

    evt.put(std::move(trackv));
  }
  
} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackKalmanReco)
