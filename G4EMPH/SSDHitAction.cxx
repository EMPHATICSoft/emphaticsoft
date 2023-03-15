////////////////////////////////////////////////////////////////////////
/// \file  SSDHitAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of SSDHits
///
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "G4EMPH/SSDHitAction.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/SSDHit.h"

#include <vector>
#include <map>

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
#include "Geant4/G4EnergyLossForExtrapolator.hh"
#include "Geant4/globals.hh"

// ART includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"
//#include "art_root_io/TFileService.h"
//#include "art_root_io/TFileDirectory.h"

#include "G4Base/UserActionFactory.h"
USERACTIONREG3(emph,SSDHitAction,emph::SSDHitAction)

#include <iostream>
#include <iomanip>

	namespace emph
{

	//-------------------------------------------------------------
	// Constructor.
	SSDHitAction::SSDHitAction() : 
		fEnergyCut(0)
	{
		fRunManager = G4RunManager::GetRunManager();
	}

	//-------------------------------------------------------------
	// Destructor.
	SSDHitAction::~SSDHitAction()
	{
		fFOutStudy1.close();
	}

	//-------------------------------------------------------------
	void SSDHitAction::Config(fhicl::ParameterSet const& pset )
	{
		fEnergyCut                    = pset.get< double >("G4EnergyThreshold")*CLHEP::GeV;
		std::cerr << " SSDHitAction::Config Energy Cut " << fEnergyCut*CLHEP::GeV << " in GeV " << std::endl;
		std::string aTokenJob = pset.get< std::string >("G4TokenSSDOut", "Undef");
		std::ostringstream fNameStrStr; fNameStrStr << "./G4EMPHSSDTuple_V1_" << aTokenJob << ".txt";
		std::string fNameStr(fNameStrStr.str());
		fFOutStudy1.open(fNameStr.c_str());
		fFOutStudy1 << " evt track pId x y z px py pz  " << std::endl;
	}

	//-------------------------------------------------------------
	void SSDHitAction::BeginOfEventAction(const G4Event*)
	{
		// Clear any previous particle information.
		fSSDHits.clear();

	}

	//-------------------------------------------------------------
	// Create our initial sim::SSDHit vector and add it to the vector of vectors
	void SSDHitAction::PreTrackingAction(const G4Track* ) //track)
	{
		//    std::vector<sim::SSDHit> ssdVec;
		//    fSSDHits.push_back(ssdVec);

		//    MF_LOG_DEBUG("SSDHitAction") << "pretracking step with track id: "
		//				 << ParticleListAction::GetCurrentTrackID();
	}

	//-------------------------------------------------------------
	void SSDHitAction::PostTrackingAction( const G4Track* /*track*/) 
	{
		//    MF_LOG_DEBUG("SSDHitAction") 
		//      << "done tracking for this g4 track, recorded "
		//      << fSSDHits[fSSDHits.size()-1].size() << " SSD hits";
	}

	//-------------------------------------------------------------
	// With every step, add to the particle's trajectory.
	void SSDHitAction::SteppingAction(const G4Step* step)
	{
		//mf::LogInfo("SSDHitAction") << "SSDHitAction::SteppingAction";

		/// Get the pointer to the track
		G4Track *track = step->GetTrack();

		//check that we are in the correct material to record a hit - ie scintillator
		std::string material = track->GetMaterial()->GetName();
		if(material.compare("SiliconWafer") != 0 ) {
			return;
		}
//		fFOutStudy1 << " " << material << " " << track->GetStep()->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName() << " " <<  track->GetStep()->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName() ;

		const CLHEP::Hep3Vector &pos0 = step->GetPreStepPoint()->GetPosition(); // Start of the step
		const CLHEP::Hep3Vector &pos  = track->GetPosition();                   // End of the step
		const CLHEP::Hep3Vector &mom  = track->GetMomentum();

		MF_LOG_DEBUG("SSDHitAction") << " momentum = "
			<< mom.x() << " " << mom.y() << " " 
			<< mom.z() << " " << mom.mag();

		double tpos0[3] = {pos0.x()/CLHEP::mm, pos0.y()/CLHEP::mm, pos0.z()/CLHEP::mm}; ///< Start of the step
		double tpos1[3] = {pos.x()/CLHEP::mm , pos.y()/CLHEP::mm , pos.z()/CLHEP::mm};  ///< End of the step

		// If it's a null step, don't use it. Otherwise it may induce an additional 
		// SSDHit, which is wrong
		if(tpos0[0]==tpos1[0] &&
				tpos0[1]==tpos1[1] &&
				tpos0[2]==tpos1[2])return;

		// Use the position ~1 nm away from the start of step to avoid the ambiguity on boundary
		double shift=1e-6;
		double tpos_s[3];
		for(int i=0;i<3;i++)tpos_s[i] = tpos0[i]+shift*(tpos1[i]-tpos0[i]);

		double mom0[3] = {mom.x()/CLHEP::GeV, mom.y()/CLHEP::GeV, mom.z()/CLHEP::GeV};

		// We should probably put some logic in to stop tracking if we know the
		// particle won't hit anything else in the spectrometer.  Eg,
		/*
			if(ParticleIsHeadedOutsideOfSpectrometer)
			track->SetTrackStatus(fStopAndKill);
			*/

		// Get the position and time the particle first enters
		// the volume, as well as the pdg code.  that information is
		// obtained from the G4Track object that corresponds to this step
		// account for the fact that we use cm, ns, GeV rather than the G4 defaults
		sim::SSDHit ssdHit;

		ssdHit.SetPId( track->GetDefinition()->GetPDGEncoding() );
		ssdHit.SetTrackID( track->GetTrackID() );
		const double edep = step->GetTotalEnergyDeposit()/CLHEP::GeV;
		ssdHit.SetDE(edep);

		// need to add code to figure out SSD plane, sensor and strip

		/// Add position, momentum
		ssdHit.SetX(tpos_s);
		ssdHit.SetP(mom0);

		fSSDHits.push_back(ssdHit);
		fFOutStudy1 << " " << fRunManager->GetCurrentEvent()->GetEventID();
		fFOutStudy1 << " " << track->GetTrackID() << " " << track->GetDefinition()->GetPDGEncoding() << std::endl;
		fFOutStudy1 << " " << tpos0[0] << " " << tpos0[1] << " " << tpos0[2]  << std::endl;
		fFOutStudy1 << " " << tpos1[0] << " " << tpos1[1] << " " << tpos1[2]  << std::endl;
		fFOutStudy1 << " " << mom0[0] << " " << mom0[1] << " " << mom0[2] << " " << step->GetTotalEnergyDeposit() << std::endl;




		//fFLSHit->AddPos(tpos2[0], tpos2[1], tpos2[2], (double)step->GetPreStepPoint()->GetGlobalTime()/CLHEP::ns, step->GetPreStepPoint()->GetKineticEnergy()  / CLHEP::GeV);


	}// end of SSDHitAction::SteppingAction


	//------------------------------------------------------------------------------
	// There's one last thing to do: All the particles have their
	// parent IDs set (in PostTrackingAction), but we haven't set the
	// daughters yet.  That's done in this method.
	void SSDHitAction::EndOfEventAction(const G4Event*)
	{

	}



}//end namespace
