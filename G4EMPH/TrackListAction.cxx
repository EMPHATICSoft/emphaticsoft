////////////////////////////////////////////////////////////////////////
/// \file  TrackListAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of Tracks propagaed in EMPH 
///
/// \author  lebrun@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "G4EMPH/TrackListAction.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/SSDHit.h"

#include <vector>

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
USERACTIONREG3(emph,TrackListAction,emph::TrackListAction)

#include <iostream>
#include <iomanip>

namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  TrackListAction::TrackListAction() : 
    fEnergyCut(1.0*CLHEP::MeV)
  {
    fRunManager = G4RunManager::GetRunManager();
  }
  void TrackListAction::SetEnergyThreshold(double e) {
    fEnergyCut = e;
  }
  //-------------------------------------------------------------
  // Destructor.
  TrackListAction::~TrackListAction()
  {
   fFOutStudy1.close();
  }

  //-------------------------------------------------------------
  void TrackListAction::Config(fhicl::ParameterSet const& pset )
  {
    fEnergyCut                    = pset.get< double >("G4EnergyThreshold", 0.0001)*CLHEP::GeV;
    std::cerr << " TrackListAction::Config Energy Cut " << fEnergyCut*CLHEP::GeV << " in GeV " << std::endl;
    std::string aTokenJob = pset.get< std::string >("G4TokenSSDOut", "Undef");
    std::ostringstream fNameStrStr; fNameStrStr << "./G4EMPHTrackListTuple_V1_" << aTokenJob << ".txt";
    std::string fNameStr(fNameStrStr.str());
    fFOutStudy1.open(fNameStr.c_str());
    fFOutStudy1 << " evt track parentTrack pId x y z px py pz  " << std::endl;
  }

  //-------------------------------------------------------------
  void TrackListAction::BeginOfEventAction(const G4Event*)
  {
    // Clear any previous particle information.
    fTracks.clear();

  }

  //-------------------------------------------------------------
  // Create our initial sim::SSDHit vector and add it to the vector of vectors
  void TrackListAction::PreTrackingAction(const G4Track* aTrack) //track)
  {
    sim::Track myTrack;
    if (aTrack->GetTotalEnergy() < fEnergyCut) return;
    myTrack.SetTrackID(aTrack->GetTrackID());
    myTrack.SetPId(aTrack->GetParticleDefinition()->GetPDGEncoding());
    myTrack.SetParentTrackID(aTrack->GetParentID());
    G4ThreeVector tPos = aTrack->GetPosition();
    G4ThreeVector tMom = aTrack->GetMomentum();
    myTrack.SetPosition(tPos.x(), tPos.y(), tPos.z());
    myTrack.SetMomentum(tMom.x(), tMom.y(), tMom.z());
    
    fTracks.push_back(myTrack);
    
    fFOutStudy1 << " " << fRunManager->GetCurrentEvent()->GetEventID();
    fFOutStudy1 << " " << aTrack->GetTrackID() << " " << aTrack->GetParentID() << " " << aTrack->GetDefinition()->GetPDGEncoding();
    fFOutStudy1 << " " << tPos[0] << " " << tPos[1] << " " << tPos[2];
    fFOutStudy1 << " " << tMom[0] << " " << tMom[1] << " " << tMom[2] << std::endl;

  }

  //-------------------------------------------------------------
  void TrackListAction::PostTrackingAction( const G4Track* /*track*/) 
  {
  }
  
  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void TrackListAction::SteppingAction(const G4Step* /*step*/)
  {
  }

  //------------------------------------------------------------------------------
  // There's one last thing to do: All the particles have their
  // parent IDs set (in PostTrackingAction), but we haven't set the
  // daughters yet.  That's done in this method.
  void TrackListAction::EndOfEventAction(const G4Event*)
  {

  }



}//end namespace
