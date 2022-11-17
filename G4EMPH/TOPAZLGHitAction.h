////////////////////////////////////////////////////////////////////////
/// \file  TOPAZLGHitAction.h
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
#include "Geant4/G4Step.hh"
#include "Geant4/G4VPhysicalVolume.hh"

//ART includes
#include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Geant4/globals.hh"

#include "Simulation/TOPAZLGHit.h"

// Forward declarations.
class G4Event;
class G4Track;
class G4Step;
class G4EnergyLossForExtrapolator;

namespace emph {

  ///list of energy deposits from Geant4
  class TOPAZLGHitAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    TOPAZLGHitAction();
    virtual ~TOPAZLGHitAction();

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
    size_t GetNumTOPAZLGHit() { return fTOPAZLGHits.size(); }
    sim::TOPAZLGHit GetTOPAZLGHit(size_t i) const { return fTOPAZLGHits[i]; }
    std::vector <sim::TOPAZLGHit> GetAllHits() const { return fTOPAZLGHits; } // !!Deep copy.. 
    // gets all the TOPAZLGHits 

  private:
    int fPrevBlockNum;
    double fRefractionIndex; // The refraction index of the glass 
    double fCritAngleTotalInt; 
    double fNumPhotPerMm;
    double fKnobAtt; // a know to tune the effective absorption in the glass.. 
    double fZGlassUpstreamFace;  // To compute the distance into the gass, to estimate the and verify the shower max. Event dependant.. 
    double fGlassBlockLength; // could should get it from geometry. 
    int fEvtNum; // The event number. 
    int fBlockNum; // the Lead glass block number, if the a Geantino enters the lead glass. 
    int fBlockNumExit; // the Lead glass block number, if the a Geantino enters the lead glass. 
    double fXStart, fYStart, fXMagnet, fYMagnet, fXLGUpstr, fXLGDownstr;
    double fYLGUpstr, fYLGDownstr, fZLGUpstr, fZLGDownstr;
    std::vector<sim::TOPAZLGHit>  fTOPAZLGHits;                 ///< The information for LeadGlass hits.
//    G4double                     fEnergyCut;      ///< The minimum energy in GeV for a particle to produce Cerenkov photons.. 
// not a fixed quantity.. Depends on the Cerenkov angle.        
    ///< be included in the list.                          

    art::ServiceHandle<emph::geo::GeometryService> fGeo; // We will use the G4Geometry.. instead.. 
    
    //
    // Convenient way to get information for within event debugging.. 
    //
    G4RunManager *fRunManager; // Can we access the Physical volumes and navigate the geometry.. 
    
    std::ofstream fFOutStudy1;
    std::ofstream fGeantinoStudy1;
    //
    // Internal stepping function, for geantinos, vs ASCII tracking, vs high statistics geantinos. 
    //
    void SteppingActionGeantinoASCIIDump(const G4Step*);
    void SteppingActionGeantinoStat(const G4Step*);
    void SteppingActionEMShowers(const G4Step*);
    int findBlockNumberFromName (const G4VPhysicalVolume *pVol);
  };

} // namespace emph
