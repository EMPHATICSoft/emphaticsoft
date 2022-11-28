////////////////////////////////////////////////////////////////////////
/// \file  TOPAZLGHitAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of TOPAZLGHits.  Simulation of creation (not propagation) of Ceenkov photon in glass) 
///  
///  
/// \author  lebrun@fnal.gov
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <climits>
#include <cfloat>

#include <cstdlib>
#include "G4EMPH/TOPAZLGHitAction.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/TOPAZLGHit.h"

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
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/globals.hh"
#include "Geant4/G4Poisson.hh"

// ART includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"
//#include "art_root_io/TFileService.h"
//#include "art_root_io/TFileDirectory.h"

#include "G4Base/UserActionFactory.h"
USERACTIONREG3(emph, TOPAZLGHitAction, emph::TOPAZLGHitAction)

namespace emph
{

  //-------------------------------------------------------------
  // Constructor.
  TOPAZLGHitAction::TOPAZLGHitAction() :
    fPrevBlockNum(INT_MAX),
    fRefractionIndex(1.8),  // from the NIM A  paper, A270 (1988) 11  
    fCritAngleTotalInt(std::asin(1.000293/fRefractionIndex)), 
    fNumPhotPerMm(4.8 * 370 * 0.1), // assume a PMT bandwith of 300 nm to 650 nm, approximetl4 4.8 eV. See PDG on Cerenkov radiation
                                   // last factor is from cm to mm.   
    fKnobAttenuation(0.9), //to be tuned. 
    fKnobCalibration(0.9), //to be tuned. 
    fZGlassUpstreamFace(DBL_MAX),   // set when a track with a momentum above ~100 Mev enters LG_glass Event dependant. 
    fGlassBlockLength(340.), fGeantinoInGlass(false),
    fEvtNum(0), // The event number. 
    fBlockNum(INT_MAX), // the Lead glass block number, if the a Geantino enters the lead glass. 
    fBlockNumExit(INT_MAX), // the Lead glass block number, for the exiting Geantino 
    fXStart(DBL_MAX), fYStart(DBL_MAX), fXMagnet(DBL_MAX), fYMagnet(DBL_MAX), fXLGUpstr(DBL_MAX), fXLGDownstr(DBL_MAX)
  {
    fRunManager = G4RunManager::GetRunManager();
  }

  //-------------------------------------------------------------
  // Destructor.
  TOPAZLGHitAction::~TOPAZLGHitAction()
  {
   fFOutStudy1.close();
   fGeantinoStudy1.close();
  }

  //-------------------------------------------------------------
  void TOPAZLGHitAction::Config(fhicl::ParameterSet const& pset )
  {
//    fEnergyCut                    = pset.get< double >("G4EnergyThreshold")*CLHEP::GeV;
    std::cerr << " TOPAZLGHitAction::Config opening files, if not already done..  " << std::endl;
    if ( fFOutStudy1.is_open()) return;
    fKnobAttenuation = pset.get<double>("G4AttenuationTOPAZLG", 0.9); 
                 // pure number, the probability for a photon to hit the photocathode, if the photonm is emitted close to the downstream face    
    fKnobCalibration = pset.get<double>("G4CalibTOPAZLG", 1.0); // overall conversion of optical photons to GeV. 
    std::string aTokenJob = pset.get< std::string >("G4TokenTOPAZLGOut", "Undef");
    std::ostringstream fNameStrStr; fNameStrStr << "./G4EMPHLGHitTuple_V1_" << aTokenJob << ".txt";
    std::string fNameStr(fNameStrStr.str());
    fFOutStudy1.open(fNameStr.c_str());
    fFOutStudy1 << " evt blkNum xm ym zm nSteps nPhot Edep EFrac " << std::endl;
    std::ostringstream fNameStrStr2; fNameStrStr2 << "./G4EMPHGeantinoTuple_V1_" << aTokenJob << ".txt";
    std::string fNameStr2(fNameStrStr2.str());
    fGeantinoStudy1.open(fNameStr2.c_str());
    fGeantinoStudy1 << " evt blkNum blkNumExit XSt YSt XMag YMag XLGUpstr XLGDownstr YLGUpstr YLGDownstr ZLGUpstr ZLGDownstr " << std::endl;
  }

  //-------------------------------------------------------------
  void TOPAZLGHitAction::BeginOfEventAction(const G4Event*)
  {
    // Clear any previous particle information.
//    fTOPAZLGHits.clear();
    for (std::vector<sim::TOPAZLGHit>::iterator it = fTOPAZLGHits.begin(); it != fTOPAZLGHits.end(); it++) 
       it->Reset();
    fXStart = DBL_MAX; fYStart = DBL_MAX;
    fXMagnet= DBL_MAX; fYMagnet = DBL_MAX;  
    fXLGUpstr = DBL_MAX; fXLGDownstr = DBL_MAX;
    fYLGUpstr = DBL_MAX; fYLGDownstr = DBL_MAX;
    fZLGUpstr = DBL_MAX; fZLGDownstr = DBL_MAX;

  }

  //-------------------------------------------------------------
  // Create our initial sim::TOPAZLGHit vector and add it to the vector of vectors
  void TOPAZLGHitAction::PreTrackingAction(const G4Track* aTrack) //track)
  {
  
    fEvtNum = fRunManager->GetCurrentEvent()->GetEventID(); fGeantinoInGlass = false;
    const G4ThreeVector pos = aTrack->GetPosition();
    int aPDG = aTrack->GetDefinition()->GetPDGEncoding();
    if (aPDG == 0) { // geantino  Could use the G4Geantino class.. Too lazy.. 
      fBlockNum = INT_MAX;   
      fBlockNumExit = INT_MAX;   
      fXStart = pos[0]; fYStart = pos[1];
      fXMagnet=DBL_MAX; fYMagnet = DBL_MAX;  fXLGUpstr = DBL_MAX; fXLGDownstr = DBL_MAX;
      fYLGUpstr = DBL_MAX; fYLGDownstr = DBL_MAX; fZLGUpstr = DBL_MAX; fZLGDownstr = DBL_MAX;
    } else {
      if ((pos[2] < 0.) && (aTrack->GetTrackID() == 1)) { // start of the first track. (beam track) 
        fXStart = pos[0]; fYStart = pos[1];
      }
    }
  }

  //-------------------------------------------------------------
  void TOPAZLGHitAction::PostTrackingAction( const G4Track* aTrack) 
  {
    const int aPDG = aTrack->GetDefinition()->GetPDGEncoding();
    if ((aPDG == 0) && (fBlockNum != INT_MAX)) {  
      fGeantinoStudy1 << " " << fEvtNum << " " << fBlockNum << " " << fBlockNumExit << " " << fXStart << " " << fYStart;
      fGeantinoStudy1 << " " << fXMagnet << " " << fYMagnet << " " <<  fXLGUpstr << " " << fXLGDownstr << " " << fYLGUpstr << " " << fYLGDownstr;
      fGeantinoStudy1 << " " <<  fZLGUpstr << " " << fZLGDownstr << std::endl;
    }
  }
  
  //-------------------------------------------------------------
  // With every step, add to the particle's trajectory.
  void TOPAZLGHitAction::SteppingAction(const G4Step* step)
  {
    //mf::LogInfo("TOPAZLGHitAction") << "TOPAZLGHitAction::SteppingAction";
  
    /// Get the pointer to the track
    G4Track *track = step->GetTrack(); // to be used later... 
    int aPDG = track->GetDefinition()->GetPDGEncoding();
    if (aPDG == 0) { 
      if (fEvtNum < 5) this->SteppingActionGeantinoASCIIDump(step); 
      this->SteppingActionGeantinoStat(step); 
    } else {
      const G4ParticleDefinition *def = track->GetParticleDefinition();
      if (std::abs(def->GetPDGCharge()/CLHEP::eplus) > 0.5)  this->SteppingActionEMShowers(step);
    }
  }
  void TOPAZLGHitAction::SteppingActionGeantinoASCIIDump(const G4Step* step)
  {
    const G4VPhysicalVolume *prePhysVolume = step->GetPreStepPoint()->GetPhysicalVolume();
    const G4VPhysicalVolume *postPhysVolume = step->GetPostStepPoint()->GetPhysicalVolume();
    if (prePhysVolume == nullptr) return;
    if (postPhysVolume == nullptr) return;
    
    if (step->IsFirstStepInVolume()) { // entering a LG block 
      std::string postVolName = postPhysVolume->GetName();
      std::string preVolName = prePhysVolume->GetName();
      if (postVolName.find("LG") == 0) {
        std::cerr << std::endl << "---------------------------------------------------------------------" << std::endl; 
        const CLHEP::Hep3Vector &pos0 = step->GetPreStepPoint()->GetPosition(); // Stend of the step
        const CLHEP::Hep3Vector &pos1 = step->GetPostStepPoint()->GetPosition(); // Stend of the step
        std::cerr << " TOPAZLGHitAction::SteppingAction, into the lead glass prevolname " << preVolName 
	          <<  " postVolName " << postVolName << " Z-pre " << pos0[2] << " Z-Post " << pos1[2] << std::endl;
	if (postVolName.find("LGCalo") == 0) {
          std::cerr << " .... into the calorimeter " << postVolName << std::endl; 
	  const G4LogicalVolume *logicCalo = postPhysVolume-> GetLogicalVolume();
	  int numDaughter = logicCalo->GetNoDaughters();
	  std::cerr << " ... For this volume, logic name is " << logicCalo->GetName() << " Number of daughters " << numDaughter << std::endl;
	  for (int kd = 4; kd != 5; kd++) {
	    
	    const G4VPhysicalVolume *aPhysD = logicCalo->GetDaughter(kd);
	    const G4LogicalVolume *logicVolD = aPhysD->GetLogicalVolume();
	    const G4VSolid *solVolD = logicVolD->GetSolid();
	    std::cerr << " ....... Daughter Phys name " <<  aPhysD->GetName() << " Logical Name " 
	            << logicVolD->GetName() << " Solid name " << solVolD->GetName() << std::endl;
	    solVolD->StreamInfo(std::cerr);   
	    solVolD->DumpInfo();	    
	
	  }
	} 	  
        if (postVolName.find("LG_block") != std::string::npos) {
	  const int blockNum = this->findBlockNumberFromName(postPhysVolume);
          std::cerr << " .... LG_block, Yeah, into the lead glass " << postVolName << " blkNum = " << blockNum 
	            << " z = " << pos1[2] << std::endl;
	  G4ThreeVector trans = postPhysVolume->GetTranslation();
	  const G4RotationMatrix *aRot = postPhysVolume->GetRotation();
	  std::cerr << " ... Translation " << trans << std::endl;
	  if (aRot != nullptr) { 
	    CLHEP::Hep3Vector rotColx = aRot->colX();
	    std::cerr << " Rotation, column X " << rotColx[0] << ", " << rotColx[1] << ", " << rotColx[2] << std::endl;
	    CLHEP::Hep3Vector rotColy = aRot->colY();
	    std::cerr << " Rotation, column Y " << rotColy[0] << ", " << rotColy[1] << ", " << rotColy[2] << std::endl;
	    CLHEP::Hep3Vector rotColz = aRot->colZ();
	    std::cerr << " Rotation, column Z " << rotColz[0] << ", " << rotColz[1] << ", " << rotColz[2] << std::endl;
	  } else {
	     std::cerr << " ... no rotation " << trans << std::endl;
	  }  
	  const G4LogicalVolume *logicVolTmp = prePhysVolume->GetLogicalVolume();
	  const G4VSolid *solidVolTmp = logicVolTmp->GetSolid();
//	const double cubicVolume = solidVolTmp->GetCubicVolume(); // Not available in this old version, my guess..  
          const G4Material *materialTmp = logicVolTmp->GetMaterial();  
	  std::cerr << " Logical Volume name is " << logicVolTmp->GetName() << " Solid name  " << solidVolTmp->GetName() 
	            <<  " Material Name " << materialTmp->GetName() << std::endl;
	//
	// Let us print the volume hierarchy, for each such physical volume, no matter where, so I can debug. 
	//
	  const G4LogicalVolume *logicVol = postPhysVolume-> GetLogicalVolume();
	  int numDaughter = logicVol->GetNoDaughters();
	  std::cerr << " ... For the prevolume, logic name is " << logicVol->GetName() << " Number of daughters " << numDaughter << std::endl;
	  for (int kd = 0; kd != 	numDaughter; kd++) {
	    const G4VPhysicalVolume *prePhysD = logicVol->GetDaughter(kd);
	    const G4LogicalVolume *logicVolD = prePhysD->GetLogicalVolume();
	    const G4VSolid *solVolD = logicVolD->GetSolid();
	    std::cerr << " ....... Daughter Phys name " <<  prePhysD->GetName() << " Logical Name " 
	            << logicVolD->GetName() << " Solid name " << solVolD->GetName() << std::endl;
	  }    
	}
        std::cerr << "=======================================================" << std::endl <<std::endl;; 
      }
    }
  }
  void TOPAZLGHitAction::SteppingActionGeantinoStat(const G4Step* step) 
  {
      const double ZLGOffset = 2800.; //such that I can histogram more accurately... 
      if (!step->IsFirstStepInVolume()) return; 
      const G4VPhysicalVolume *prePhysVolume = step->GetPreStepPoint()->GetPhysicalVolume();
      const G4VPhysicalVolume *postPhysVolume = step->GetPostStepPoint()->GetPhysicalVolume();
      if (prePhysVolume == nullptr) return;
      if (postPhysVolume == nullptr) return;
//      std::cerr << " TOPAZLGHitAction::SteppingActionGeantinoStat, begin.. evtNum " << fEvtNum << std::endl;
      std::string postVolName = postPhysVolume->GetName();
      std::string preVolName = prePhysVolume->GetName();
      const G4ThreeVector postPos = step->GetPostStepPoint()->GetPosition();
      const G4ThreeVector prePos = step->GetPreStepPoint()->GetPosition();
      if (postVolName.find("magnetSide") == 0) {
	fXMagnet = postPos[0]; fYMagnet = postPos[1]; return;
      }
      if (preVolName.find("LG") == 0) { 
//        std::cerr << "  ...... into  " << postVolName << " from " << preVolName << " Z-Pre " 
//	                               << prePos[2] << " Z-post " << postPos << std::endl;
        if ((!fGeantinoInGlass) && ((postVolName.find("LG_glass") == 0) && (preVolName.find("LG_block") == 0))) {
           fBlockNum = this->findBlockNumberFromName(prePhysVolume);
           fXLGUpstr = prePos[0]; fYLGUpstr = prePos[1]; fZLGUpstr = prePos[2] - ZLGOffset; 
//           std::cerr << "  ...... entering " << postVolName << " from " << preVolName <<  " block number " << fBlockNum << std::endl;
           fGeantinoInGlass = true;
	   return;
        } 
        if (fGeantinoInGlass && (((preVolName.find("LG_glass") == 0) && (postVolName.find("LG_block") == 0)) || 
	    ((preVolName.find("LG_glass") == 0) && (postVolName.find("LG_protru") == 0)))) {
//           std::cerr << "  ...... leaving" << preVolName << " to " << postVolName << "  at Z " << postPos[2] - ZLGOffset << std::endl;
	   if (postVolName.find("LG_block") == 0) fBlockNumExit = this->findBlockNumberFromName(prePhysVolume);
	   if (fBlockNumExit == INT_MAX) fBlockNumExit = fBlockNum; // it meas we exited in thack of the same counter, via the protusion 
           fXLGDownstr = postPos[0]; fYLGDownstr = postPos[1]; fZLGDownstr = postPos[2] - ZLGOffset; return;
        }
        if (fGeantinoInGlass && (preVolName.find("LG_block") == 0) && (postVolName.find("LGCalo") == 0)) {
//           std::cerr << "  ...... leaving" << preVolName << " to " << postVolName << "  at Z " << postPos[2] - ZLGOffset << std::endl;
	   fBlockNumExit = this->findBlockNumberFromName(prePhysVolume);
	   return;
        }
      }
   }
    
   void TOPAZLGHitAction::SteppingActionEMShowers(const G4Step* step) 
   {
//      std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers, start...  " << std::endl;
      const G4Track *theTrack = step->GetTrack();
      const G4VPhysicalVolume *prePhysVolume = step->GetPreStepPoint()->GetPhysicalVolume();
      const G4VPhysicalVolume *postPhysVolume = step->GetPostStepPoint()->GetPhysicalVolume();
      if (postPhysVolume == nullptr) return;
      std::string postVolName = postPhysVolume->GetName();
      std::string preVolName = prePhysVolume->GetName();
      if (preVolName.find("LG") == std::string::npos) return;     
      if (postVolName.find("LG") == std::string::npos) return;     
      const G4ThreeVector posPost = step->GetPostStepPoint()->GetPosition();
      if (fZGlassUpstreamFace == DBL_MAX) {
         std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers fZGlassUpstreamFace not set " << std::endl;
        if ((postVolName.find("LG_glass") == 0)) {
          fZGlassUpstreamFace = posPost[2];
	    std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers fZGlassUpstreamFace is now " << fZGlassUpstreamFace << std::endl;
	}
      }
      if ((postVolName.find("LG_glass") == std::string::npos) ||  
          (preVolName.find("LG_glass") == std::string::npos)) return;
// Get the block number from this name       
//      std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers Entering glass via " << preVolName << std::endl;
      fBlockNum = this->findBlockNumberFromName(prePhysVolume);
      
      
      const G4ThreeVector posPre = step->GetPostStepPoint()->GetPosition();
//      std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers, got position.. " << std::endl;
      
//      std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers, In Lead glass..  " << std::endl;
      const G4ParticleDefinition *def = theTrack->GetParticleDefinition();
      const double mass = def->GetPDGMass();
      const double gamma = theTrack->GetTotalEnergy()/mass;
      const double beta = std::sqrt(std::abs(1.0 - 1./(gamma*gamma)));
      const double cosThetaCer = 1.0/(fRefractionIndex * beta);
//      std::cerr << " TOPAZLGHitAction::SteppingActionEMShowers, In Lead glass..beta  " << beta <<  " cosThetaCer " << cosThetaCer << std::endl;
      if (cosThetaCer >= 1.0) return; // too low energy
      const double sinThetaCerSq = 1.0 - cosThetaCer*cosThetaCer;
      const double numPhot = fNumPhotPerMm*sinThetaCerSq*step->GetStepLength();
      // Now we need to fluctuate, Poisson distrb. 
      
      const double numPhotFluct = G4Poisson(numPhot);
//      std::cerr << " ........... Number of photon emitted  " << numPhot <<  " fluctuated " << numPhotFluct << " over  " << step->GetStepLength() 
//                << " fNumPhotPerMm " << fNumPhotPerMm << std::endl;
      
      if (numPhot < 1 ) return;  // too short path length. 
      const double zFromCathode = std::max((posPost[2] - fZGlassUpstreamFace - fGlassBlockLength), fGlassBlockLength);
      const double attFactorTmp = 1.0 - std::abs(zFromCathode/fGlassBlockLength); 
      const double attFactor = std::max(0.25, attFactorTmp); // Crude model... 
      int numPhotDetected = static_cast<int> (numPhotFluct * attFactor * fKnobAttenuation);
      if (fBlockNum == INT_MAX) return;
      std::vector<sim::TOPAZLGHit>::iterator itSel = fTOPAZLGHits.end();
      for (std::vector<sim::TOPAZLGHit>::iterator it = fTOPAZLGHits.begin(); it != fTOPAZLGHits.end(); it++) {
        if (fBlockNum == it->GetBlockNumber()) { itSel = it; break; } 
      }
      if (itSel == fTOPAZLGHits.end()) {
//        std::cerr << " Block " << fBlockNum << " is clean, first hit " << std::endl;
        sim::TOPAZLGHit aHit;
	aHit.Reset();
	aHit.SetBlockNumber(fBlockNum);
	fTOPAZLGHits.push_back(aHit);
	itSel = fTOPAZLGHits.end();
	itSel--;
      }
//      std::cerr << " Block " << itSel->GetBlockNumber() << " finally, add  " 
//               << numPhotDetected <<  " detected photons " << " steplength " << step->GetStepLength() << std::endl;
//      std::cerr << " Current number of  Block with photons " <<   fTOPAZLGHits.size() << std::endl;    
      const double xMid = 0.5*(posPost[0] + posPre[0]); const double yMid = 0.5*(posPost[1] + posPre[1]); 
      const double zMid = 0.5*(posPost[2] + posPre[2]) - fZGlassUpstreamFace;
      const double time = theTrack->GetLocalTime() + 0.5 * step->GetDeltaTime() - 4000./33. ; // the last time is some arbitrary trigger offset. 
      itSel->AddSomePhotons( numPhotDetected, xMid, yMid, zMid, time);
           
  }// end of TOPAZLGHitAction::SteppingAction


  //------------------------------------------------------------------------------
  // There's one last thing to do: All the particles have their
  // parent IDs set (in PostTrackingAction), but we haven't set the
  // daughters yet.  That's done in this method.
  void TOPAZLGHitAction::EndOfEventAction(const G4Event*)
  {
//    std::cerr << " TOPAZLGHitAction::EndOfEventAction Number of lead glass blocks with photons " << fTOPAZLGHits.size() << std::endl;
//    fFOutStudy1 << " evt blkNum xm ym zm nSteps nPhot Edep EdepFrac " << std::endl;
    int nPhotTotal = 0;
    double eDepTotal = 0.;
    int nStepTotal = 0;
    for (std::vector<sim::TOPAZLGHit>::iterator it = fTOPAZLGHits.begin(); it != fTOPAZLGHits.end(); it++) {
      if (it->GetNumPhot() < 1) continue;
      it->FinalizeAndCalibrate(fKnobCalibration);
      nPhotTotal += it->GetNumPhot(); eDepTotal += it->GetEDeposited(); nStepTotal += it->GetNumStep();
    }
    for (std::vector<sim::TOPAZLGHit>::iterator it = fTOPAZLGHits.begin(); it != fTOPAZLGHits.end(); it++) {
     if (it->GetNumPhot() < 1) continue;
     fFOutStudy1 << " " << fEvtNum << " " << it->GetBlockNumber() << " " << it->GetMeanX() << " " <<  it->GetMeanY() 
                  << " " <<  it->GetMeanZ() << " " << it->GetNumStep() << " " 
		  << it->GetNumPhot() << " " << it->GetEDeposited() << " " << it->GetEDeposited()/eDepTotal << std::endl;
    }
    fFOutStudy1 << " " << fEvtNum << " 9 0. 0. 0. " <<  nStepTotal << " " << nPhotTotal << " " << eDepTotal << " 1. " << std::endl;
     
  }

  int TOPAZLGHitAction::findBlockNumberFromName (const G4VPhysicalVolume *pVol) {
  
      std::string pVolName = pVol->GetName();
      if (pVolName.find("LG_block") == 0 ) {
        std::string blockNumStr = pVolName.substr(8, 2);
        return atoi(blockNumStr.c_str());
      }
      if (pVolName.find("LG_glass") == 0 ) {
        std::string blockNumStr = pVolName.substr(8, 2);
        return atoi(blockNumStr.c_str());
      }
      return INT_MAX;
   }
}//end namespace
