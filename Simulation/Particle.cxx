////////////////////////////////////////////////////////////////////////
/// \brief   A class to describe a particle created in the simulation
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "Simulation/Particle.h"

#include <climits>

namespace sim
{

  const int Particle::s_uninitialized = std::numeric_limits<int>::min();

  //...................................................................
  Particle::Particle()
  {
  }
  
  //...................................................................
  /// Standard constructor.
  Particle::Particle(const int trackId, 
                     const int pdg,
                     const std::string process,
                     const int mother,
                     const double mass,
                     const int status)
    : caf::SRParticle(trackId, pdg, process, mother, mass, status)
  {
  }

  //...................................................................
  /*Particle::~Particle(): 
  {}
*/
}
