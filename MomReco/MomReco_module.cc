////////////////////////////////////////////////////////////////////////
/// \brief   Test module for various reconstruction tasks
/// \author  $Author: jmirabit@bu.edu
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TGraph.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "RawData/TRB3RawDigit.h"
#include "SSDAlignment/SSDAlign.h"
#include "RecoBase/SSDCluster.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class MomReco : public art::EDProducer {
  public:
    explicit MomReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MomReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    void Analyze();
    
  private:
    
    emph::cmap::ChannelMap* fChannelMap;
    emph::al::SSDAlign* fAlignmentFile;
	runhist::RunHistory* fRunHistory;
	emph::geo::Geometry *emgeo;
    int         fEvtNum;
    int event;
    std::vector<double> x_shifts, y_shifts, u_shifts, v_shifts;
    //TF1* fit;
    art::ServiceHandle<art::TFileService> tfs;
    
  };

  //.......................................................................
  
  MomReco::MomReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {
    //this->reconfigure(pset);
    fEvtNum = 0;

  }

  //......................................................................
  
  MomReco::~MomReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  // void MomReco::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }
 
  //......................................................................
  
  void MomReco::beginJob()
  {
	
  }

  //......................................................................
  
  void MomReco::beginRun(art::Run& run)
  {
    // initialize channel map
    fChannelMap = new emph::cmap::ChannelMap();
	 fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
	 emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());

    // initialize alignment constants
    fAlignmentFile = new emph::al::SSDAlign();
    fAlignmentFile->LoadShifts(fRunHistory->AlignFile(),x_shifts,y_shifts,u_shifts,v_shifts);
    std::cout<<"*************************************************************"<<std::endl;
    std::cout<<"X shifts: ";
    for (size_t i=0; i<x_shifts.size(); ++i){
        std::cout<<x_shifts[i]<<" ";
    }
    std::cout<<std::endl<<"Y shifts: ";
    for (size_t i=0; i<y_shifts.size(); ++i){
        std::cout<<y_shifts[i]<<" ";
    }
    std::cout<<std::endl<<"U shifts: ";
        for (size_t i=0; i<u_shifts.size(); ++i){
        std::cout<<u_shifts[i]<<" ";
    }
    std::cout<<std::endl<<"V shifts: ";
    for (size_t i=0; i<v_shifts.size(); ++i){
        std::cout<<v_shifts[i]<<" ";
    }
    std::cout<<std::endl;
  }
    
  //......................................................................
  
  void MomReco::endJob()
  {
  }
  
  //......................................................................
  
  void MomReco::Analyze()
   {
    //if (event<100) std::cout<<"event number is: "<<event<<" or " <<fEvtNum<<'\n';

	//Initialize vectors for holding x/y positions for each event and their corresponding z positions
	std::vector<std::vector<double>> xpos = {{},{},{},{},{},{},{},{}};
	std::vector<std::vector<double>> ypos = {{},{},{},{},{},{},{},{}};
	std::vector<std::vector<double>> upos = {{},{},{},{},{},{},{},{}};
	std::vector<std::vector<double>> vpos = {{},{},{},{},{},{},{},{}};

	//Initialize vectors to hold positions from events used for alignment
    std::vector<double> uz_cal={0.45, 120.95, 360.45, 481.45, 988.75, 1214.95};
	std::vector<double> vz_cal={0.45, 120.95, 363.15, 484.15, 986.05, 1212.25};

	//Looping over all SSD hits
		
	//Residual Method of Alignment
	art::ServiceHandle<art::TFileService> tfs;

	//Linear Fit function
	//TF1* fit = new TF1("fit","[0] +[1]*x",0,1250);
    }
  
  //......................................................................
  void MomReco::produce(art::Event& evt)
  { 
	  std::string fClusterLabel = "clust";

	  art::Handle< std::vector<rb::SSDCluster> > clustH;
	  
	  try {
		  evt.getByLabel(fClusterLabel, clustH);
		  if (!clustH->empty()) {	
			  for (size_t idx=0; idx < clustH->size(); ++idx) {
				  const rb::SSDCluster& clust = (*clustH)[idx];
				  const emph::geo::SSDStation &st = emgeo->GetSSDStation(clust.Station());
				  const emph::geo::Detector &sd = st.GetSSD(clust.Sensor());
                  event = evt.event();
				  emph::al::SSDAlign hit(clust, sd, st, event);
                  Analyze();
			  }
			  fEvtNum++;
		  }
	  }
	  catch(...) {

	  }
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::MomReco)
