////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to reconstruct tracks
/// \author  $Author: ChristopherWoolford $
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
#include "art/Framework/Core/EDAnalyzer.h"
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

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  /// A class for communication with the viewer via shared memory segment
  ///
  class TrackReconstruction : public art::EDAnalyzer
  {
  public:
    explicit TrackReconstruction(fhicl::ParameterSet const& pset);
    ~TrackReconstruction();
    
    void analyze(const art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginRun(art::Run const&);
    //    void endRun(art::Run const&);
    //    void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    TTree* fSSDTree;
    int fRun;
    int fSubrun;
    int fEvent;
    int fPid;
    std::vector<double> fSSDx;
    std::vector<double> fSSDy;
    std::vector<double> fSSDz;
    std::vector<double> fSSDpx;
    std::vector<double> fSSDpy;
    std::vector<double> fSSDpz;
    
    bool fMakeSSDTree;
    
  };
  
  //.......................................................................
  TrackReconstruction::TrackReconstruction(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {
    fEvent = 0;
    this->reconfigure(pset);

  }

  //......................................................................

  TrackReconstruction::~TrackReconstruction()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void TrackReconstruction::reconfigure(const fhicl::ParameterSet& pset)
  {
    fMakeSSDTree = pset.get<bool>("MakeSSDTree"); 
  }

  //......................................................................

  void TrackReconstruction::beginJob()
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
  //......................................................................

  void TrackReconstruction::beginRun(art::Run const&)
  // Write code to retrieve information about HitGeneration.
  // Retrieve the position and momentum of every particle that hits the ssds.
  // Get other information about the particles...idk maybe mass?
  // SSD hits create current in the SSD to detect particles...maybe exact current generated for hit?
  // Or ssd current over time?
  {







  }
  //......................................................................


    
  void TrackReconstruction::endJob() {     

  }

  //......................................................................

  void TrackReconstruction::analyze(const art::Event& evt)
  { 

    std::string labelStr = "geantgen"; // NOTE, this is probably the wrong label.
    art::Handle< std::vector<std::vector<sim::SSDHit> > > ssdHitH;
    try {
      evt.getByLabel(labelStr,ssdHitH);
    }
    catch(...) {
      std::cout << "WARNING: No SSDHits found!" << std::endl;
    }

    if (fMakeSSDTree && !ssdHitH->empty()) {
	
      // clear/reset variables at the start of each event
      fRun = evt.run();
      fSubrun = evt.subRun();
      fEvent++;
      fPid = 0; // deal with this later

      for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
	// clear/reset vectors for each particle
	fSSDx.clear();
	fSSDy.clear();
	fSSDz.clear();
	fSSDpx.clear();
	fSSDpy.clear();
	fSSDpz.clear();
	const std::vector<sim::SSDHit> ssdv = (*ssdHitH)[idx];
	for (size_t idx2=0; idx2 < ssdv.size(); ++idx2) {
	  fSSDx.push_back(ssdv[idx2].GetX());
	  fSSDy.push_back(ssdv[idx2].GetY());
	  fSSDz.push_back(ssdv[idx2].GetZ());
	  fSSDpx.push_back(ssdv[idx2].GetPx());
	  fSSDpy.push_back(ssdv[idx2].GetPy());
	  fSSDpz.push_back(ssdv[idx2].GetPz());
	}
	fSSDTree->Fill();
      } // end loop over SSD hits for the event
      		
    }

  } // TrackReconstruction::analyze()

}  // end namespace emph

DEFINE_ART_MODULE(emph::TrackReconstruction)
