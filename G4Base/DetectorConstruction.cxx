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
                                             bool        const& validateSchema)
  {
    if ( gdmlFile.empty() ) {
      throw cet::exception("DetectorConstruction") << "Supplied GDML filename is empty\n"
						   << __FILE__ << ":" << __LINE__ << "\n";
    }
    // Get the path to the GDML file from the Geometry interface.
    const G4String GDMLfile = static_cast<const G4String>( gdmlFile );

    // Use Geant4's GDML parser to convert the geometry to Geant4 format.
    G4GDMLParser parser;
    parser.SetOverlapCheck(overlapCheck);
    parser.Read(GDMLfile,validateSchema);

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
    //    art::ServiceHandle<emph::MagneticFieldService> bField;
    //    auto magField = bField->Field(); //get the provider
    mag::GlobalMagneticField* magField = new mag::GlobalMagneticField();

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
    
    return fWorld;
  }

}// namespace
