////////////////////////////////////////////////////////////////////////
/// \file  FastStopAction.h
/// \brief Use Geant4's user "hooks" to stop and kill a track based on its longitudinal position, 
///        to avoid wasting time propogating showers downstream of the region of interest. (such as the Lead Glass. )
///
/// \author  lebrun@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class implements the nutools/G4Base::UserAction interface in order to
/// kill to stop and kill a track. 
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

namespace sim{
  class FastStop;
}

namespace emph {

  ///list of energy deposits from Geant4
  class FastStopAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    FastStopAction();
    virtual ~FastStopAction();

    void Config(fhicl::ParameterSet const& pset);

    // UserActions method that we'll override, to obtain access to
    // Geant4's particle tracks and trajectories.
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void PreTrackingAction(const G4Track*);
    void PostTrackingAction(const G4Track*);
    void SteppingAction(const G4Step*);
    //    bool ParticleProjection(G4Track*);

  private:

    double fLastZPos;
    int fLastStationNumber; 
    std::string fLastDetectorName;     

    art::ServiceHandle<emph::geo::GeometryService> fGeo; // could be useful, but not for now.. 
    
    //
    // Convenient way to get information for within event debugging.. 
    //
    G4RunManager *fRunManager;
    
  };

} // namespace emph
