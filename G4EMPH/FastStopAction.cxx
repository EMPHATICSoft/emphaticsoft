////////////////////////////////////////////////////////////////////////
/// \file  FastStopAction.cxx
/// \brief Kill tracks, improve the speed of tracking.. No need to generate showers in the Lead Glass. 
///
/// \author  lebrun@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "G4EMPH/FastStopAction.h"
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
USERACTIONREG3(emph,FastStopAction,emph::FastStopAction)


namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  FastStopAction::FastStopAction() : 
    fLastZPos(DBL_MAX/2),
    fLastStationNumber(INT_MAX),
    fLastDetectorName("None")
  {
    fRunManager = G4RunManager::GetRunManager();
  }

  //-------------------------------------------------------------
  // Destructor.
  FastStopAction::~FastStopAction()
  {
  }

  //-------------------------------------------------------------
  void FastStopAction::Config(fhicl::ParameterSet const& pset )
  {
    fLastZPos                    = pset.get< double >("G4LastZPosition", DBL_MAX/2)*CLHEP::mm;
    if (fLastZPos < DBL_MAX/4) std::cerr << " FastStopAction::Config Last Z Position for tracking " << fLastZPos*CLHEP::meter << " meters " << std::endl;
    fLastDetectorName = pset.get< std::string >("G4LastDetector", "None");
    if (fLastDetectorName != "None") { 
      std::cerr << " FastStopAction::Config Last Detector for tracking " << fLastDetectorName << std::endl;
      size_t ii = fLastDetectorName.find("SSDStation");
      if (ii != std::string::npos) {
        ii+=10;
	std::string iStrNum = fLastDetectorName.substr(ii, 1);
	fLastStationNumber = atoi(iStrNum.c_str());
        std::cerr << " FastStopAction::Config Last SSD station number is " << fLastStationNumber << std::endl;
      }
      ii = fLastDetectorName.find("ssdStation");
      if (ii != std::string::npos) {
        ii+=10;
	std::string iStrNum = fLastDetectorName.substr(ii, 1);
	fLastStationNumber = atoi(iStrNum.c_str());
        std::cerr << " FastStopAction::Config Last SSD station number is " << fLastStationNumber << std::endl;
      }
      
    }  
  }

  //-------------------------------------------------------------
  void FastStopAction::BeginOfEventAction(const G4Event *)
  {
//    std::cerr << " FastStopAction::BeginOfEventAction at Event " << evt->GetEventID() << std::endl;
  }

  //-------------------------------------------------------------
  // Create our initial sim::SSDHit vector and add it to the vector of vectors
  void FastStopAction::PreTrackingAction(const G4Track* ) //track)
  {
    //    MF_LOG_DEBUG("FastStopAction") << "pretracking step with track id: "
    //				 << ParticleListAction::GetCurrentTrackID();
  }

  //-------------------------------------------------------------
  void FastStopAction::PostTrackingAction( const G4Track* /*track*/) 
  {
  }
  
  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void FastStopAction::SteppingAction(const G4Step* theStep)
  {
    //mf::LogInfo("FastStopAction") << "FastStopAction::SteppingAction";
//    std::cerr << " Entering FastStopAction::SteppingAction " << std::endl;
    if (theStep->GetPostStepPoint() == NULL) return;
    if (theStep->GetPreStepPoint() == NULL) return;
    const CLHEP::Hep3Vector &posFinal = theStep->GetPostStepPoint()->GetPosition(); // end of the step 
    G4Track * track = theStep->GetTrack();
    if (posFinal[2] > fLastZPos)  { track->SetTrackStatus(fStopAndKill); return; } // did it! 
//    std::cerr << " ... At Z " << posFinal[2] << std::endl;
    if (fLastDetectorName == std::string("None")) {
//      std::cerr << " Leaving FastStopAction::SteppingAction, no end volume defined  " << std::endl;       
       return;
     }
    if (fLastStationNumber != INT_MAX) {
      
      if(theStep->GetPreStepPoint()->GetPhysicalVolume() != NULL &&
        theStep->GetPostStepPoint()->GetPhysicalVolume()!= NULL) {

       std::string preStepPointName  = theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
       std::string postStepPointName  = theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
       std::ostringstream keyNameStrStr; keyNameStrStr << "ssdStation" << fLastStationNumber << "_phys";
       std::string keyNameStr(keyNameStrStr.str());
       if ((preStepPointName == keyNameStrStr.str()) && (postStepPointName.find("world") != std::string::npos)) 
         { track->SetTrackStatus(fStopAndKill); return; }
      }
	 
    }
//     std::cerr << " Leaving FastStopAction::SteppingAction " << std::endl;
 }// end of FastStopAction::SteppingAction


  //------------------------------------------------------------------------------
  // There's one last thing to do: All the particles have their
  // parent IDs set (in PostTrackingAction), but we haven't set the
  // daughters yet.  That's done in this method.
  void FastStopAction::EndOfEventAction(const G4Event*)
  {
//    std::cerr << " FastStopAction::EndOfEventAction at Event " << evt->GetEventID() << std::endl;

  }



}//end namespace
