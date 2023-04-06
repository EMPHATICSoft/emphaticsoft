////////////////////////////////////////////////////////////////////////
/// \file  ExampleAction.h
/// \brief Use Geant4's user "hooks" to kill particles in the rock
///
/// \version $Id: ExampleAction.h,v 1.1 2012-08-27 17:55:03 rhatcher Exp $
/// \author  rhatcher@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class implements the G4Base::UserAction interface in order to
/// decide whether to keep or kill particles

#ifndef ALTNS_EXAMPLEACTION_H
#define ALTNS_EXAMPLEACTION_H

#include "G4Base/UserAction.h"

// Forward declarations.
class G4Event;
class G4Track;
class G4Step;

namespace altns {

  // accumulate a list of particles modeled in G4
  class ExampleAction : public g4b::UserAction {

  public:
    // Standard constructors and destructors;
    ExampleAction();
    virtual ~ExampleAction();

    void Config(fhicl::ParameterSet const& pset);
    void PrintConfig(std::string const& opt);

    /// UserActions method that we'll override, to obtain access to
    /// Geant4's particle tracks and trajectories.
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void PreTrackingAction(const G4Track*);
    void PostTrackingAction(const G4Track*);
    void SteppingAction(const G4Step*);

    /// Does this UserAction do stacking?  
    /// Override to return "true" if the following interfaces are implemented
    bool ProvidesStacking() { return true; } 
    /// G4UserStackingAction interfaces
    G4ClassificationOfNewTrack StackClassifyNewTrack(const G4Track*);
    void StackNewStage();
    void StackPrepareNewEvent();

  private:

    double           fSomeValue;            ///< some user config value
    int              fVerbose;              ///< verbosity
    int              fStepMsgMaxPerEvt;     ///< shut up about steps
    int              fTrack2ndMsgMaxPerEvt; ///< shut up about 2ndary tracks

    int              fStepMsg;      ///< # steps have we printed this evt?
    int              fTrack2ndMsg;  ///< # of 2ndary track printed this evt?

   };

} // namespace altns

#endif // ALTNS_EXAMPLEACTION_h
