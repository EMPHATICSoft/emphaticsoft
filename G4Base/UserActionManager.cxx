////////////////////////////////////////////////////////////////////////
/// \file  UserActionManager.cxx
/// \brief Create the physics lists to be used by Geant4.
///
/// \version $Id: UserActionManager.cxx,v 1.4 2012-09-20 21:47:06 greenc Exp $
/// \author  seligman@nevis.columbia.edu
////////////////////////////////////////////////////////////////////////
/// 18-Sep-2007 Bill Seligman
/// Invoke the appropriate action for each stored user-action class.

/// 27-Jan-2009 <seligman@nevis.columbia.edu> Revised for LArSoft.
/// 2012-08-17: <rhatcher@fnal.gov> Add G4UserStackingAction interfaces

#include "G4Base/UserActionManager.h"
#include "G4Base/UserAction.h"

#include "Geant4/G4Run.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4Step.hh"

#include <vector>
#include <map>

namespace g4b {

  UserActionManager::fuserActions_t UserActionManager::fuserActions;

  //-------------------------------------------------
  UserActionManager::UserActionManager() 
  {
  }

  //-------------------------------------------------
  // Standard implementation of a singleton pattern.
  UserActionManager* UserActionManager::Instance()
  {
    static UserActionManager instance;
    return &instance;
  }

  //-------------------------------------------------
  UserActionManager::~UserActionManager()
  {
    // This destructor is probably never going to be called.  If it
    // is, make sure all the UserAction classes we manage are deleted
    // properly.
    Close();
  }


  //-------------------------------------------------
  void UserActionManager::Close()
  {
    // Since we adopted the pointers for the user-action classes we're
    // managing, delete all of them.
    for ( fuserActions_t::iterator i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      delete *i;
    }
  
    fuserActions.clear();
  }

  //-------------------------------------------------
  G4int UserActionManager::GetIndex(std::string const& name) const
  {
    int indx=0;
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++, indx++ ){
      if ( (*i)->GetName() == name ) return indx;
    }
    // not found
    return -1;
  }

  //-------------------------------------------------
  UserAction* UserActionManager::GetAction(std::string const& name) const
  {
    G4int indx = GetIndex(name);
    if ( indx < 0 ) return 0;  // not found
    return fuserActions[indx];
  }

  //-------------------------------------------------
  void UserActionManager::PrintActionList(std::string const& opt) const
  {
    bool pcfg = ( opt.find("config") != std::string::npos );
    std::cout << "UserActionManager::PrintActionList " << GetSize()
              << " entries" << std::endl;
    for ( G4int indx=0; indx < GetSize(); ++indx ) {
      UserAction* action = GetAction(indx);
      std::cout << "   [" << indx << "] " << action->GetName()
                << ( action->ProvidesStacking() ? " [stacking]":"" )
                << std::endl;
      if ( pcfg ) action->PrintConfig(opt);
    }
  }

  //-------------------------------------------------
  // For the rest of the UserAction methods: invoke the corresponding
  // method for each of the user-action classes we're managing.

  // Reminder: i is a vector<UserAction*>::iterator
  //          *i is a UserAction*

  //-------------------------------------------------
  void UserActionManager::BeginOfRunAction(const G4Run* a_run)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      (*i)->BeginOfRunAction(a_run);
    }
  }

  //-------------------------------------------------
  void UserActionManager::EndOfRunAction(const G4Run* a_run)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      (*i)->EndOfRunAction(a_run);
    }
  }

  //-------------------------------------------------
  void UserActionManager::BeginOfEventAction(const G4Event* a_event)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      (*i)->BeginOfEventAction(a_event);
    }
  }

  //-------------------------------------------------
  void UserActionManager::EndOfEventAction(const G4Event* a_event)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ) {
      (*i)->EndOfEventAction(a_event);
    }
  }

  //-------------------------------------------------
  void UserActionManager::PreUserTrackingAction(const G4Track* a_track)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      (*i)->PreTrackingAction(a_track);
    }
  }

  //-------------------------------------------------
  void UserActionManager::PostUserTrackingAction(const G4Track* a_track)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      (*i)->PostTrackingAction(a_track);
    }
  }

  //-------------------------------------------------
  void UserActionManager::UserSteppingAction(const G4Step* a_step)
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      (*i)->SteppingAction(a_step);
    }
  }

  //-------------------------------------------------
  G4ClassificationOfNewTrack
    UserActionManager::ClassifyNewTrack(const G4Track* a_track)
  {
    std::map<G4ClassificationOfNewTrack,int> stackChoices;
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      if ( (*i)->ProvidesStacking() ) {
        G4ClassificationOfNewTrack choice = (*i)->StackClassifyNewTrack(a_track);
        stackChoices[choice]++;
      }
    }
    // based on all results pick an action
    //      fUrgent,    // put into the urgent stack
    //      fWaiting,   // put into the waiting stack
    //      fPostpone,  // postpone to the next event
    //      fKill       // kill without stacking

    //G4ClassificationOfNewTrack choice = fUrgent;  // safe choice
    // prioritize:  anyone shoots it, it's dead;
    //   then postpone; then waiting; finally just process it
    const G4ClassificationOfNewTrack priority[] = 
      { fKill, fPostpone, fWaiting, fUrgent };
    const size_t nprio = sizeof(priority)/sizeof(G4ClassificationOfNewTrack);
    for (unsigned int j=0; j<nprio; ++j) {
      G4ClassificationOfNewTrack saction = priority[j];
      if ( stackChoices[saction] > 0 ) return saction;
    } 
    // shouldn't get here (already covered) ... but a fall back
    return fUrgent;
  }

  //-------------------------------------------------
  void UserActionManager::NewStage()
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      if ( (*i)->ProvidesStacking() ) {
        (*i)->StackNewStage();
      }
    }
  }

  //-------------------------------------------------
  void UserActionManager::PrepareNewEvent()
  {
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      if ( (*i)->ProvidesStacking() ) {
        (*i)->StackPrepareNewEvent();
      }
    }
  }

  //-------------------------------------------------
  bool UserActionManager::DoesAnyActionProvideStacking()
  {
    // do any managed UserActions do stacking?
    bool doany = false;
    for ( fuserActions_ptr_t i = fuserActions.begin(); i != fuserActions.end(); i++ ){
      doany |= (*i)->ProvidesStacking();  // any == take the "or"
    }
    return doany;
  }

}// namespace
