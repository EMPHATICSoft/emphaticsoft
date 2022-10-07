////////////////////////////////////////////////////////////////////////
/// \file  G4Helper.h
/// \brief Use Geant4 to run the detector simulation
///
/// \version $Id: G4Helper.h,v 1.15 2012-12-03 23:29:50 rhatcher Exp $
/// \author  seligman@nevis.columbia.edu, brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This object has the following functions:
///
/// - Initialize Geant4 physics, detector geometry, and other
///   processing.
///
/// - Pass the primary particles to the Geant4 simulation to calculate
///   "truth" information for the detector response.
///

#ifndef G4BASE_G4HELPER_H
#define G4BASE_G4HELPER_H

#include "G4Base/ConvertMCTruthToG4.h"

#include <cstring>

// ART Includes
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"

#include "Geant4/G4RunManager.hh"
#include "Geant4/G4VUserParallelWorld.hh"

// Forward declarations
class G4UImanager;

namespace simb{ class MCTruth;      }
namespace rndm { class ParticleList; }

///basic interface to Geant4 for ART-based software
namespace g4b {  

  // Forward declarations within namespace.
  class ParticleListAction;
  class ConvertPrimaryToGeant4;
  class DetectorConstruction;

  class G4Helper {

  public:

    /// Standard constructor and destructor for an FMWK module.
    G4Helper();
    G4Helper(std::string const& g4macropath, 
             std::string const& g4physicslist = "QGSP_BERT",
             std::string const& gdmlFile = "");
    virtual ~G4Helper();

    // have to call this before InitPhysics if you want to load in
    // parallel worlds.  G4Helper takes over ownership
    void SetParallelWorlds(std::vector<G4VUserParallelWorld*> pworlds);

    // Call this method to set a step size limit in the chosen volume
    // It must be called before InitPhysics (which calls SetPhysicsList)
    // so that the physics list will know to register a step limiter
    void SetVolumeStepLimit(std::string const& volumeName,
                            double             maxStepSize);
    
    // extra control over how GDML is parsed
    inline void SetOverlapCheck(bool check);
    inline void SetValidateGDMLSchema(bool validate);

    // have to call this before InitPhysics if you want to control
    // when the detector is constructed, useful if you need to 
    // muck with G4LogicalVolumes
    // if the fDetector pointer is null when InitMC is called
    // it will just construct the fDetector
    void ConstructDetector(std::string const& gdmlFile);

    // Initialization for the Geant4 Monte Carlo, called before the
    // first event is simulated.  InitPhysics gets the G4 physics 
    // initialized, and the UserPrimaryGeneratorAction (ConvertMCTruthToG4).
    // SetUserAction hands the UserActionManager over (so call it
    // after it is fully configured) to the RunManager; it also
    // runs the initial command macro and completes the initialization.
    // These two should be called in this order with any UserActionManager
    // configuration in between.
    void InitPhysics();
    void SetUserAction();

    // This is the method that actually passes a list of MCTruth objects to G4 
    // so it can create a list of particles
    bool G4Run(std::vector<const simb::MCTruth*> &primaries);

    // Pass a single MCTruth object to G4
    bool G4Run(art::Ptr<simb::MCTruth>& primary);

    // Pass a single MCTruth object to G4
    bool G4Run(const simb::MCTruth* primary);

    G4RunManager* GetRunManager() { return fRunManager; }

  protected:

    void SetPhysicsList(std::string physicsList);

    // These variables are "protected" rather than private, because I
    // can forsee that it may be desirable to derive other simulation
    // routines from this one.
    std::string                        fG4MacroPath;        ///< Full directory path for Geant4 macro file
                          	                                ///< to be executed before main MC processing.
    std::string           	           fG4PhysListName;     ///< Name of physics list to use
    std::string                        fGDMLFile;           ///< Name of the gdml file containing the detector Geometry
    bool                               fCheckOverlaps;      ///< Have G4GDML check for overlaps?
    bool                               fValidateGDMLSchema; ///< Have G4GDML validate geometry schema?
    bool                               fUseStepLimits;      ///< Set in SetVolumeStepLimit

    G4RunManager*         	           fRunManager;         ///< Geant4's run manager.
    G4UImanager*          	           fUIManager;          ///< Geant4's user-interface manager.
    ConvertMCTruthToG4*   	           fConvertMCTruth;     ///< Converts MCTruth objects;
                                                            ///< Geant4 event generator.
    DetectorConstruction* 	           fDetector;           ///< DetectorConstruction object
    std::vector<G4VUserParallelWorld*> fParallelWorlds;     ///< list of parallel worlds
  };

} // namespace g4b

#ifndef __GCCXML__
inline void g4b::G4Helper::SetOverlapCheck(bool check)          { fCheckOverlaps      = check;    }
inline void g4b::G4Helper::SetValidateGDMLSchema(bool validate) { fValidateGDMLSchema = validate; }
#endif


#endif // G4BASE_G4HELPER_H
