////////////////////////////////////////////////////////////////////////
/// \file G4Alg.h
//
/// \version $Id: G4Alg.h,v 1.3 2012-09-13 15:23:13 brebel Exp $
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef G4NOVA_G4ALG_H
#define G4NOVA_G4ALG_H

// ART includes
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/Ptr.h"

namespace art { 
  class Event; 
  class ParameterSet;
}
namespace g4b{
    class G4Helper;
}
namespace simb{
    class MCTruth;
}
namespace sim{
    class SSDHit;
//    class Particle;
    class Track;
}

namespace emph {

  /// An algorithm to pass interaction information to Geant4 and create hits and particle lists
  class G4Alg {
  public:
    explicit G4Alg(fhicl::ParameterSet const &pset);
    virtual ~G4Alg();                        

    void RunGeant(std::vector< art::Handle< std::vector<simb::MCTruth> > >& mclists,
                  std::vector<sim::SSDHit> & ssdhitlist,
                  std::vector< sim::Track >& tracklist,
                  std::vector< std::vector<std::pair<size_t, size_t> > >&   pListLimits);
    
    void RunGeant(std::vector< const simb::MCTruth* >& mctruths,
                  std::vector<sim::SSDHit> & flshitlist,
                  std::vector< sim::Track >& tracklist,
                  std::map<int, size_t >& trackIDToMCTruthIndex);
    
    void RunGeant(art::Ptr<simb::MCTruth> mctruth,
                  std::vector<sim::SSDHit> & flshitlist,
                  std::vector< sim::Track >& tracklist,
                  int trackIDOffset=-1);

    void RunGeant(const simb::MCTruth* mctruth,
                  std::vector<sim::SSDHit> & ssdhitlist,
                  std::vector< sim::Track >& tracklist,
                  int trackIDOffset=-1);

    bool IsAborted();
    
  private:
    
    void ConfigUserActionManager(std::vector<std::string> const& actionList,
                                 fhicl::ParameterSet      const& pset);

    g4b::G4Helper*  fG4Help;              ///< G4Helper object
    std::string     fG4MacroPath;         ///< path to G4 macro
    std::string     fG4PhysListName;      ///< name of G4 physics list to use
    double          fEnergyThresh;        ///< threshold for stopping the tracking of a particle in GeV
    bool            fManyParticles;       ///< if true, keep individual track ids from processes like compt and brem
    bool            fSparseTrajectories;  ///< if true, only save necessary points in particle trajectories
    std::string     fGenModuleLabel;      ///< label of module that made the particles to track

    int             fPlaIndex;            ///< index of the ParticleListAction in the UserActionManager
    int             fShaIndex;            ///< index of the SSDHitAction in the UserActionManager

    std::vector<std::string> fUserActions;///< UserAction classes 

  };
}

#endif // G4NOVA_G4ALG_H
////////////////////////////////////////////////////////////////////////
