/// LArG4::UserActionManager.h
/// 18-Sep-2007 Bill Seligman
///
/// 27-Jan-2009 <seligman@nevis.columbia.edu> Revised for LArSoft.
///
/// In my experience, people barely understand what the UserAction
/// interface does.  So why do we need a UserActionManager class?
///
/// Suppose I've written a class that inherits from UserAction that
/// makes histograms.  You've written a class that inherits from
/// UserAction to write events to a disk file.  Jane has written an
/// UserAction that makes ntuples.  A big massive 500-CPU-hour run of
/// G4 is planned, and we're all planning how to put our user-action
/// classes together.
///
/// By using a UserActionManager class, each one of us can develop our
/// own user-action classes independently.  Then, when we have the big
/// run, the user-action classes can be put successively called by the
/// UserActionManager without changing any of the classes.
///
/// Another feature is the ability to separate different user-action
/// functions.  For example, you don't have to mix your code that
/// writes hits with the code that makes histograms; the code can be
/// put into separate classes.
///
/// 18-Sep-2007: Make this a true "Manager" class by turning it into a
/// singleton.  Give the UserAction-derived classes access to the
/// Geant4 user-class managers.
/// 
/// 2012-08-17:  <rhatcher@fnal.gov> Add G4UserStackingAction interfaces.
/// By default these aren't invoked unless the UserAction::ProvidesStacking()
/// returns "true".  Generally there should be only one such in the managed
/// list, but if there are and they can't agree on the track classification
/// then prioritize them in what seems a sensible manner.

#ifndef G4BASE_UserActionManager_H
#define G4BASE_UserActionManager_H

#include "G4Base/UserAction.h"

#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4UserEventAction.hh"
#include "Geant4/G4UserTrackingAction.hh"
#include "Geant4/G4UserSteppingAction.hh"
#include "Geant4/G4UserStackingAction.hh"

#include "Geant4/G4Run.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4Step.hh"

#include "Geant4/G4EventManager.hh"
#include "Geant4/G4TrackingManager.hh"
#include "Geant4/G4SteppingManager.hh"

#include <vector>

namespace g4b {

  class UserActionManager : public G4UserRunAction
			  , public G4UserEventAction
			  , public G4UserTrackingAction
			  , public G4UserSteppingAction 
                          , public G4UserStackingAction {
  public:
  
    // Access to instance:
    static UserActionManager* Instance();

    virtual ~UserActionManager();
  
    // Delete all the UserAction classes we manage.
    void Close();

    G4int       GetSize()                 const { return fuserActions.size(); }
    UserAction* GetAction(G4int i)        const { return fuserActions[i];     }
    UserAction* GetAction(std::string const& name) const;
    G4int       GetIndex(std::string const& name) const;

    void        PrintActionList(std::string const& opt) const;

    static void AddAndAdoptAction(UserAction* a){ fuserActions.push_back(a);  }

    // G4UserRunAction interfaces
    virtual void BeginOfRunAction      (const G4Run*  );
    virtual void EndOfRunAction        (const G4Run*  );
    // G4UserEventAction interfaces
    virtual void BeginOfEventAction    (const G4Event*);
    virtual void EndOfEventAction      (const G4Event*);
    // G4UserTrackingAction interfaces
    virtual void PreUserTrackingAction (const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
    // G4UserSteppingAction interface
    virtual void UserSteppingAction    (const G4Step* );
    // G4UserStackingAction interfaces
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
    virtual void NewStage();
    virtual void PrepareNewEvent();
    virtual bool DoesAnyActionProvideStacking();  // do any managed UserActions do stacking

    // "Mysterious accessors": Where do the pointers to these managers
    // come from?  They are all defined in the G4User*Action classes.
    // Use care when calling these accessors; for example, the
    // SteppingManager is probably not available in a TrackingAction
    // method.  Keep the heirarchy in mind: Run > Event > Track > Step.
    G4EventManager*    GetEventManager()    const { return fpEventManager;    }
    G4TrackingManager* GetTrackingManager() const { return fpTrackingManager; }
    G4SteppingManager* GetSteppingManager() const { return fpSteppingManager; }

  private:
    typedef std::vector<UserAction*>       fuserActions_t;
    typedef fuserActions_t::const_iterator fuserActions_ptr_t;
    static  fuserActions_t                 fuserActions; 

  protected:
    // The constructor is protected according to the standard
    // singleton pattern.
    UserActionManager();
  
  };

} // namespace g4b

#endif // G4BASE_UserActionManager_H
