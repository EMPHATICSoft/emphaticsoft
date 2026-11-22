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
    fBlockNum(INT_MAX), // 
    ngamma_gen_aero0(0),
    ngamma_gen_aero1(0)
  {
    fRunManager = G4RunManager::GetRunManager();
  }

  //-------------------------------------------------------------
  // Destructor.
  ARICHHitAction::~ARICHHitAction()
  {
      fARICHfile.close();
  }

  //-------------------------------------------------------------
  void ARICHHitAction::Config(fhicl::ParameterSet const& pset )
  {
//    fEnergyCut                    = pset.get< double >("G4EnergyThreshold")*CLHEP::GeV;
      fARICHfile.open("ARICHfile.txt");
  }

  //-------------------------------------------------------------
  void ARICHHitAction::BeginOfEventAction(const G4Event*)
  {
    // Clear any previous particle information.
//    fARICHHits.clear();
    fARICHHits.clear();
    ngamma_gen_aero0=0;
    ngamma_gen_aero1=0;   
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
	  const G4ThreeVector& p = step->GetPreStepPoint()->GetPosition();

/*	  if(aTrack->GetParticleDefinition()->GetParticleName() =="opticalphoton" && postvolume){ 
	     std::string cVolName = aTrack->GetLogicalVolumeAtVertex()->GetName();
	     std::string processName = aTrack->GetCreatorProcess()->GetProcessName(); 

	     bool isCerenkov = processName.find("Cerenkov") != std::string::npos;
	     bool primaryparent = aTrack->GetParentID() == 1;

	     if( cVolName.find("aerogel_vol0") != std::string::npos && isCerenkov && primaryparent)ngamma_gen_aero0++;
	     else if(cVolName.find("aerogel_vol1") != std::string::npos && isCerenkov && primaryparent)ngamma_gen_aero1++;	
	  }
*/	  
	  if (aTrack->GetTrackStatus()==fStopAndKill && aTrack->GetParticleDefinition()->GetParticleName() =="opticalphoton" && postvolume){
		  std::string pVolName = postvolume->GetName();
		  std::string processName = aTrack->GetCreatorProcess()->GetProcessName(); 
		  std::string gVolName =  aTrack->GetLogicalVolumeAtVertex()->GetName();

        	  bool isCerenkov = processName.find("Cerenkov") != std::string::npos;
	          //bool primaryparent = aTrack->GetParentID() == 1;

		  bool gen_in_aerogel =  gVolName.find("aerogel_vol") != std::string::npos;

		  if(pVolName.find("PMT_phys") == std::string::npos)return;
		  if(!isCerenkov && !gen_in_aerogel)return;

		  ngamma_gen_aero0++;

		 /*std::cout << "TrackID " << aTrack->GetTrackID() << " particle " << aTrack->GetParticleDefinition()->GetParticleName()  << 
		 " parent ID " << aTrack->GetParentID() <<" made in " << aTrack->GetLogicalVolumeAtVertex()->GetName() 
		<< " creator process " << aTrack->GetCreatorProcess()->GetProcessName() << std::endl;
		*/
		
		  const emph::arich_util::PMT& mpmt = fGeo->Geo()->FindPMTByName(pVolName);

		  double e = step->GetPreStepPoint()->GetTotalEnergy();
		  //		  std::cout<<"e "<<e<<std::endl;
		  double l = h_Planck*c_light/e;//mm
		  //std::cout<<"wavelength "<<l<<" mm"<<std::endl;

			  sim::ARICHHit arichHit;
			  arichHit.SetBlockNumber(mpmt.PMTnum());
			  arichHit.AddToAncestorTrack(aTrack->GetParentID());
			  arichHit.SetTime(step->GetPreStepPoint()->GetGlobalTime()/CLHEP::second);
			  arichHit.SetWavelength(l*1e6);
			  arichHit.SetEnergyDepo(e);
			  arichHit.SetPosx(p.x());
			  arichHit.SetPosy(p.y()); 			   
			  fARICHHits.push_back(arichHit);
/*			  
			  if (fSaveTextFiles) {
			  fFOutStudy1 << " " << fRunManager->GetCurrentEvent()->GetEventID();
			  fFOutStudy1 << " " << aTrack->GetParentID();
			  fFOutStudy1 << " " << arichHit.GetBlockNumber();
			  fFOutStudy1 << " " << arichHit.GetTime() << std::endl;
}
*/
	  }
  }
  
  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void ARICHHitAction::SteppingAction(const G4Step* step)
  {	
	G4Track *aTrack = step->GetTrack(); 
	G4VPhysicalVolume* postvolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();	
	 const G4ThreeVector& p = step->GetPreStepPoint()->GetPosition();

	if(abs(aTrack->GetParticleDefinition()->GetPDGCharge()) != 1 || !postvolume)return;
	 std::string pVolName = postvolume->GetName();	
	if(pVolName.find("PMT_phys") == std::string::npos)return;
	const int pdg = aTrack->GetParticleDefinition()->GetPDGEncoding();
	if (std::abs(pdg) == 11) return;
	
	const emph::arich_util::PMT& mpmt = fGeo->Geo()->FindPMTByName(pVolName);      
	
  	double e = step->GetTotalEnergyDeposit();
  	double l = 0;

  	sim::ARICHHit arichHit;
  	arichHit.SetBlockNumber(mpmt.PMTnum());
  	arichHit.AddToAncestorTrack(aTrack->GetParentID());
  	arichHit.SetTime(step->GetPreStepPoint()->GetGlobalTime()/CLHEP::second);
  	arichHit.SetWavelength(l*1e6);
  	arichHit.SetEnergyDepo(e);
	arichHit.SetPosx(p.x());
        arichHit.SetPosy(p.y());
 	fARICHHits.push_back(arichHit);

	// std::cout << aTrack->GetParticleDefinition()->GetParticleName() << " parent id " <<  aTrack->GetParentID() << " e depo " << step->GetTotalEnergyDeposit() << " " << postvolume->GetName() << std::endl;		  


		  
  }// end of ARICHHitAction::SteppingAction


  //------------------------------------------------------------------------------
  // There's one last thing to do: All the particles have their
  // parent IDs set (in PostTrackingAction), but we haven't set the
  // daughters yet.  That's done in this method.
  void ARICHHitAction::EndOfEventAction(const G4Event*)
  {
	fARICHfile << "N gamma areo0 "<<  ngamma_gen_aero0 << " N gamma areo1 " << ngamma_gen_aero1 << std::endl;
  }
}//end namespace
