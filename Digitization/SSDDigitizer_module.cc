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
#include "DetGeoMap/service/DetGeoMapService.h"

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
  /*
  void SSDDigitizer::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    if (fMakeSSDTree) {
      fSSDTree = tfs->make<TTree>("fSSDTree","");
      fSSDTree->Branch("run",&fRun);
      fSSDTree->Branch("subrun",&fSubrun);
      fSSDTree->Branch("event",&fEvent);
      fSSDTree->Branch("pid",&fPid);
      fSSDTree->Branch("ssdx",&fSSDx);
      fSSDTree->Branch("ssdy",&fSSDy);
      fSSDTree->Branch("ssdz",&fSSDz);
      fSSDTree->Branch("ssdpx",&fSSDx);
      fSSDTree->Branch("ssdpy",&fSSDy);
      fSSDTree->Branch("ssdpz",&fSSDz);
    }

  }    
  */
  //......................................................................
  /*    
  void SSDDigitizer::endJob() {     

  }
  */
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
    
    art::ServiceHandle<emph::dgmap::DetGeoMapService> detGeoMap;
    auto dgmap = detGeoMap->Map();

    if (!ssdHitH->empty()) {
	
      for (size_t idx=0; idx < ssdHitH->size(); ++idx) {

	const sim::SSDHit& ssdhit = (*ssdHitH)[idx];
	rawdata::SSDRawDigit* dig;

	dig = dgmap->SSDSimHitToRawDigit(ssdhit);
	if (dig) {
	  ssdRawD->push_back(rawdata::SSDRawDigit(*dig));
	  delete dig;
	}
	
      } // end loop over SSD hits for the event
      		
    }
    
    evt.put(std::move(ssdRawD));

  } // SSDDigitizer::analyze()

}  // end namespace emph

DEFINE_ART_MODULE(emph::SSDDigitizer)
