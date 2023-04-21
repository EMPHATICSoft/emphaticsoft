////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to convert Geant hits in SSDs to raw digits
/// \author  $Author: jpaley $
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
#include "TTree.h"

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
#include "Geometry/DetectorDefs.h"
#include "Simulation/SSDHit.h"
#include "RawData/SSDRawDigit.h"
//#include "DetGeoMap/service/DetGeoMapService.h"
#include "ChannelMap/service/ChannelMapService.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  /// A class for communication with the viewer via shared memory segment
  ///
  class SSDDigitizer : public art::EDProducer
  {
  public:
    explicit SSDDigitizer(fhicl::ParameterSet const& pset);
    ~SSDDigitizer();
    
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    //    void beginJob();
    //    void beginRun(art::Run const&);
    //    void endRun(art::Run const&);
    //    void endSubRun(art::SubRun const&);
    //    void endJob();
    
  private:
    std::string fG4Label;

    std::unordered_map<int,int> fSensorMap;

    void FillSensorMap();

    //    TTree* fSSDTree;
    /*
    int fRun;
    int fSubrun;
    int fEvent;
    int fPid;
    */

    //    std::vector<double> fSSDx;
    //    std::vector<double> fSSDy;
    //    std::vector<double> fSSDz;
    //    std::vector<double> fSSDpx;
    //    std::vector<double> fSSDpy;
    //    std::vector<double> fSSDpz;
    
    //    bool fMakeSSDTree;
    
  };
  
  //.......................................................................
  SSDDigitizer::SSDDigitizer(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {
    //    fEvent = 0;
    this->reconfigure(pset);

    fSensorMap.clear();

    produces<std::vector<rawdata::SSDRawDigit> >();

  }

  //......................................................................

  SSDDigitizer::~SSDDigitizer()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void SSDDigitizer::reconfigure(const fhicl::ParameterSet& pset)
  {
    fG4Label = pset.get<std::string>("G4Label");

    //    fMakeSSDTree = pset.get<bool>("MakeSSDTree"); 
  }

  //......................................................................
  
  void SSDDigitizer::FillSensorMap()
  {
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

    auto echanMap = cmap->EMap();
    for (auto it=echanMap.begin(); it != echanMap.end(); ++it) {
      auto dchan = it->second;
      if (dchan.DetId() == emph::geo::SSD) {
	fSensorMap[dchan.HiLo()] = dchan.Channel();
	//	std::cout << dchan.HiLo() << "," << dchan.Channel() << std::endl;
      }
    }
  }
  
  //......................................................................

  void SSDDigitizer::produce(art::Event& evt)
  { 

    art::Handle< std::vector<sim::SSDHit> > ssdHitH;
    try {
      evt.getByLabel(fG4Label,ssdHitH);
    }
    catch(...) {
      std::cout << "WARNING: No SSDHits found!" << std::endl;
    }
    
    std::unique_ptr<std::vector<rawdata::SSDRawDigit> >ssdRawD(new std::vector<rawdata::SSDRawDigit>);
    
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

    if (!ssdHitH->empty()) {

      int station, chip, set, strip, t, adc, trig;
      int sensor;
      t = 0;
      trig = 0;

      emph::cmap::EChannel echan;
      emph::cmap::DChannel dchan;
      echan.SetBoardType(emph::cmap::SSD);
      dchan.SetDetId(emph::geo::SSD);
      dchan.SetChannel(-1);

      if (fSensorMap.empty()) FillSensorMap();

      for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
	
	const sim::SSDHit& ssdhit = (*ssdHitH)[idx];
	rawdata::SSDRawDigit* dig=0;
	
	station = ssdhit.GetStation();
	sensor = ssdhit.GetSensor(); // need to convert this from 0-27 range to 0-5
	strip = ssdhit.GetStrip();
	chip = strip/128;
	set = strip/128; // not sure what "set" is... do we even use this for anything?  Is it redundant?
	adc = ssdhit.GetDE()/8; // this needs attention!

	dchan.SetStation(station);
	dchan.SetHiLo(sensor);
	dchan.SetChannel(fSensorMap[sensor]);
	echan = cmap->ElectChan(dchan);
	dig = new rawdata::SSDRawDigit(echan.Board(), echan.Channel(), chip, set,
				       strip, t, adc, trig);
	//	std::cout << "(" << station << "," << sensor << "," << fSensorMap[sensor] << ") --> (" << echan.Board() << "," << echan.Channel() << ")" << std::endl;
	ssdRawD->push_back(rawdata::SSDRawDigit(*dig));
	delete dig;
	
      } // end loop over SSD hits for the event
      
    }
    
    evt.put(std::move(ssdRawD));

  } // SSDDigitizer::analyze()

}  // end namespace emph

DEFINE_ART_MODULE(emph::SSDDigitizer)
