#ifndef GLOBALMAGNETICFIELD_H
#define GLOBALMAGNETICFIELD_H
//
// G4 interface for full magnetic field.
//
//
// Original author Eldwan Brianne 
//

#include <string>

#include "Geant4/G4MagneticField.hh"
#include "Geant4/G4Types.hh"
#include "Geant4/G4ThreeVector.hh"
#include "MagneticField/service/MagneticFieldService.h"

namespace mag {

  //  class emph::MagneticFieldService;

  class GlobalMagneticField: public G4MagneticField {

  public:

    explicit GlobalMagneticField();
    virtual ~GlobalMagneticField(){};

    // This is called by G4.
    virtual void GetFieldValue(const G4double Point[4], G4double *Bfield) const;

  private:

    const emph::MagneticFieldService* _bFieldHandle;
  };
}
#endif /* GLOBALMAGNETICFIELD_H */
