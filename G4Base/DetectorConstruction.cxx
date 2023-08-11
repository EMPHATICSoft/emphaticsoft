////////////////////////////////////////////////////////////////////////
/// \file  DetectorConstruction.cxx
/// \brief Build Geant4 geometry from GDML
///
/// \version $Id: DetectorConstruction.cxx,v 1.10 2012-12-03 23:29:49 rhatcher Exp $
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"

#include "G4Base/DetectorConstruction.h"
#include "G4Base/EmphMisaligner.h"
#include "G4Base/GlobalMagneticField.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4GDMLParser.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4UniformMagField.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4Mag_UsualEqRhs.hh"
#include "Geant4/G4MagIntegratorStepper.hh"
#include "Geant4/G4ClassicalRK4.hh"
#include "Geant4/G4ChordFinder.hh"
#include "Geant4/G4PropagatorInField.hh"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace CLHEP;

namespace g4b{

  // Allocate static variables.
  G4VPhysicalVolume* DetectorConstruction::fWorld    = nullptr;
  G4FieldManager*    DetectorConstruction::fFieldMgr = nullptr;

  //---------------------------------------------------
  // Constructor
  DetectorConstruction::DetectorConstruction(std::string const& gdmlFile,
                                             bool        const& overlapCheck,
                                             bool        const& validateSchema, 
					     int misalignModelNum, unsigned int misAlignSeed, double misalignDoubleSSDGap)
  {
    if ( gdmlFile.empty() ) {
      throw cet::exception("DetectorConstruction") << "Supplied GDML filename is empty\n"
						   << __FILE__ << ":" << __LINE__ << "\n";
    }
    std::cerr << "DetectorConstruction::DetectorConstruction, begin with gdmlFile " 
              <<  gdmlFile << " with misalignModelNum " << misalignModelNum << std::endl;
    // Get the path to the GDML file from the Geometry interface.
    const G4String GDMLfile = static_cast<const G4String>( gdmlFile );

    // Use Geant4's GDML parser to convert the geometry to Geant4 format.
    G4GDMLParser parser;
    parser.SetOverlapCheck(overlapCheck);
    if (overlapCheck) std::cerr << " ... We will check for eventual volumes overlaps.. Good " << std::endl;
    else std::cerr << " ... We will NOT check for eventual volumes overlaps.. Results will be suspicious " << std::endl;
    std::string effGDMLFile(gdmlFile); // effective geometry file name.
    if ((misalignModelNum != 0)  || (std::abs(misalignDoubleSSDGap) < 1.0e-3)) {
      std::cerr << " We will modify the G4 Geometry to implement quasi-realistic misalignments, model number " 
                << misalignModelNum << std::endl;
		
// 
// Previous code, 
//      g4b::EmphMisaligner myMis(gdmlFile, misAlignSeed);
//      myMis.doIt(misalignModelNum, misalignDoubleSSDGap);
//      std::ostringstream misStrStr; misStrStr << "./MisAlignedEmphPhase1x_" 
//                                                << misalignModelNum << "_" << misAlignSeed << ".gdml";
//      std::string misStr(misStrStr.str());
//      myMis.writeIt(misStr.c_str());
//      std::ostringstream misStrStr2; misStrStr2 << "./MisAlignedEmphPhase1xPred_" 
//                                                << misalignModelNum << "_" << misAlignSeed << ".txt";
//      std::string misStr2(misStrStr2.str());
//      myMis.dumpRawMisAlignParams(misStr2.c_str());
//      effGDMLFile = misStr;
//      
//     We now create a new version of the perl script itself and run it from here.
//  
       g4b::EmphMisaligner myMis(std::string(""), misAlignSeed);
       
       myMis.doIt(misalignModelNum, misalignDoubleSSDGap);
       std::cerr << "   .............. done it ... now run the perl script.. " << std::endl;   
       effGDMLFile = myMis.runIt(std::string("Tr1")); 
       std::cerr << "   .............. ran the perl script..gdml file path is " << effGDMLFile << std::endl;
//                 << std::endl <<  " .................. And quit now.. " << std::endl; exit(2);
    }
    parser.Read(effGDMLFile,validateSchema);
//    std::cerr << " Geometry created, we stop here " << std::endl; exit(2);
    // Fetch the world physical volume from the parser.  This contains
    // the entire detector, not just the outline of the experimental
    // hall.
    fWorld = parser.GetWorldVolume();
    
  }
  
  //---------------------------------------------------
  // Destructor.
  DetectorConstruction::~DetectorConstruction() 
  {
  }
  
  //---------------------------------------------------
  G4VPhysicalVolume* DetectorConstruction::Construct()
  {
    // Setup the magnetic field situation 
    art::ServiceHandle<emph::MagneticFieldService> bField;
    auto magField = bField->Field(); //get the provider
    
    G4FieldManager* fieldMgr
      = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    fieldMgr->SetDetectorField(magField);
    G4Mag_UsualEqRhs *fEquation = new G4Mag_UsualEqRhs(magField);
    
    G4MagIntegratorStepper *pStepper = new G4ClassicalRK4 (fEquation);
    
    G4ChordFinder *pChordFinder = 
      new G4ChordFinder(magField, 1.e-1*mm, pStepper);
    pChordFinder->SetDeltaChord(1.0e-3*mm);
    fieldMgr->SetChordFinder(pChordFinder);
    fieldMgr->SetDeltaOneStep(1.0e-3*mm);
    fieldMgr->SetDeltaIntersection(1.0e-4*mm);
    G4PropagatorInField* fieldPropagator
      = G4TransportationManager::GetTransportationManager()
      ->GetPropagatorInField();
    fieldPropagator->SetMinimumEpsilonStep(1.e-5*mm);
    fieldPropagator->SetMaximumEpsilonStep(1.e-2*mm);
    /*
    // loop over the possible fields
    for(auto fd : pProvider->Fields()){
      switch (fd.fMode) {
      case mag::kNoBFieldMode:
      break;
      case mag::kConstantBFieldMode: {
      // Attach this to the magnetized volume only, so get that volume
      G4LogicalVolume *bvol = G4LogicalVolumeStore::GetInstance()->GetVolume(fd.fVolume);
          
          // Define the basic field, using p we should get the uniform field
          G4UniformMagField* magField = new G4UniformMagField( fd.fField * CLHEP::tesla );
          fFieldMgr = new G4FieldManager();
          fFieldMgr->SetDetectorField(magField);
          fFieldMgr->CreateChordFinder(magField);
          
          MF_LOG_INFO("DetectorConstruction")
          << "Setting uniform magnetic field to be "
          << magField->GetConstantFieldValue().x() << " "
          << magField->GetConstantFieldValue().y() << " "
          << magField->GetConstantFieldValue().z() << " "
          << " in " << bvol->GetName();
          
          // Reset the chord finding accuracy
          // fFieldMgr->GetChordFinder()->SetDeltaChord(1.0 * cm);
          
          // the boolean tells the field manager to use local volume
          bvol->SetFieldManager(fFieldMgr, true);
          
          break;
        } // case mag::kConstantBFieldMode
        case mag::kFieldRZMapMode: {
 
          // Attach this to the magnetized volume only, so get that volume
          G4LogicalVolume *bvol = G4LogicalVolumeStore::GetInstance()->GetVolume(fd.fVolume);

          mag::GlobalMagneticField *magField = new mag::GlobalMagneticField();
          fFieldMgr = new G4FieldManager();
          fFieldMgr->SetDetectorField(magField);
          fFieldMgr->CreateChordFinder(magField);

          MF_LOG_INFO("DetectorConstruction")
          << "Setting magnetic field in kFieldRZMapMode to be"
          << " in " << bvol->GetName();

          // the boolean tells the field manager to use local volume
          bvol->SetFieldManager(fFieldMgr, true);

          break;
        } // case mag::kFieldRZMapMode
        case mag::kFieldXYZMapMode: {

          // Attach this to the magnetized volume only, so get that volume
          G4LogicalVolume *bvol = G4LogicalVolumeStore::GetInstance()->GetVolume(fd.fVolume);

          mag::GlobalMagneticField *magField = new mag::GlobalMagneticField();
          fFieldMgr = new G4FieldManager();
          fFieldMgr->SetDetectorField(magField);
          fFieldMgr->CreateChordFinder(magField);
  
          MF_LOG_INFO("DetectorConstruction")
          << "Setting magnetic field in kFieldXYZMapMode to be"
          << " in " << bvol->GetName();

           // the boolean tells the field manager to use local volume
          bvol->SetFieldManager(fFieldMgr, true);
          
          break;
        } // case mag::kFieldXYZMapMode
        default: // Complain if the user asks for something not handled
          MF_LOG_ERROR("DetectorConstruction")
          << "Unknown or illegal Magneticfield "
          << "mode specified: " 
          << fd.fMode
          << ". Note that AutomaticBFieldMode is reserved.";
          break;
      } // end switch cases
      
      } // end loop over fields
    */
    //
    // Align the magnet..
    
    art::ServiceHandle<emph::geo::GeometryService> geomService;
    const emph::geo::Geometry* theGeo = geomService->Geo();
    magField->G4GeomAlignIt(theGeo);
    
    return fWorld;
  }

}// namespace
