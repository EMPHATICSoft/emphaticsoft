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
#include "ChannelMap/service/ChannelMapService.h"
//#include "RunHistory/service/RunHistoryService.h"
#include "Geometry/DetectorDefs.h"
//#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
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
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    //    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    void Fit();
    
  private:
    
    //    emph::cmap::ChannelMap* fChannelMap;
    //	 runhist::RunHistory* fRunHistory;
    //	 emph::geo::Geometry *emgeo;
    int         fEvtNum;
    std::vector<rb::SSDHit> ssdvec;
    
  };

  //.......................................................................
  
  SSDCalibration::SSDCalibration(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

    //    this->produces<std::vector<rb::ARing>>();

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
  
  //  void SSDCalibration::beginRun(art::Run& run)
  //  {
  // initialize channel map
  //    fChannelMap = new emph::cmap::ChannelMap();
  //    fRunHistory = new runhist::RunHistory(run.run());
  //    fChannelMap->LoadMap(fRunHistory->ChanFile());
  //    emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
  //  }
    
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
      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
      art::ServiceHandle<emph::geo::GeometryService> geo;
      auto emgeo = geo->Geo();
      if (!ssdH->empty()) {	
	for (size_t idx=0; idx < ssdH->size(); ++idx) {
	  const rawdata::SSDRawDigit& ssd = (*ssdH)[idx];
	  echan.SetBoard(ssd.FER());
	  echan.SetChannel(ssd.Module());
	  emph::cmap::DChannel dchan = cmap->DetChan(echan);
	  const emph::geo::SSDStation &st = emgeo->GetSSDStation(dchan.Station());
	  const emph::geo::Plane &pln = st.GetPlane(dchan.Plane());
	  const emph::geo::Detector &sd = pln.SSD(dchan.HiLo());
	  rb::SSDHit hit(ssd, sd);
	  ssdvec.push_back(hit);
	  double x = (ssd.Row()*hit.Pitch()-sd.Height()/2)*sin(sd.Rot())+sd.Pos()[0];
	  double y = (ssd.Row()*hit.Pitch()-sd.Height()/2)*cos(sd.Rot())+sd.Pos()[1];
	  double z = st.Pos()[2] + sd.Pos()[2];
	  if ( fEvtNum < 100 ) std::cout << x << " " << y << " " << z << std::endl;
	}
	fEvtNum++;
      }
    }
    catch(...) {

    }

  }

} // end namespace emph

DEFINE_ART_MODULE(emph::SSDCalibration)
