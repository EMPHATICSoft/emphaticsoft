////////////////////////////////////////////////////////////////////////
/// \brief Interface for calculating the "ultimate mother" of a particle in a simulated event.
///
/// \author  seligman@nevis.columbia.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "Simulation/EveIdCalculator.h"

#include "Simulation/ParticleHistory.h"
#include "Simulation/ParticleNavigator.h"

#include "cetlib_except/exception.h"

namespace sim {

  // Constructor. Keep this empty, since users who override with their
  // class may forget to call this constructor.
  EveIdCalculator::EveIdCalculator()
  {}

  // Destructor.
  EveIdCalculator::~EveIdCalculator()
  {}
  

  // Initialization.
  void EveIdCalculator::Init( const sim::ParticleNavigator* nav )
  {
    // Save the ParticleNavigator associated with this simulated chain of
    // particles.
    m_particleNav = nav;

    // Reset the results of previous calculations.
    m_previousNav.clear();
  }


  int EveIdCalculator::CalculateEveId( const int trackID )
  {
    // Look to see if the eve ID has been previously calculated for
    // this track.
    m_previousNav_ptr search = m_previousNav.find( trackID );
    if ( search == m_previousNav.end() ){
      // It hasn't been calculated before. Do the full eve ID
      // calculation.
      int eveID = DoCalculateEveId( trackID );
      
      // Save the result of the calculation.
      m_previousNav[ trackID ] = eveID;
      
      return eveID;
    }

    // If we get here, we've calculated the eve ID for this track
    // before. Return that result.
    return (*search).second;
  }


  int EveIdCalculator::DoCalculateEveId( const int trackID )
  {
    // This is the default eve ID calculation method. It gets called
    // if the user doesn't override it with their own method.

    // Almost any eve ID calculation will use this: Get the entire
    // history of the particle and its ancestors in the simulated
    // event.
    sim::ParticleHistory particleHistory( m_particleNav, trackID );

    if ( particleHistory.empty() ){
      // Something went wrong; most likely the track ID isn't
      // present in the event.
      throw cet::exception("EveIdCalculator")
        << "particle history for track id: " << trackID
        << " is empty, that can't be good\n"
        << __FILE__ << ":" << __LINE__ << "\n";
    }

    // Return the primary particle from the event generator associated
    // with this track ID.
    const Particle* particle = particleHistory[0];
    return particle->TrackId();
  }

}
