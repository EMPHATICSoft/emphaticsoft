////////////////////////////////////////////////////////////////////////
/// \file  ARICHHitAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of ARICHHits.  Simulation of creation (not propagation) of Ceenkov photon in glass) 
///  
///  
/// \author  lebrun@fnal.gov
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <climits>
#include <cfloat>

#include <cstdlib>
#include "G4EMPH/ARICHHitAction.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/ARICHHit.h"

#include <vector>
#include <map>

// G4 includes
#include "Geant4/G4Event.hh"
#include "Geant4/G4Track.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4DynamicParticle.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4EnergyLossForExtrapolator.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/globals.hh"
#include "Geant4/G4Poisson.hh"
#include "Geant4/G4PhysicalConstants.hh"

// ART includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"

#include "G4Base/UserActionFactory.h"
USERACTIONREG3(emph, ARICHHitAction, emph::ARICHHitAction)

namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  ARICHHitAction::ARICHHitAction() :
    fBlockNum(INT_MAX) // 
  {
    fRunManager = G4RunManager::GetRunManager();
  }

  //-------------------------------------------------------------
  // Destructor.
  ARICHHitAction::~ARICHHitAction()
  {
   fFOutStudy1.close();
  }

  //-------------------------------------------------------------
  void ARICHHitAction::Config(fhicl::ParameterSet const& pset )
  {
//    fEnergyCut                    = pset.get< double >("G4EnergyThreshold")*CLHEP::GeV;
    std::cerr << " ARICHHitAction::Config opening files, if not already done..  " << std::endl;
    if ( fFOutStudy1.is_open()) return;
    std::string aTokenJob = pset.get< std::string >("G4TokenARICHOut", "Undef");
    std::ostringstream fNameStrStr; fNameStrStr << "./G4EMPHARICHHitTuple_V1_" << aTokenJob << ".txt";
    std::string fNameStr(fNameStrStr.str());
    fFOutStudy1.open(fNameStr.c_str());
    fFOutStudy1 << " event parent_track mPMT_anode time " << std::endl;
  }

  //-------------------------------------------------------------
  void ARICHHitAction::BeginOfEventAction(const G4Event*)
  {
    // Clear any previous particle information.
//    fARICHHits.clear();
    fARICHHits.clear();
  }

  //-------------------------------------------------------------
  // Create our initial sim::ARICHHit vector and add it to the vector of vectors
  void ARICHHitAction::PreTrackingAction(const G4Track* ) //track)
  {
  }

  //-------------------------------------------------------------
  void ARICHHitAction::PostTrackingAction( const G4Track* aTrack) 
  {

	  const G4Step* step = aTrack->GetStep();
	  G4VPhysicalVolume* postvolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

	  if (aTrack->GetTrackStatus()==fStopAndKill && aTrack->GetParticleDefinition()->GetParticleName() =="opticalphoton" && postvolume){
		  std::string pVolName = postvolume->GetName();
		  if(pVolName.find("PMT_phys") == std::string::npos)return;
		  emph::arich_util::PMT mpmt = fGeo->Geo()->FindPMTByName(pVolName);

		  double e = step->GetPreStepPoint()->GetTotalEnergy();
		  //		  std::cout<<"e "<<e<<std::endl;
		  double l = h_Planck*c_light/e;//mm
		  //std::cout<<"wavelength "<<l<<" mm"<<std::endl;

//		  if(mpmt.ifDet(l)){  moved to Digitization  

			  sim::ARICHHit arichHit;
			  arichHit.SetBlockNumber(mpmt.PMTnum());
			  arichHit.AddToAncestorTrack(aTrack->GetParentID());
			  arichHit.SetTime(step->GetPreStepPoint()->GetGlobalTime()/CLHEP::second);
			  arichHit.SetWavelength(l*1e6);
			  arichHit.SetEnergyDepo(e); 

			  fARICHHits.push_back(arichHit);
			  fFOutStudy1 << " " << fRunManager->GetCurrentEvent()->GetEventID();
			  fFOutStudy1 << " " << aTrack->GetParentID();
			  fFOutStudy1 << " " << arichHit.GetBlockNumber();
			  fFOutStudy1 << " " << arichHit.GetTime() << std::endl;


		 // }
	  }


  }
  
  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void ARICHHitAction::SteppingAction(const G4Step* )
  {
		  
  }// end of ARICHHitAction::SteppingAction


  //------------------------------------------------------------------------------
  // There's one last thing to do: All the particles have their
  // parent IDs set (in PostTrackingAction), but we haven't set the
  // daughters yet.  That's done in this method.
  void ARICHHitAction::EndOfEventAction(const G4Event*)
  {
     
  }
}//end namespace
