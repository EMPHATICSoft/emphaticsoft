////////////////////////////////////////////////////////////////////////
/// \file  ExampleAction.cxx
/// \brief Example UserAction w/ Geant4's user "hooks"
///
/// \version $Id: ExampleAction.cxx,v 1.2 2012-09-20 21:47:05 greenc Exp $
/// \author  rhatcher@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "G4Base/ExampleAction.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// self-register with the factory
#include "G4Base/UserActionFactory.h"
USERACTIONREG3(altns,ExampleAction,altns::ExampleAction)

// G4 includes
#include "Geant4/G4Event.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4VUserPrimaryParticleInformation.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VTouchable.hh"

// ROOT includes
#include <TLorentzVector.h>
#include <TString.h>

// C/C++ includes
#include <algorithm>
#include <string>

namespace altns {

  // Initialize static members.

  //-------------------------------------------------------------
  // Constructor.
  ExampleAction::ExampleAction()
    : fSomeValue(0)
    , fVerbose(0)
    , fStepMsgMaxPerEvt(42)
    , fTrack2ndMsgMaxPerEvt(2)
  {
    /// Create the object
  }

  //-------------------------------------------------------------
  // Destructor.
  ExampleAction::~ExampleAction()
  {
    /// Delete anything that we created with "new'.
  }

  //-------------------------------------------------------------
  void ExampleAction::Config(fhicl::ParameterSet const& pset)
  {
    /// Configure the object

    fSomeValue            = pset.get< double >("SomeValue",0)*CLHEP::GeV;
    fVerbose              = pset.get< int    >("Verbose",0);
    fStepMsgMaxPerEvt     = pset.get< int    >("StepMsgMaxPerEvt",42);
    fTrack2ndMsgMaxPerEvt = pset.get< int    >("Track2ndMsgMaxPerEvt",2);

  }

  //-------------------------------------------------------------
  void ExampleAction::PrintConfig(std::string const& /* opt */)
  {
    mf::LogInfo("ExampleAction")
      << "ExampleAction::PrintConfig \n"
      << "    SomeValue            " << fSomeValue            << "\n"
      << "    Verbose              " << fVerbose              << "\n"
      << "    StepMsgMaxPerEvt     " << fStepMsgMaxPerEvt     << "\n"
      << "    Track2ndMsgMaxPerEvt " << fTrack2ndMsgMaxPerEvt << "\n";

  }

  //-------------------------------------------------------------
  void ExampleAction::BeginOfEventAction(const G4Event* event)
  {
    /// This method is invoked before converting the primary particles
    /// to G4Track objects. A typical use of this method would be to 
    /// initialize and/or book histograms for a particular event.

    mf::LogInfo("ExampleAction")
      << "ExampleAction::BeginOfEventAction EventID="
      << event->GetEventID();

    fStepMsg = 0;
    fTrack2ndMsg = 0;

  }

  //-------------------------------------------------------------
  void ExampleAction::EndOfEventAction(const G4Event* event)
  {
    /// This method is invoked at the very end of event processing.
    /// It is typically used for a simple analysis of the processed event.

    mf::LogInfo("ExampleAction")
      << "ExampleAction::EndOfEventAction EventID="
      << event->GetEventID();
  }

  //-------------------------------------------------------------
  void ExampleAction::PreTrackingAction(const G4Track* track)
  {
    /// This method is invoked before any stepping of this track
    /// has occurred

    G4int parent_id = track->GetParentID();
    if ( parent_id > 0 && fTrack2ndMsg > fTrack2ndMsgMaxPerEvt ) return;

    mf::LogInfo("ExampleAction")
      << "ExampleAction::PreTrackingAction TrackID="
      << track->GetTrackID()
      << " is a " << track->GetParticleDefinition()->GetParticleName();
  }

  //-------------------------------------------------------------
  void ExampleAction::PostTrackingAction( const G4Track* track)
  {
    /// This method is invoked after all stepping of this track
    /// has occurred

    G4int parent_id = track->GetParentID();
    std::string extra_msg = "";
    if ( parent_id > 0 ) {
      ++fTrack2ndMsg;
      if ( fTrack2ndMsg  > fTrack2ndMsgMaxPerEvt ) return;
      if ( fTrack2ndMsg == fTrack2ndMsgMaxPerEvt ) {
        extra_msg = "...last such message this event";
      }
    }

    mf::LogInfo("ExampleAction")
      << "ExampleAction::PostTrackingAction TrackID="
      << track->GetTrackID()
      << " " << extra_msg;
  }

  //-------------------------------------------------------------
  void ExampleAction::SteppingAction(const G4Step* step)
  {
    /// This method is invoked at each end of stepping

    ++fStepMsg;
    if ( ++fStepMsg > fStepMsgMaxPerEvt ) return;

    std::string extra_msg = "";
    if ( fStepMsg == fStepMsgMaxPerEvt ) {
      extra_msg = "...last such message this event";
    }

    mf::LogInfo("ExampleAction")
      << "ExampleAction::SteppingAction TrackID="
      << step->GetTrack()->GetTrackID()
      << " " << extra_msg;


  }

  //-------------------------------------------------------------
  G4ClassificationOfNewTrack
  ExampleAction::StackClassifyNewTrack(const G4Track* track)
  {
    /// This method is invoked by G4StackManager whenever a new G4Track
    /// object is "pushed" onto a stack by G4EventManager. ClassifyNewTrack()
    /// returns an enumerator, G4ClassificationOfNewTrack, whose value 
    /// indicates to which stack, if any, the track will be sent. 
    /// G4ClassificationOfNewTrack has four possible values:
    ///    fUrgent - track is placed in the urgent stack
    ///    fWaiting - track is placed in the waiting stack, 
    ///               and will not be simulated until the urgent stack is empty
    ///    fPostpone - track is postponed to the next event
    ///    fKill - the track is deleted immediately and not stored in any stack.

    G4int parent_id = track->GetParentID();
    std::string tsrc = "primary";
    if ( parent_id < 0 ) tsrc = "postponed (from previous event)";
    if ( parent_id > 0 ) tsrc = "secondary";

    mf::LogInfo("ExampleAction")
      << "ExampleAction::StackClassifyNewTrack TrackID="
      << track->GetTrackID()
      << " ParentID=" << parent_id << " "
      << track->GetDefinition()->GetParticleName()
      << " (" << tsrc << " particle)";

    // One *must* return a classification
    // Since we're not doing anything useful in NewStage/PrepareNewEvent
    // the only things we should return are fUrgent or fKill
    return fUrgent;
  }

  //-------------------------------------------------------------
  void ExampleAction::StackNewStage()
  {
    /// This method is invoked when the urgent stack is empty and the 
    /// waiting stack contains at least one G4Track object. Here the user
    /// may kill or re-assign to different stacks all the tracks in the
    /// waiting stack by calling the stackManager->ReClassify() method
    /// which, in turn, calls the ClassifyNewTrack() method. If no user
    /// action is taken, all tracks in the waiting stack are transferred
    /// to the urgent stack. The user may also decide to abort the current
    /// event even though some tracks may remain in the waiting stack by 
    /// calling stackManager->clear(). This method is valid and safe only
    /// if it is called from the G4UserStackingAction class.

    mf::LogInfo("ExampleAction")
      << "ExampleAction::StackNewStage";
  }

  //-------------------------------------------------------------
  void ExampleAction::StackPrepareNewEvent()
  {
    /// This method is invoked at the beginning of each event. At this
    /// point no primary particles have been converted to tracks, so the
    /// urgent and waiting stacks are empty. However, there may be tracks
    /// in the postponed-to-next-event stack; for each of these the 
    /// ClassifyNewTrack() method is called and the track is assigned 
    /// to the appropriate stack.

    mf::LogInfo("ExampleAction")
      << "ExampleAction::StackPrepareNewEvent";
  }

} // end namespace
