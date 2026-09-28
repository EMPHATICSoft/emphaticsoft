////////////////////////////////////////////////////////////////////////
/// \file  TargetHitAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of TargetHits
///
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "G4EMPH/TargetHitAction.h"
#include "Simulation/TargetHit.h"

#include <vector>
#include <map>

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

#include "G4Base/UserActionFactory.h"
USERACTIONREG3(emph,TargetHitAction,emph::TargetHitAction)

#include <iostream>
#include <iomanip>

namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  TargetHitAction::TargetHitAction() : 
    fEnergyCut(0)
  {
    fRunManager = G4RunManager::GetRunManager();
  }

  //-------------------------------------------------------------
  // Destructor.
  TargetHitAction::~TargetHitAction()
  {
  }

  //-------------------------------------------------------------
  void TargetHitAction::Config(fhicl::ParameterSet const& pset )
  {
    fEnergyCut                    = pset.get< double >("G4EnergyThreshold")*CLHEP::GeV;
  }

  //-------------------------------------------------------------
  void TargetHitAction::BeginOfEventAction(const G4Event*)
  {
    // Clear any previous particle information.
//    std::cout << "TargetHitAction::BeginOfEventAction: Clearing previous " << fTargetHits.size() << " hits" << std::endl;
    fTargetHits.clear();
  }

  //-------------------------------------------------------------
  void TargetHitAction::PreTrackingAction(const G4Track* ) //track)
  {
  }

  //-------------------------------------------------------------
  void TargetHitAction::PostTrackingAction( const G4Track* /*track*/) 
  {
  }

  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void TargetHitAction::SteppingAction(const G4Step* step)
  {
    G4VPhysicalVolume* vol = step->GetPreStepPoint()->GetPhysicalVolume();
    std::string volStr = vol->GetName();
    if (volStr.find("target_phys") == std::string::npos) return;

    const double edep = step->GetTotalEnergyDeposit()/CLHEP::GeV;
    int pdgCode = step->GetTrack()->GetDefinition()->GetPDGEncoding();
    if (abs(pdgCode) == 11 || abs(pdgCode) > 10000) return; // skip electrons, positrons and nuclei
    
//    mf::LogInfo("TargetHitAction") << "Target HitAction::SteppingAction";

    std::string procName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
//    if (procName.find("Transportation") != std::string::npos) return;
//    if (procName.find("NoProcess") != std::string::npos) return;

    /// Get the pointer to the track
    G4Track *track = step->GetTrack();
    
    const CLHEP::Hep3Vector &prepos = step->GetPreStepPoint()->GetPosition(); // Start of the step
    const CLHEP::Hep3Vector &premom = step->GetPreStepPoint()->GetMomentum(); // Start of the step
    const CLHEP::Hep3Vector &postpos  = track->GetPosition();                   // End of the step
    const CLHEP::Hep3Vector &postmom  = track->GetMomentum();

    // skip if the pre-step momentum is zero
    if (premom.mag() == 0) return;

    double tpos0[3] = {prepos.x()/CLHEP::mm, prepos.y()/CLHEP::mm, prepos.z()/CLHEP::mm}; ///< Start of the step
    double tpos1[3] = {postpos.x()/CLHEP::mm , postpos.y()/CLHEP::mm , postpos.z()/CLHEP::mm};  ///< End of the step
    double tmom0[3] = {premom.x()/CLHEP::GeV, premom.y()/CLHEP::GeV, premom.z()/CLHEP::GeV}; ///< Start of the step
    double tmom1[3] = {postmom.x()/CLHEP::GeV, postmom.y()/CLHEP::GeV, postmom.z()/CLHEP::GeV};  ///< End of the step

    sim::TargetHit targetHit;

    G4int trackID = track->GetTrackID();
    targetHit.SetPId( pdgCode );
    targetHit.SetTrackID( trackID );
    targetHit.SetProcess( procName );
    targetHit.SetDE(edep);

    /// Add position, momentum
    targetHit.SetPreX(tpos0);
    targetHit.SetPreP(tmom0);
    targetHit.SetPostX(tpos1);
    targetHit.SetPostP(tmom1);

    fTargetHits.push_back(targetHit);

  }// end of TargetHitAction::SteppingAction


  //------------------------------------------------------------------------------
  void TargetHitAction::EndOfEventAction(const G4Event*)
  {

  }



}//end namespace
