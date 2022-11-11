////////////////////////////////////////////////////////////////////////
/// \file  DetectorConstruction.h
/// \brief Build Geant4 geometry from GDML
///
/// \version $Id: DetectorConstruction.h,v 1.5 2012-12-03 23:29:49 rhatcher Exp $
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef G4BASE_DetectorConstruction_h
#define G4BASE_DetectorConstruction_h

#include "Geant4/G4VUserDetectorConstruction.hh"

// forward declarations
class G4VPhysicalVolume;
class G4FieldManager;

namespace g4b {

  class DetectorConstruction : public G4VUserDetectorConstruction {

  public:
    /// Standard constructor and destructor.
    explicit DetectorConstruction(std::string const& gdmlFile,
                                  bool        const& overlapCheck   = false,
                                  bool        const& validateSchema = true);
    virtual ~DetectorConstruction();

    /// The key method in this class; returns the Geant4 version of
    /// the detector geometry.  Required of any class that inherits
    /// from G4VUserDetectorConstruction.
    G4VPhysicalVolume* Construct();

    ///Set a custom Magnetic service
    void SetCustomMagneticService();

    /// Return a pointer to the world volume.  This returns the same
    /// pointer as Construct(), but Construct() also does other
    /// actions such as setting up sensitive detectors.  World() just
    /// returns the pointer to the top physical volume.
    static G4VPhysicalVolume* GetWorld() { return fWorld;    }

    /// Magnetic field
    static G4FieldManager* GetFieldMgr() { return fFieldMgr; }

  private:
    static G4VPhysicalVolume* fWorld;    ///< pointer to the world volume
    static G4FieldManager*    fFieldMgr; ///< pointer to the field manager

  };

} // namespace g4b

#endif // G4BASE_DetectorConstruction_h
