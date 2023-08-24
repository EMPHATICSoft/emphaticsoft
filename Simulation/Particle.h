////////////////////////////////////////////////////////////////////////
/// \brief   A class to describe a particle created in the simulation
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_PARTICLE_H
#define SIM_PARTICLE_H

#include <vector>
#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCTruth.h"

namespace sim {
  /// A single unit of energy deposition in the liquid scintillator
  class Particle : public simb::MCParticle {
  public:
    /// An indicator for an uninitialized variable (see Particle.cxx).
    static const int s_uninitialized; //! Don't write this as ROOT output
    
    Particle();
    Particle(const int trackId, const int pdg,
	     const std::string process,
	     const int mother = -1,
	     const double mass = s_uninitialized,
	     const int status = 1);
    
    ~Particle();
    
  };
  
}
#endif
////////////////////////////////////////////////////////////////////////
