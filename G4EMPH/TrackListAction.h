////////////////////////////////////////////////////////////////////////
/// \file  TrackiListAction.h
/// \brief Use Geant4's user "hooks" to generate our list of Geant4 track that are propagated. 
///
/// \author  lebrun@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class implements the nutools/G4Base::UserAction interface in order to
/// accumulate a list of tracks, with pId, the track id, and the parent Id 
//
#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// G4 
#include "G4TrackingManager.hh"
#include "G4RunManager.hh"

// G4EMPH includes
#include "G4Base/UserAction.h"
#include "Geometry/service/GeometryService.h"

//ART includes
// #include "art/Framework/Services/Registry/ServiceHandle.h"

#include "Geant4/globals.hh"
//
// Our sim includes 
//
#include "Simulation/Track.h"

// Forward declarations.
class G4Event;
class G4Track;
class G4EnergyLossForExtrapolator;


namespace emph {

  ///list of energy deposits from Geant4
  class TrackListAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    TrackListAction();
    virtual ~TrackListAction();

    void Config(fhicl::ParameterSet const& pset);
    void SetEnergyThreshold(double e); 
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
    // One could (should !) question  
    sim::Track GetTrack(size_t i) { return fTracks[i]; } // deep copy, 
    // gets specific track.
    std::vector <sim::Track> GetAllTracks() { return fTracks; }
    // gets all the tracks
    
  private:

  private:
    G4double                     fEnergyCut;      ///< The minimum energy in GeV for a particle to       
    G4double                fEnergyCutStore;        ///< The minimum energy for a particle to be included in the list for keeping, and dumping on ASCII file
    std::vector<sim::Track> fTracks;                 ///< The information for Track.  Abbreviated G4Track (no volume info) 
    ///< be included in the list.                          

    //
    // Convenient way to get information for within event debugging.. 
    //
    G4RunManager *fRunManager;
    
    std::ofstream fFOutStudy1;

  };

} // namespace g4n
