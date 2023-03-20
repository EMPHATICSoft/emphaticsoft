////////////////////////////////////////////////////////////////////////
/// \file  SSDHitAction.cxx
/// \brief Use Geant4's user "hooks" to maintain a list of SSDHits
///
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "G4EMPH/SSDHitAction.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/SSDHit.h"
#include "Simulation/SSDHitAlgo1.h"

#include <vector>
#include <map>
#include <cstdlib>

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
#include "Geant4/G4TouchableHandle.hh"
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
    fFOutStudy1 << " evt track pId stNum plNum x y z xl yl zl px py pz  " << std::endl;
  }

  //-------------------------------------------------------------
  void SSDHitAction::BeginOfEventAction(const G4Event*)
  {
    // Clear any previous particle information.
    fSSDHits.clear();
    fSSDHitsAlgo1.clear();

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
    const double Q = track->GetDynamicParticle()->GetCharge();
    if (std::abs(Q) < 0.5) return;
  
    const G4ThreeVector &pos0 = step->GetPreStepPoint()->GetPosition(); // Start of the step
    const G4ThreeVector &pos  = step->GetPostStepPoint()->GetPosition();                   // End of the step
    const CLHEP::Hep3Vector &mom  = track->GetMomentum();
    
//    MF_LOG_DEBUG("SSDHitAction") << " momentum = "
//				 << mom.x() << " " << mom.y() << " " 
//				 << mom.z() << " " << mom.mag();
    
    double tpos0[3] = {pos0.x()/CLHEP::mm, pos0.y()/CLHEP::mm, pos0.z()/CLHEP::mm}; ///< Start of the step
    double tpos1[3] = {pos.x()/CLHEP::mm , pos.y()/CLHEP::mm , pos.z()/CLHEP::mm};  ///< End of the step

    // If it's a null step, don't use it. Otherwise it may induce an additional 
    // SSDHit, which is wrong
    
    if(tpos0[0]==tpos1[0] &&
       tpos0[1]==tpos1[1] &&
       tpos0[2]==tpos1[2])return;

    double mom0[3] = {mom.x()/CLHEP::GeV, mom.y()/CLHEP::GeV, mom.z()/CLHEP::GeV};
    
    // We should probably put some logic in to stop tracking if we know the
    // particle won't hit anything else in the spectrometer.  Eg,
    /*
      if(ParticleIsHeadedOutsideOfSpectrometer)
      track->SetTrackStatus(fStopAndKill);
      // This is done in another dedicated User Action, FastStopAction. 
    */
    
    //check that we are in the correct material to record a hit - ie scintillator
    std::string material = track->GetMaterial()->GetName();
    if(material.compare("SiliconWafer") != 0 ) {
      return;
    }
//    std::cerr << " SSDHitAction::SteppingAction, entering silicon waver.. z in " << tpos0[2] << " z Out " << tpos1[2]
//              << " Q " << Q << std::endl;
    
    
    //    const double edep = step->GetTotalEnergyDeposit()/CLHEP::GeV;
    //
    // Convert to Local coordinates. 
    //
    G4TouchableHandle theTouchable = step->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector localPos0 = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos0); //entrance
    G4ThreeVector localPos1 = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos); //exit
    const G4VPhysicalVolume *vol0 = step->GetPreStepPoint()->GetPhysicalVolume();
    const std::string volName = vol0->GetName();
    // Get the station number from the parent volume name. 
    //
    // Get the position and time the particle first enters
    // the volume, as well as the pdg code.  that information is
    // obtained from the G4Track object that corresponds to this step
    // account for the fact that we use cm, ns, GeV rather than the G4 defaults
    sim::SSDHit ssdHit;
    
    ssdHit.SetPId( track->GetDefinition()->GetPDGEncoding() );
    ssdHit.SetTrackID( track->GetTrackID() );
    
    // need to add code to figure out SSD plane, sensor and strip
    // Instead, for Algo1, simply the G4 volume name of the sensor, and the Station number.
    //   We'll get to the channel number in the converter. 
    // There is more than one strip, for tracks close to a strip boundary, delta-ray, etc.. 

    /// Add position
    ssdHit.SetX(tpos0);
    ssdHit.SetP(mom0);
    
    sim::SSDHitAlgo1 ssdHitAlgo1;
    ssdHitAlgo1.SetX(tpos0);
    ssdHitAlgo1.SetP(mom0);
    ssdHitAlgo1.SetEDep(step->GetTotalEnergyDeposit());
    ssdHitAlgo1.SetPId( track->GetDefinition()->GetPDGEncoding() );
    ssdHitAlgo1.SetTrackID( track->GetTrackID() );
    ssdHitAlgo1.SetTLocal0(localPos0[1]); // always the y coordinate, by definition 
    ssdHitAlgo1.SetTLocal1(localPos1[1]); // always the y coordinate, by definition 
//    ssdHitAlgo1.SetG4SensorVolName(volName  ); No longer needed. 
    // Tedious.. not my fault ! 
    const G4LogicalVolume *volM0 = vol0->GetMotherLogical();
    const std::string volMName = volM0->GetName();;
    short int aStationNum = 255;
    short int aPlaneNum = 255;   
    const std::string keySingle("ssdStationsingle"); 
    const std::string keyRotate("ssdStationrotate"); 
    const std::string keyDouble("ssdStationdouble");
    size_t iPosSingle = volMName.find(keySingle); size_t iPosRotate = volMName.find(keyRotate);
    size_t iPosDouble = volMName.find(keyDouble);
//    std::cerr << " SSDHitAction::SteppingAction at volume  " << volName << " mother " << volMName << std::endl;
    std::string stationBitStr("?");
    if (iPosSingle != std::string::npos)  {
       stationBitStr = volMName.substr(iPosSingle + keySingle.length(), 1);
       aStationNum = static_cast<short int>(std::atoi(stationBitStr.c_str()));
       if ((volName.find("ssdsensorsingle000") != std::string::npos) || 
           (volName.find("ssdsensorsingle100") != std::string::npos)) aPlaneNum = 0; 
       if ((volName.find("ssdsensorsingle010") != std::string::npos) || 
           (volName.find("ssdsensorsingle110") != std::string::npos)) aPlaneNum = 1; 
    } else if (iPosRotate != std::string::npos)  {
      stationBitStr = volMName.substr(iPosRotate + keyRotate.length(), 1); 
      aStationNum = 2 + static_cast<short int>(std::atoi(stationBitStr.c_str()));
      if ((volName.find("ssdsensorrotate000") != std::string::npos) || 
           (volName.find("ssdsensorrotate100") != std::string::npos)) aPlaneNum = 0; 
      if ((volName.find("ssdsensorrotate010") != std::string::npos) || 
           (volName.find("ssdsensorrotate110") != std::string::npos)) aPlaneNum = 1; 
      if ((volName.find("ssdsensorrotate020") != std::string::npos) || 
           (volName.find("ssdsensorrotate120") != std::string::npos)) aPlaneNum = 2; 
    } else if (iPosDouble != std::string::npos)  {
      stationBitStr = volMName.substr(iPosDouble + keyDouble.length(), 1);
      aStationNum = 4 + static_cast<short int>(std::atoi(stationBitStr.c_str()));
      if ((volName.find("ssdsensordouble000") != std::string::npos) || 
           (volName.find("ssdsensordouble100") != std::string::npos)) aPlaneNum = 0; 
      if ((volName.find("ssdsensordouble001") != std::string::npos) || 
           (volName.find("ssdsensordouble101") != std::string::npos)) aPlaneNum = 1; 
      if ((volName.find("ssdsensordouble010") != std::string::npos) || 
           (volName.find("ssdsensordouble110") != std::string::npos)) aPlaneNum = 2; 
      if ((volName.find("ssdsensordouble011") != std::string::npos) || 
           (volName.find("ssdsensordouble111") != std::string::npos)) aPlaneNum = 3; 
      if ((volName.find("ssdsensordouble020") != std::string::npos) || 
           (volName.find("ssdsensordouble120") != std::string::npos)) aPlaneNum = 4; 
      if ((volName.find("ssdsensordouble021") != std::string::npos) || 
           (volName.find("ssdsensordouble121") != std::string::npos)) aPlaneNum = 5; 
    } 
    if (stationBitStr == std::string("?")) {
      std::cerr << "  SSDHitAction::SteppingAction Unknown Mother volume for sensor " << volName 
                << " is " <<  volMName << " skip.... " << std::endl; return; 
    }
    ssdHitAlgo1.SetStation(aStationNum);
    ssdHitAlgo1.SetPlane(aPlaneNum);
    
//    fSSDHits.push_back(ssdHit);
    fSSDHitsAlgo1.push_back(ssdHitAlgo1);
    fFOutStudy1 << " " << fRunManager->GetCurrentEvent()->GetEventID();
    fFOutStudy1 << " " << track->GetTrackID() << " " << track->GetDefinition()->GetPDGEncoding();
    fFOutStudy1 << " " << aStationNum << " " << aPlaneNum;
    fFOutStudy1 << " " << tpos0[0] << " " << tpos0[1] << " " << tpos0[2];
    fFOutStudy1 << " " << localPos0[0] << " " << localPos0[1] << " " << localPos0[2];
    fFOutStudy1 << " " << mom0[0] << " " << mom0[1] << " " << mom0[2] << std::endl;
    
    
    
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
