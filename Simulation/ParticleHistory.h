////////////////////////////////////////////////////////////////////////
/// \brief   A "chain" of particles associated with production of 
///          a Particle in a ParticleList.
///
/// \author  seligman@nevis.columbia.edu
/// \date
////////////////////////////////////////////////////////////////////////

/// A container for a chain of particles in an event. It's meant as
/// a convenience for looking at a sequence of particles within a
/// sim::ParticeList.

/// Here's an example to illustrate the purpose and use of a
/// list. Assume a pi+ is a primary particle in an event whose decay
/// is modeled like this:

/// TrackID   Particle
///     2       pi+
///   101       nu_mu
///   102       mu+
///   341       nu_mu_bar
///   342       nu_e
///   343       e+   

/// I'm playing around with the ParticleNavigator for the event, and I'm
/// interested in what produced track ID 343, which is an e+. I can
/// use the ParticleHistory class to quickly go through the production
/// chain:

/// sim::ParticleNavigator* particleNav = // ... from somewhere
/// int trackID = 343;
/// const sim::ParticleHistory particleHistory( particleNav, trackID );
/// for ( int i = 0; i != particleHistory.size(); ++i )
/// { 
///   const sim::Particle* particle = particleHistory[i];
///   // ... 
/// }

/// In the above example:
/// particleHistory.size() == 3
/// particleHistory[0] points to the particle with track ID 2
/// particleHistory[1] points to the particle with track ID 102
/// particleHistory[2] points to the particle with track ID 343

/// So as you go through a ParticleHistory "array," the first element
/// is a primary particle in the event, and the last element is the
/// particle you used to create the history.

/// ParticleHistory looks like a vector< const sim::Particle* >, with the
/// following additions:

/// - a ParticleNavigator() method that returns a ParticleNavigator* to the
///   object that's associated with the history.

/// - an EndParticleID() method that returns the track ID of the last
///   particle in the chain; that is, it's the second argument in the
///   constructor.

/// - Print() and operator<< methods for ROOT display and ease of
///   debugging.

/// TECHNICAL NOTES:

/// ParticleHistory behaves mostly like a vector, but it's actually a
/// deque. This means that you can't assume that &particleHistory[0]
/// is a continugous array of Particle*. If those two sentences mean
/// nothing to you, don't worry about it; this only matters to folks
/// familiar with STL.

/// A given ParticleHistory object is associated with the ParticleNavigator
/// used to create it. If you delete the ParticleNavigator (by reading in a
/// new event, for example) then the contents of the corresponding
/// ParticleHistory object(s) are garbage.

/// If you create a ParticleHistory object like this:
///    const sim::ParticleHistory ph(particleNav,1123);
/// and there is no track 1123 in the particle list, then ph.size()==0.

/// particleHistory[0] is not necessarily a primary particle in the
/// event. It's possible for a production chain to be broken due to
/// simulation cuts. The first element just represents as far back we
/// can go in the production chain given the ParticleNavigator.
#ifndef SIM_PARTICLEHISTORY_H
#define SIM_PARTICLEHISTORY_H

#include <deque>
#include <iostream>

#include "Simulation/Particle.h"

namespace sim 
{
  // Forward declaration
  class ParticleNavigator;
  
  class ParticleHistory : public std::deque< const sim::Particle* >
  {
  public:
    
    // Constructor and destructor
    ParticleHistory( const sim::ParticleNavigator* nav, const int trackID );
    virtual ~ParticleHistory();
    
    // For which particle was this history generated?
    int EndParticleID() const { return m_trackID; }
    
    // With which ParticleNavigator is this history associated?
    const sim::ParticleNavigator* ParticleNavigator() const { return m_particleNav; }
    
    // Provide a ROOT-compatible print method. (However, this class
    // does not inherit from TObject, so it can't be read or written
    // via ROOT I/O.)
    virtual void Print() const;
    friend std::ostream& operator<< ( std::ostream& output, const ParticleHistory& );
    
  private:
    const sim::ParticleNavigator* m_particleNav; ///< The ParticleNavigator associated with this chain.
    int m_trackID;                                ///< The particle for which a history was created.
  };
  
} // namespace sim

#endif // SIM_PARTICLEHISTORY_H
/////////////////////////////////////////////////////////////////////////////
