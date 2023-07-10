////////////////////////////////////////////////////////////////////////
/// \brief   A class to describe a MCParticle created in the simulation
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "SimulationBase/MCParticle.h"

#include <climits>

namespace simb
{

  const int MCParticle::s_uninitialized = std::numeric_limits<int>::min();

  //...................................................................
  MCParticle::MCParticle()
  {
  }
  
  //...................................................................
  /// Standard constructor.
  MCParticle::MCParticle(const int trackId, 
                     const int pdg,
                     const std::string process,
                     const int mother,
                     const double mass,
                     const int status)
    : caf::SRParticle(trackId, pdg, process, mother, mass, status)
  {
  }

  //...................................................................
  MCParticle::~MCParticle() 
  {std::cerr << "Destructing simb::MCParticle\n";
  }

}
