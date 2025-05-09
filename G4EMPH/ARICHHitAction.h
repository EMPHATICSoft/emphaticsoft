////////////////////////////////////////////////////////////////////////
/// \file  ARICHHitAction.h
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
#include "Geometry/service/GeometryService.h"
#include "Geant4/G4Step.hh"
#include "Geant4/G4VPhysicalVolume.hh"

//ART includes
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Geant4/globals.hh"

#include "Simulation/ARICHHit.h"

// Forward declarations.
class G4Event;
class G4Track;
class G4Step;

namespace emph {

  ///list of energy deposits from Geant4
  class ARICHHitAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    ARICHHitAction();
    virtual ~ARICHHitAction();

    void Config(fhicl::ParameterSet const& pset);

    // UserActions method that we'll override, to obtain access to
    // Geant4's particle tracks and trajectories.
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void PreTrackingAction(const G4Track*);
    void PostTrackingAction(const G4Track*);
    void SteppingAction(const G4Step* step);

    //  Returns the current hit being saved in the list of
    //  hits.  
    size_t GetNumARICHHit() { return fARICHHits.size(); }
    sim::ARICHHit GetARICHHit(size_t i) const { return fARICHHits[i]; }
    std::vector <sim::ARICHHit> GetAllHits() const { return fARICHHits; } // !!Deep copy.. 
    // gets all the ARICHHits 

  private:
    int fBlockNum; // the Lead glass block number, if the a Geantino enters the lead glass. 
    std::vector<sim::ARICHHit>  fARICHHits;                 ///< The information for LeadGlass hits.

    art::ServiceHandle<emph::geo::GeometryService> fGeo; // We will use the G4Geometry.. instead.. 
    
    //
    // Convenient way to get information for within event debugging.. 
    //
    G4RunManager *fRunManager; // Can we access the Physical volumes and navigate the geometry.. 
    
    std::ofstream fFOutStudy1;
    //
    // Internal stepping function, for geantinos, vs ASCII tracking, vs high statistics geantinos. 
    //
  };

} // namespace emph
