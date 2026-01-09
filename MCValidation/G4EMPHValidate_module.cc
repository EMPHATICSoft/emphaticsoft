////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create online monitoring plots
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
#include "TVectorD.h"
#include "TGraph.h"

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
#include "Simulation/Particle.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  /// A class for communication with the viewer via shared memory segment
  ///
  class G4EMPHValidate : public art::EDAnalyzer
  {
  public:
    explicit G4EMPHValidate(fhicl::ParameterSet const& pset);
    ~G4EMPHValidate();
    
    void analyze(const art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    //    void beginRun(art::Run const&);
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
  G4EMPHValidate::G4EMPHValidate(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset),
      fMakeSSDTree (pset.get<bool>("MakeSSDTree"))
  {
    fEvent = 0;
  }

  //......................................................................

  G4EMPHValidate::~G4EMPHValidate()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void G4EMPHValidate::beginJob()
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
    
  void G4EMPHValidate::endJob() {     

  }

  //......................................................................

  void G4EMPHValidate::analyze(const art::Event& evt)
  { 

    std::string labelStr = "geantgen"; // NOTE, this is probably the wrong label.
    art::Handle< std::vector<sim::SSDHit> > ssdHitH;
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
      fSSDx.clear();
      fSSDy.clear();
      fSSDz.clear();
      fSSDpx.clear();
      fSSDpy.clear();
      fSSDpz.clear();
      
      for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
	auto ssdv = (*ssdHitH)[idx];
	fSSDx.push_back(ssdv.X());
	fSSDy.push_back(ssdv.Y());
	fSSDz.push_back(ssdv.Z());
	fSSDpx.push_back(ssdv.Px());
	fSSDpy.push_back(ssdv.Py());
	fSSDpz.push_back(ssdv.Pz());
      }
      fSSDTree->Fill();

      art::Handle< std::vector<sim::Particle> > partH;
      try {
	evt.getByLabel(labelStr,partH);
      }
      catch(...) {
	std::cout << "WARNING: No sim::Particles found!" << std::endl;
      }
      
      art::ServiceHandle<art::TFileService> tfs;

      if (!partH->empty()) {
	
	TVectorD zpos;
	TVectorD xpos;
	TVectorD ypos;
	char grName[64];

	sprintf(grName,"Event_%d",fEvent);
	tfs->mkdir(grName);
	for (size_t idx=0; idx < partH->size(); ++idx) {
	  auto part = (*partH)[idx];
	  auto traj = part.ftrajectory;
	  size_t npoints = traj.size();
	  zpos.ResizeTo(npoints);
	  xpos.ResizeTo(npoints);
	  ypos.ResizeTo(npoints);
	  for (size_t i=0; i<npoints; ++i) {
	    zpos[i] = traj.Z(i);
	    xpos[i] = traj.X(i);
	    ypos[i] = traj.Y(i);
	  }
	  sprintf(grName,"zVsx_%d_%d",fEvent,int(idx));
	  TGraph* zxG = tfs->make<TGraph>(zpos,xpos);
	  zxG->SetName(grName);
	  zxG->Write();
	  sprintf(grName,"zVsy_%d_%d",fEvent,int(idx));
	  TGraph* zyG = tfs->make<TGraph>(zpos,ypos);
	  zyG->SetName(grName);
	} // end loop over SSD hits for the event
      }
    }

  } // G4EMPHValidate::analyze()

}  // end namespace emph

DEFINE_ART_MODULE(emph::G4EMPHValidate)
