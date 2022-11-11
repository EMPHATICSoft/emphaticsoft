//
//  G4 interface for full magnetic field.
//
//
// Original author Eldwan Brianne 
//

// C++ includes
#include <iostream>

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "G4Base/GlobalMagneticField.h"
//#include "MagneticField/MagneticFieldService.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"

using CLHEP::Hep3Vector;
using namespace std;

namespace mag {

  GlobalMagneticField::GlobalMagneticField()
  {
    art::ServiceHandle<emph::MagneticFieldService> bField;
    _bFieldHandle = &*bField;
  }

  // This is the entry point called by G4.
  void GlobalMagneticField::GetFieldValue(const G4double Point[4], G4double *Bfield) const {

    //Check provider units (geant4 uses mm, your framework can use another unit...)
    const auto pProvider = _bFieldHandle->Field();

    //    G4ThreeVector point(Point[0], Point[1], Point[2]);//should be in mm
    double field[3];
    pProvider->GetFieldValue(Point,field);
    //    const G4ThreeVector field = pProvider->FieldAtPoint(point);//should return tesla

    Bfield[0] = field[0]; //.x();
    Bfield[1] = field[1]; //.y();
    Bfield[2] = field[2]; //.z();

    // std::cout << "Returned field " << Bfield[0] << ", " << Bfield[1] << ", " << Bfield[2] << std::endl;

    return;
  }

} // end namespace mag
