////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create calibration constants for SSD
///          alignment
/// \author  $Author: linyan $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

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
#include "RecoBase/SSDHit.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class SSDCalibration : public art::EDProducer {
  public:
    explicit SSDCalibration(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~SSDCalibration();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    void Fit();
    
  private:
    
    emph::cmap::ChannelMap* fChannelMap;
	 runhist::RunHistory* fRunHistory;
	 emph::geo::Geometry *emgeo;
    int         fEvtNum;
	 std::vector<rb::SSDHit> ssdvec;

  };

  //.......................................................................
  
  SSDCalibration::SSDCalibration(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

//    this->produces<std::vector<rb::ARing>>();

    //this->reconfigure(pset);
    fEvtNum = 0;

  }

  //......................................................................
  
  SSDCalibration::~SSDCalibration()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  // void SSDCalibration::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }

  //......................................................................
  
  void SSDCalibration::beginRun(art::Run& run)
  {
    // initialize channel map
    fChannelMap = new emph::cmap::ChannelMap();
	 fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
	 emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
  }
    
  //......................................................................
  
  void SSDCalibration::endJob()
  {
	  Fit();
	  //Then output the alignment constants to ConstBase/SSDCalibration.dat
  }
  
  //......................................................................
  
  void SSDCalibration::Fit()
  {
   //Fit the ssdvectors and obtain x, y shifts
  }
  
  //......................................................................
  void SSDCalibration::produce(art::Event& evt)
  { 
	  std::string labelstr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::SSD));
	  emph::cmap::FEBoardType boardType = emph::cmap::SSD;
	  emph::cmap::EChannel echan;
	  echan.SetBoardType(boardType);

	  art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdH;
	  try {
		  evt.getByLabel(labelstr, ssdH);
		  if (!ssdH->empty()) {	
			  for (size_t idx=0; idx < ssdH->size(); ++idx) {
				  const rawdata::SSDRawDigit& ssd = (*ssdH)[idx];
				  echan.SetBoard(ssd.FER());
				  echan.SetChannel(ssd.Module());
				  emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
				  const emph::geo::SSDStation &st = emgeo->GetSSDStation(dchan.Station());
				  const emph::geo::Detector &sd = st.GetSSD(dchan.Channel());
				  rb::SSDHit hit(ssd, sd);
				  ssdvec.push_back(hit);
			  }
			  fEvtNum++;
		  }
	  }
	  catch(...) {

	  }

  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::SSDCalibration)