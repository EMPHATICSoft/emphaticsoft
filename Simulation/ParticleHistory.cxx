////////////////////////////////////////////////////////////////////////
/// \brief   A "chain" of particles associated with production of 
///          a Particle in a ParticleNavigator.
///
/// \author  seligman@nevis.columbia.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "Simulation/ParticleHistory.h"

#include <cmath>

#include "Simulation/ParticleNavigator.h"

namespace sim 
{
  // Nothing special need be done for the constructor.
  ParticleHistory::ParticleHistory( const sim::ParticleNavigator* nav, const int trackID ) 
    : m_particleNav(nav)
    , m_trackID(trackID)
  {
    // Look for the track in the particle list.
    ParticleNavigator::const_iterator search = m_particleNav->find( m_trackID );
    
    // While we're still finding particles in the chain...
    while ( search != m_particleNav->end() )
      {
	const sim::Particle* particle = (*search).second;
	push_front( particle );
	
	// If this is a primary particle, we're done.
	int trackID = particle->TrackId();
	if ( m_particleNav->IsPrimary( trackID ) ) break;
	
	// Now look for the parent of this particle.
	int parentID = particle->Mother();
	search = m_particleNav->find( parentID );
	
      } // while we're finding particles in the chain
  }
  
  //......................................................................
  ParticleHistory::~ParticleHistory()
  {
  }
  
  //......................................................................
  void ParticleHistory::Print() const 
  { 
    std::cout << (*this); 
  }
  
  //......................................................................
  std::ostream& operator<< ( std::ostream& output, const ParticleHistory& list )
  {
    // Determine a field width for the particle number.
    ParticleHistory::size_type numberOfParticles = list.size();
    int numberOfDigits = (int) std::log10( (double) numberOfParticles ) + 1;
    
    // A simple header.
    output.width( numberOfDigits );
    output << "#" << ": < ID, particle >" << std::endl; 
    
    // Write each particle on a separate line.
    ParticleHistory::size_type nParticle = 0;
    for ( ParticleHistory::const_iterator particle = list.begin(); particle != list.end(); ++particle, ++nParticle ){
      output.width( numberOfDigits );
      output << nParticle << ": " 
	     << *(*particle)
	     << std::endl;
    }
    
    return output;
  }
  
} // end namespace sim
//////////////////////////////////////////////////////////////////////////////
