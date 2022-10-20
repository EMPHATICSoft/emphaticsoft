////////////////////////////////////////////////////////////////////////
/// \file  SSDHitAction.h
/// \brief Use Geant4's user "hooks" to generate our simulated response of the Silicon Strip Detectors. (SSD) 
///
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class implements the nutools/G4Base::UserAction interface in order to
/// accumulate a list of ssd hits modeled by Geant4.
//
#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// G4 
#include "G4SteppingManager.hh"
#include "G4RunManager.hh"

// G4EMPH includes
#include "G4Base/UserAction.h"
#include "Geometry/GeometryService.h"

//ART includes
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Geant4/globals.hh"

// Forward declarations.
class G4Event;
class G4Track;
class G4Step;
class G4EnergyLossForExtrapolator;

namespace sim{
  class SSDHit;
}

namespace emph {

  ///list of energy deposits from Geant4
  class SSDHitAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    SSDHitAction();
    virtual ~SSDHitAction();

    void Config(fhicl::ParameterSet const& pset);

    // UserActions method that we'll override, to obtain access to
    // Geant4's particle tracks and trajectories.
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void PreTrackingAction(const G4Track*);
    void PostTrackingAction(const G4Track*);
    void SteppingAction(const G4Step*);
    //    bool ParticleProjection(G4Track*);

    //  Returns the current hit being saved in the list of
    //  hits.  
    std::vector<sim::SSDHit> GetSSDHits(size_t i) { return fSSDHits[i]; }
    // gets specific ssdhit.
    std::vector<std::vector <sim::SSDHit> > GetAllHits() { return fSSDHits; }
    // gets all the ssdhits

  private:

  private:
    std::vector<std::vector<sim::SSDHit> > fSSDHits;                 ///< The information for SSD hits.
    G4double                     fEnergyCut;      ///< The minimum energy in GeV for a particle to       
    ///< be included in the list.                          
    bool                         fIsParticleInsideDetectorBigBox;///< Is the particle inside the Big Box?

    art::ServiceHandle<emph::geo::GeometryService> fGeo;
    
    //
    // Convenient way to get information for within event debugging.. 
    //
    G4RunManager *fRunManager;
    
    std::ofstream fFOutStudy1;

  };

} // namespace g4n
