////////////////////////////////////////////////////////////////////////
/// \file  OpticalAction.cxx
/// \brief Kill optical photons outside ARICH
///
/// \author wanly@bu.edu 
////////////////////////////////////////////////////////////////////////
#include "G4EMPH/OpticalAction.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/SSDHit.h"

#include <vector>
#include <map>
#include <cfloat>
#include <climits>
#include <iostream>
#include <iomanip>

// G4 includes
#include "Geant4/G4Event.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/globals.hh"

// ART includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"

#include "G4Base/UserActionFactory.h"
USERACTIONREG3(emph,OpticalAction,emph::OpticalAction)


namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  OpticalAction::OpticalAction() : 
    ON_OFF(false)
  {
    fRunManager = G4RunManager::GetRunManager();
  }

  //-------------------------------------------------------------
  // Destructor.
  OpticalAction::~OpticalAction()
  {
  }

  //-------------------------------------------------------------
  void OpticalAction::Config(fhicl::ParameterSet const& pset )
  {
    ON_OFF = pset.get<bool>("G4OpticalSwitch", false);
	 std::cout << "G4OpticalSwitch: " << ON_OFF << std::endl;
  }

  //-------------------------------------------------------------
  void OpticalAction::BeginOfEventAction(const G4Event *)
  {
  }

  //-------------------------------------------------------------
  // Create our initial sim::SSDHit vector and add it to the vector of vectors
  void OpticalAction::PreTrackingAction(const G4Track*) //track)
  {
  }

  //-------------------------------------------------------------
  void OpticalAction::PostTrackingAction( const G4Track* /*track*/) 
  {
  }
  
  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void OpticalAction::SteppingAction(const G4Step* theStep)
  {
	  //mf::LogInfo("OpticalAction") << "OpticalAction::SteppingAction";
	  //    std::cerr << " Entering OpticalAction::SteppingAction " << std::endl;
	  if(ON_OFF==false)return;

	  G4Track * track = theStep->GetTrack();
	  if (track->GetParticleDefinition()->GetParticleName() !="opticalphoton") return;

	  if (theStep->GetPreStepPoint() == NULL) return;

	  std::string preStepPointName  = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
		  if (preStepPointName.find("aerogel")==std::string::npos && preStepPointName.find("ARICH")==std::string::npos)
		  {
			  std::cout<<"preStepName "<<preStepPointName<<std::endl;
			  track->SetTrackStatus(fStopAndKill); 
			  return; 
		  }

  }// end of OpticalAction::SteppingAction


  //------------------------------------------------------------------------------
  // There's one last thing to do: All the particles have their
  // parent IDs set (in PostTrackingAction), but we haven't set the
  // daughters yet.  That's done in this method.
  void OpticalAction::EndOfEventAction(const G4Event*)
  {
//    std::cerr << " OpticalAction::EndOfEventAction at Event " << evt->GetEventID() << std::endl;

  }



}//end namespace
