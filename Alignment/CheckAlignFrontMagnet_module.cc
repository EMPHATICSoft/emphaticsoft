////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct single-particle tracks
///       
/// \author  $Author: robert chirco $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraph2D.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
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
#include "Align/service/AlignService.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RecoBase/SSDCluster.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/Track.h"
#include "RecoUtils/RecoUtils.h"
#include "Simulation/SSDHit.h"
#include "Simulation/Particle.h"
#include "TrackReco/SingleTrackAlgo.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class CheckAlignFrontMagnet : public art::EDAnalyzer {
  public:
    explicit CheckAlignFrontMagnet(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~CheckAlignFrontMagnet() {};
    
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(const art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

  private:
  
    int         run,subrun,event;
    int         fEvtNum;
    
    size_t      nPlanes;
    size_t      nStations;
    const double zTgt; 

    //fcl parameters

    std::string fTrackSegLabel;
    std::string fToken;

    std::ofstream fOut1; 
  };

  //.......................................................................
  
  emph::CheckAlignFrontMagnet::CheckAlignFrontMagnet(fhicl::ParameterSet const& pset)
    : EDAnalyzer{pset},
    run(0), subrun(0), event(0), fEvtNum(0), nPlanes(0), nStations(0), zTgt(675.), 
    fTrackSegLabel(pset.get< std::string >("TrackSegLabel")),
    fToken(pset.get< std::string >("Token")) 
    {
    }
  
  //......................................................................
  
//  MakeSingleTracks::~MakeSingleTracks()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................

  
void emph::CheckAlignFrontMagnet::beginRun(const art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();
    std::cerr << " CheckAlignFrontMagnet::beginRun, nPlanes " << nPlanes << " nStations " << std::endl;
  }

  //......................................................................
   
  void emph::CheckAlignFrontMagnet::beginJob()
  {
    std::cerr << " CheckAlignFrontMagnet::beginJob, nPlanes " << nPlanes << " nStations " << std::endl;
    std::string fName("./CheckAlignFrontMagnet_"); fName += fToken + std::string("_v1.txt");
    fOut1.open(fName.c_str()); 
    fOut1 << " spill evt nUps nDwn nDwnM x0 y0 slxU slyU x2 y2 slxD slyD xTgtCUps yTgtCUps xTgtCDwn yTgtCDwn" << std::endl;
  }
 
  //......................................................................
  
  void emph::CheckAlignFrontMagnet::endJob()
  {
    std::cerr << " CheckAlignFrontMagnet::endJob, nPlanes " << nPlanes << " nStations " << std::endl;
    fOut1.close();
  }

  //......................................................................

  void emph::CheckAlignFrontMagnet::analyze(const art::Event& evt)
  {

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();
    
//    if (event > 50) return;
    
    bool myDbg = ( event < 20); 
    
    if (myDbg) std::cerr << " emph::CheckAlignFrontMagnet::analyze, at event " << event << " spill " << subrun <<  std::endl;
    
    art::Handle< std::vector<rb::TrackSegment> > tsH;
    evt.getByLabel(fTrackSegLabel, tsH);
    if (myDbg) std::cerr << " ... got TracksSegments, size " << tsH->size() << std::endl;
    
    if (!tsH->empty()){
      std::array<double,3> AUpsPt; std::array<double,3> BUpsPt;
      std::array<double,3> ADwnPt; std::array<double,3> BDwnPt;
      std::array<double,3> ADwnPt2; std::array<double,3> BDwnPt2;
      int nUps = 0; int nDwn = 0; int nDWnM = 0;
      for (auto it = tsH->cbegin(); it != tsH->cend(); ++it) {
//        if (it->B()[2] > 1700. ) { nDWnM++; }  // downstream of the magnet,we do not use this data  
        if (it->RegLabel() == rb::Region::kRegion3) { nDWnM++; }
//        if (it->B()[2] < 300. ) // sloppy, should use the geometry..
        if (it->RegLabel() == rb::Region::kRegion1) { 
	   nUps++;
	   if (nUps == 1) { for (size_t k=0; k !=3; k++) { AUpsPt[k] = it->A()[k]; BUpsPt[k] = it->B()[k];}  }
	   continue;
	}
//        if ((std::abs(it->A()[2] - 505.7417) < 30. ) && (std::abs(it->B()[2] - 846.1500) < 30.)) {
        if ((it->RegLabel() == rb::Region::kRegion2) && (it->B()[2] > 820.) && (it->A()[2] < 515.)){ 
	    nDwn++; 
	   if (nDwn == 1) { for (size_t k=0; k !=3; k++) { ADwnPt[k] = it->A()[k]; BDwnPt[k] = it->B()[k];}  }
	   if (nDwn == 2) { for (size_t k=0; k !=3; k++) { ADwnPt2[k] = it->A()[k]; BDwnPt2[k] = it->B()[k];}  }
	 }
      }
      if (((nUps != 1) || (nDwn > 4)) && myDbg) 
        std::cerr << " ...Too many segments, Upstream " << nUps << " Downstream " << nDwn << " Dwn Magnet " << nDWnM << std::endl;  
      else if (myDbg) {
          std::cerr << " ...a track, Upstream segment  "  
	          <<  " A = (" << AUpsPt[0] << ", " << AUpsPt[1] << ", " << AUpsPt[2] 
		  << " B = (" << BUpsPt[0] << ", " << BUpsPt[1] << ", " << BUpsPt[2] << " ) " << std::endl;
          std::cerr << " ..and... Downstream segment  "  
	          <<  " A = (" << ADwnPt[0] << ", " << ADwnPt[1] << ", " << ADwnPt[2] 
		  << " B = (" << BDwnPt[0] << ", " << BDwnPt[1] << ", " << BDwnPt[2] << " ) " << std::endl;
         std::cerr << " ..and...2nd  Downstream segment  "  
	          <<  " A = (" << ADwnPt2[0] << ", " << ADwnPt2[1] << ", " << ADwnPt2[2] 
		  << " B = (" << BDwnPt2[0] << ", " << BDwnPt2[1] << ", " << BDwnPt2[2] << " ) " << std::endl;
	  	  
	 std::cerr << " ... done for this event.. #... " << event << std::endl;	  
    }
    if ((nUps == 1) && (nDwn != 0)) {
//    fOut1 << " spill evt nUps nDwn nDwnM x0 y0  slxU slyU x2 y2 slxD slyD xTgtCUps yTgtCUps xTgtCDwn yTgtCDwn" << std::endl;
      fOut1 << subrun << " " << event << " " << nUps << " " << nDwn << " " << nDWnM;
      const double slxU = (BUpsPt[0] - AUpsPt[0])/(BUpsPt[2] - AUpsPt[2]);  
      const double slyU = (BUpsPt[1] - AUpsPt[1])/(BUpsPt[2] - AUpsPt[2]);  
      fOut1 << " " << AUpsPt[0] << " " << AUpsPt[1] << " " << slxU << " " << slyU;
      const double slxD = (BDwnPt[0] - ADwnPt[0])/(BDwnPt[2] - ADwnPt[2]);  
      const double slyD = (BDwnPt[1] - ADwnPt[1])/(BDwnPt[2] - ADwnPt[2]);  
      fOut1 << " " << ADwnPt[0] << " " << ADwnPt[1] << " " << slxD << " " << slyD;
      const double xTgtCUps = BUpsPt[0] + slxU*(zTgt - BUpsPt[2]);
      const double yTgtCUps = BUpsPt[1] + slyU*(zTgt - BUpsPt[2]);
      const double xTgtCDwn = BDwnPt[0] + slxD*(zTgt - BDwnPt[2]);
      const double yTgtCDwn = BDwnPt[1] + slyD*(zTgt - BDwnPt[2]);
      fOut1 << " " << xTgtCUps << " " << yTgtCUps << " " << xTgtCDwn << " " << yTgtCDwn << std::endl;
    }  
   }
 }
}
DEFINE_ART_MODULE(emph::CheckAlignFrontMagnet)
