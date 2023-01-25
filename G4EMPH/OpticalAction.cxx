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

// ROOT includes
#include "TGeoMaterial.h"
#include <TGeoManager.h>
#include "TH2D.h"

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
//#include "art_root_io/TFileService.h"
//#include "art_root_io/TFileDirectory.h"

#include "G4Base/UserActionFactory.h"
USERACTIONREG3(emph,OpticalAction,emph::OpticalAction)


namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  OpticalAction::OpticalAction() : 
    fLastDetectorName("None")
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
    fLastDetectorName = pset.get< std::string >("G4LastDetector", "None");
    if (fLastDetectorName != "None") { 
      std::cerr << " OpticalAction::Config Last Detector for tracking " << fLastDetectorName << std::endl;
    }  
  }

  //-------------------------------------------------------------
  void OpticalAction::BeginOfEventAction(const G4Event *)
  {
  }

  //-------------------------------------------------------------
  // Create our initial sim::SSDHit vector and add it to the vector of vectors
  void OpticalAction::PreTrackingAction(const G4Track* ) //track)
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
	  G4Track * track = theStep->GetTrack();
	  if (track->GetParticleDefinition()->GetParticleName() !="opticalphoton") return;

	  if (theStep->GetPreStepPoint() == NULL) return;
	  if (theStep->GetPreStepPoint() == NULL) return;

	  if(theStep->GetPreStepPoint()->GetPhysicalVolume() != NULL &&
			  theStep->GetPostStepPoint()->GetPhysicalVolume()!= NULL) {

		  std::string preStepPointName  = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
		  if (preStepPointName != "ARICH_phys" ) 
		  { track->SetTrackStatus(fStopAndKill); return; }
	  }

	  //     std::cerr << " Leaving OpticalAction::SteppingAction " << std::endl;
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
