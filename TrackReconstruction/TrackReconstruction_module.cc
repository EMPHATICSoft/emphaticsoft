////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to reconstruct tracks
/// \author  Author: ChristopherWoolford
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
    //void beginRun(art::Run const&);
    //    void endRun(art::Run const&);
    //    void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    // ROOT Tree to log all info for Track Reconstruction
    TTree* EventInfo;

    // Keeps track of events and runs
    int fRun;
    int fSubrun;
    int fEvent;
    int fPid;

    // Vectors for position and momentum of SSD Hits!
    // Vector of the form: (run, event, ssd hit)
    std::vector<   std::vector< std::vector<double>  >  >  fSSDHit_Position;
    std::vector<   std::vector< std::vector<double>  >  >  fSSDHit_Momentum;

    // Position Vector for an individual hit
    std::vector<double> hitPosition;

    // Vectors for the form: (run, event, hit mass / type)
    std::vector<  std::vector<  double  >   >  fParticleMass;
    std::vector<  std::vector<  std::string   >   >  fParticleType;

    // vector of ssd event
    std::vector<sim::SSDHit> ssd_event;

    // vector of ssd hit positions for each event
    std::vector<double> ssd_event_position;
    // vector of particle masses for each event
    std::vector<double> ssd_event_masses;
    // vector of particle types for each event
    std::vector<std::string> ssd_event_particleTypes;


    // Use if statements to control what info you're logging!
    bool GetSSD_HitInfo;
    bool IsGEANT4;    
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
    GetSSD_HitInfo = pset.get<bool>("GetSSD_HitInfo");
    IsGEANT4 = pset.get<bool>("IsGEANT4");
  }

  //......................................................................

  void TrackReconstruction::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    EventInfo = tfs->make<TTree>("EventInfo","");

    // If specified in fcl file, then log particle position and momentum for ssd hits!
    if (GetSSD_HitInfo) {
      EventInfo->Branch("run",&fRun);
      EventInfo->Branch("subrun",&fSubrun);
      EventInfo->Branch("event",&fEvent);
      EventInfo->Branch("pid",&fPid);
      EventInfo->Branch("ssd_momentum",&fSSDHit_Momentum);
      EventInfo->Branch("ssd_position",&fSSDHit_Position);
    }

    if (IsGEANT4) {
      // Get the real mass and type of each logged particle for

      EventInfo->Branch("ParticleMass", &fParticleMass);
      EventInfo->Branch("ParticleType", &fParticleType);
    }

  }    
  //......................................................................
    
  void TrackReconstruction::endJob() {     

  }

  //......................................................................

  void TrackReconstruction::analyze(const art::Event& evt)
  { 
    std::string labelStr = "geantgen"; // NOTE, this is probably the wrong label.
      // pull info on SSD hits from art
    art::Handle< std::vector<std::vector<sim::SSDHit> > > ssdHitH;
    try {
      evt.getByLabel(labelStr,ssdHitH);
        }
    catch(...) {
        std::cout << "WARNING: No SSDHits found!" << std::endl;
               }

    if (!ssdHitH->empty()){

      // clear/reset variables at the start of each event
      fRun = evt.run();
      fSubrun = evt.subRun();
      fEvent++;
      fPid = 0;


      // loop over every event!
      for (size_t event=0; event < ssdHitH->size(); ++event){
       
        // vector of a ssd event
        ssd_event = (*ssdHitH)[event];

        // loop over every ssdhit!
	      for (size_t hit=0; hit < ssd_event.size(); ++hit) {
          
          // vector containing the position of an individual ssd hit.
          // first clear the old stuff out of the vector
          hitPosition.clear();
          hitPosition = { ssd_event[hit].GetX(), ssd_event[hit].GetY(), ssd_event[hit].GetZ() };

          // Add position vector to a vector of events
          ssd_event_position.push_back(hitPosition);

          // If this is a GEANT4 simulations 
          if (IsGEANT4) {
          // Add code to retrieve info about the type of particle hit (electron? or something else?) and the particles mass.
            ssd_event_masses.push_back( ssd_event[hit].GetMass() );
            ssd_event_particleTypes.push_back( ssd_event[hit].GetParticleType() );
          }

        } // end of event loop
        // Add ssd event positions to vector.
        fSDDHit_Position[event].push_back(*ssd_event_position);
        // Add ssd event masses and particle type to vector.
        fParticleMass[event].push_back(*ssd_event_masses);
        fParticleType[event].push_back(*ssd_event_particleTypes);
      } // end of run loop
      // fill the root tree
      EventInfo->Fill();
    } // end of if statement
  } // TrackReconstruction::analyze()
}  // end namespace emph

DEFINE_ART_MODULE(emph::TrackReconstruction)