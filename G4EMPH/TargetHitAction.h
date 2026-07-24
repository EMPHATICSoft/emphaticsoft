////////////////////////////////////////////////////////////////////////
/// \file  TargetHitAction.h
/// \brief Use Geant4's user "hooks" to generate our simulated response of the Target Detectors. (Target) 
///
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class implements the nutools/G4Base::UserAction interface in order to
/// accumulate a list of target hits modeled by Geant4.
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
#include "Geometry/service/GeometryService.h"

//ART includes
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Geant4/globals.hh"
#include "Simulation/TargetHit.h"
// Forward declarations.
class G4Event;
class G4Track;
class G4Step;
class G4EnergyLossForExtrapolator;

namespace emph {

  ///list of energy deposits from Geant4
  class TargetHitAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    TargetHitAction();
    virtual ~TargetHitAction();

    void Config(fhicl::ParameterSet const& pset);

    // UserActions method that we'll override, to obtain access to
    // Geant4's particle tracks and trajectories.
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void PreTrackingAction(const G4Track*);
    void PostTrackingAction(const G4Track*);
    void SteppingAction(const G4Step*);
    //    bool ParticleProjection(G4Track*);

    //  Returns the current hit being saved in the list of hits.  
    sim::TargetHit GetTargetHit(size_t i) const { return fTargetHits[i]; }
    std::vector <sim::TargetHit> GetAllHits() const { return fTargetHits; } ///< gets all the targethits

  private:
    std::vector<sim::TargetHit> fTargetHits;       ///< The information for Target hits.
    G4double                  fEnergyCut;     ///< The minimum energy in GeV for a particle to
                                              ///< be included in the list.                          
    bool                      fIsParticleInsideTargetBigBox; ///< Is the particle inside the Big Box?

    art::ServiceHandle<emph::geo::GeometryService> fGeo;
    
    // Convenient way to get information for within event debugging.. 
    G4RunManager *fRunManager;
  };

} // namespace emph
