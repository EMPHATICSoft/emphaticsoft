////////////////////////////////////////////////////////////////////////
/// \file  UserAction.h
/// \brief see below
///
/// \version $Id: UserAction.h,v 1.4 2012-09-20 21:47:05 greenc Exp $
/// \author  seligman@nevis.columbia.edu, brebel@fnal.gov, rhatcher@fnal.gov
////////////////////////////////////////////////////////////////////////
/// G4Base::UserAction.h
/// 1-Sep-1999 Bill Seligman
///
/// 27-Jan-2009 <seligman@nevis.columbia.edu> Revised for LArSoft.
///
/// 2012-08-17 <rhatcher@fnal.gov> Add G4UserStackingAction-like interfaces
///
/// This is an abstract base class to be used with Geant 4.0.1 (and
/// possibly higher, if the User classes don't change).  
///
/// Why is this interface useful?  Answer: Geant4 provides several
/// classes that are meant to be "user hooks" in G4 processing.  A
/// couple of examples are G4UserRunAction and G4EventAction.  The user
/// is meant to publically inherit from these classes in order to
/// perform tasks at the beginning and ending of run or event
/// processing.
///
/// However, typical tasks that physicists perform generally involve
/// more than one user-hook class.  For example, to make histograms, a
/// physicist might define the histograms at the beginning of a run,
/// fill the histograms after each event, and write the histograms at
/// the end of a run.
///
/// It's handy to have all the code for such tasks (making histograms,
/// studying G4 tracking, event persistency) all in one class, rather
/// than split between two or three different classes.  That's where
/// UserAction comes in.  It gathers all the G4 user-hook or
/// user-action classes into one place.

#ifndef G4BASE_UserAction_H
#define G4BASE_UserAction_H

// The following objects are the arguments to the methods
// invoked in the user action classes.  In other words, they
// contain the variables that we are normally able to record
// in Geant.

class G4Run;
class G4Event;
class G4Track;
class G4Step;
#include "Geant4/G4ClassificationOfNewTrack.hh"

#include <string>
#include "fhiclcpp/ParameterSet.h"

namespace g4b {

  class UserAction {
   
  public:

    UserAction() {};
    UserAction(fhicl::ParameterSet const& pset) { Config(pset); }
    virtual ~UserAction() {};

    /// Override Config() to extract any necessary parameters
    virtual void Config(fhicl::ParameterSet const& /* pset */ ) {};

    /// Override PrintConfig() to print out current configuration
    virtual void PrintConfig(std::string const& /* opt */ ) {};

    /// The following a list of methods that correspond to the available 
    /// user action classes in Geant 4.0.1 and higher.

    /// G4UserRunAction interfaces
    virtual void BeginOfRunAction  (const G4Run*  ) {};
    virtual void EndOfRunAction    (const G4Run*  ) {};

    /// G4UserEventAction interfaces
    virtual void BeginOfEventAction(const G4Event*) {};
    virtual void EndOfEventAction  (const G4Event*) {};

    /// G4UserTrackingAction interfaces
    virtual void PreTrackingAction (const G4Track*) {};
    virtual void PostTrackingAction(const G4Track*) {};

    /// G4UserSteppingAction interface
    virtual void SteppingAction    (const G4Step* ) {};

    /// Does this UserAction do stacking?  
    /// Override to return "true" if the following interfaces are implemented
    virtual bool ProvidesStacking() { return false; } 
    /// G4UserStackingAction interfaces
    virtual G4ClassificationOfNewTrack 
      StackClassifyNewTrack(const G4Track*) { return fUrgent; }
    virtual void StackNewStage() {};
    virtual void StackPrepareNewEvent() {};

    // allow self-identification
    std::string const & GetName() const { return myName; }
    void                SetName(std::string const& name) { myName = name; }
  private:
    std::string myName;  ///< self-knowledge
  };

} // namespace g4b

#endif // G4BASE_UserAction_H
