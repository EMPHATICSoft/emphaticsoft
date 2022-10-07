////////////////////////////////////////////////////////////////////////
/// \brief Interface for calculating the "ultimate mother" of a particle in a simulated event.
///
/// \author  seligman@nevis.columbia.edu
/// \date
////////////////////////////////////////////////////////////////////////

/// This is the base class for an algorithm to calculate the "eve
/// ID". This begs two questions:

/// - What is an eve ID?

/// - What does it mean to be a base class?

/// The "eve ID" is the ultimate "mother" of a particle in a shower
/// produced by the detector simulation. Consider the following
/// example chain of particles:

/// TrackID   Particle       "Mother"
///     2       pi+              0
///   101       nu_mu            2
///   102       mu+              2
///   341       nu_mu_bar      102 
///   342       nu_e           102
///   343       e+             102
///  1022       gamma          343
///  1123       e+            1022
///  1124       e-            1022

/// The "mother" (or parent) particle of track ID 1123 is 1022; the
/// "mother" of track ID 102 is 2. The mother of ID 2 is 0 because it
/// is a primary particle, created by the event generator instead of
/// the detector simulation.

/// The track IDs were originally assigned by the simulation. Each
/// particle is stored in a sim::Particle object. All of the particles
/// for a given event are stored in a sim::ParticleNavigator object.

/// When you are studying an event, especially one with many primary
/// particles, it can be helpful to go up the decay chain, from
/// "child" to mother to "grand-mother", to eventually the ultimate
/// mother particle of the chain. The track ID of the ultimate mother
/// is the "eve ID".

/// In the above example, if we want the eve ID to refer to primary
/// particles, the eve ID of track ID 1123 is 2.

/// This class is never called directly. Instead, it's invoked
/// indirectly via the sim::ParticleNavigator class. For example:

///    const sim::ParticleNavigator* particleList = // ... from somewhere
///    int trackID = // ... an ID you select according to some criteria
///    int eveID = particleList->EveId( trackID );

/// The class below defines the eve ID to be a primary particle that
/// comes from the event. But what if you want a different definition
/// of the ultimate mother; for example, perhaps you only want to go
/// up the chain of particles for basic e-m showers, but stop at more
/// significant physics; for example, in the chain above, you might
/// want the eve ID of track 1123 to be 343, since all the particles
/// descended from 343 are part of the same e-m shower.

/// You can override the default calculation of this class (the "base"
/// calculation) with one of your own. For an example of how to do
/// this, see EmEveIdCalculator.h.

/// (If you're familiar with design patterns, this base class
/// implements the Template Method. No, this has nothing to do with
/// C++ templates; see "Design Patterns" by Gemma et al., or
/// "Effective C++" by Scott Meyers.)

#ifndef SIM_EveIdCalculator_H
#define SIM_EveIdCalculator_H

#include <map>

namespace sim {

  // Forward declaration
  class ParticleNavigator;

  class EveIdCalculator
  {
  public:

    /// Constructor and destructor
    EveIdCalculator();
    virtual ~EveIdCalculator();

    /// Initialize this calculator for a particular ParticleNavigator.
    void Init( const sim::ParticleNavigator* list );

    /// Accessor: For which ParticleNavigator does this calculator generate
    /// results?
    const sim::ParticleNavigator* ParticleNavigator() const { return m_particleNav; }

    /// The main eve ID calculation method. This is the reason why we
    /// use the Template Method for this class: because no matter what
    /// the core eve ID calculation is, we want to perform the
    /// following an additional task:

    /// To save on time, keep the results of previous eve ID
    /// calculations for the current particle list. Only do the
    /// complete eve ID calculation if we haven't done it already for
    /// a given track ID.
    int CalculateEveId( const int trackID );

  protected:
    /// This is the core method to calculate the eve ID. If another
    /// class is going to override the default calculation, this the
    /// method that must be implemented.
    virtual int DoCalculateEveId( const int trackID );

    const sim::ParticleNavigator* m_particleNav; ///< The ParticleNavigator associated with the eve ID calculation.

  private:
    /// Keep track of the previous eve IDs for the current ParticleNavigator.
    typedef std::map< int, int >             m_previousNav_t;
    typedef m_previousNav_t::const_iterator m_previousNav_ptr;
    m_previousNav_t                         m_previousNav; ///< The results of previous eve ID calculations for the current ParticleNavigator.
  };

} // namespace sim

#endif // SIM_EveIdCalculator_H
