////////////////////////////////////////////////////////////////////////
/// \brief   Particle list in DetSim contains Monte Carlo particle information.
///
/// \author  seligman@nevis.columbia.edu
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef SIM_PARTICLENAVIGATOR_H
#define SIM_PARTICLENAVIGATOR_H

#include <iostream>
#include <map>
#include <set>

#include "Simulation/Particle.h"

namespace sim 
{
  // Forward declarations.
  class EveIdCalculator;

  /// A container for particles generated during an event simulation.
  /// It acts like a map<int,Particle*> but with additional features:
  ///
  /// - A method Cut(double) that will remove all particles with energy
  ///   less than the argument.
  ///
  /// - Methods TrackId(int) and Particle(int) for those who are unfamiliar with the
  ///   concepts associated with STL maps:
  ///      sim::ParticleNavigator* particleList = // ...;
  ///      int numberOfParticles = particleList->size();
  ///      for (int i=0; i<numberOfParticles; ++i)
  ///        {
  ///           int trackID = particleList->TrackId(i);
  ///           sim::Particle* particle = particleList->Particle(i);
  ///        }
  ///   The STL equivalent to the above statements (more efficient):
  ///      sim::ParticleNavigator* particleList = // ...;
  ///      for ( sim::ParticleNavigator::const_iterator i = particleList->begin();
  ///            i != particleList->end(); ++i )
  ///        {
  ///            const sim::Particle* particle = (*i).second;
  ///            int trackID = particle->TrackId();  // or...
  ///            int trackID = (*i).first;
  ///        }
  ///
  /// - Methods to access the list of primary particles in the event:
  ///      sim::ParticleNavigator particleList = // ...;
  ///      int numberOfPrimaries = particleList->NumberOfPrimaries();
  ///      for ( int i = 0; i != numberOfPrimaries; ++i )
  ///        {
  ///          sim::Particle* particle = particleList->Primary(i);
  ///          ...
  ///        }
  ///   There's also a simple test:
  ///      int trackID = // ...;
  ///      if ( particleList->IsPrimary(trackID) ) {...}
  ///
  ///   (Aside: note that particleList[i] does NOT give you the "i-th"
  ///   particle in the list; it gives you the particle whose trackID
  ///   is "i".)
  ///
  ///  - A method EveId(int) to determine the "eve ID" (or ultimate
  ///    mother) for a given particle. For more information, including how
  ///    to supply your own eve ID calculation, see
  ///    Simulation/EveIdCalculator.h and Simulation/EmEveIdCalculator.h.
  ///
  /// - If you use the clear() or erase() methods, the list will also
  ///   delete the underlying Particle*.  (This means that if you use
  ///   insert() or Add() to add a particle to the list, the
  ///   ParticleNavigator will take over management of it.  Don't delete the
  ///   pointer yourself!)
  ///
  /// - Print() and operator<< methods for ROOT display and ease of
  ///   debugging.
  class ParticleNavigator {
  public:
    // Some type definitions to make life easier, and to help "hide"
    // the implementation details.  (If you're not familiar with STL,
    // you can ignore these definitions.)
    typedef std::map<int,sim::Particle*>        list_type;
    typedef list_type::key_type                 key_type;
    typedef list_type::mapped_type              mapped_type;
    typedef list_type::value_type               value_type;
    typedef list_type::iterator                 iterator;
    typedef list_type::const_iterator           const_iterator;
    typedef list_type::reverse_iterator         reverse_iterator;
    typedef list_type::const_reverse_iterator   const_reverse_iterator;
    typedef list_type::size_type                size_type;
    typedef list_type::difference_type          difference_type;
    typedef list_type::key_compare              key_compare;
    typedef list_type::allocator_type           allocator_type;
    
    // Standard constructor and destructor.
    ParticleNavigator();
    explicit ParticleNavigator(const std::vector<sim::Particle>& parts);
    virtual ~ParticleNavigator();
    
    // Because this list contains pointers, we have to provide the
    // copy and assignment constructors.
    ParticleNavigator( const ParticleNavigator& rhs );
    ParticleNavigator& operator=( const ParticleNavigator& rhs );
    
    // The methods advertised above:
    
    /// Apply an energy threshold cut to the particles in the list,
    /// removing all those that fall below the cut.
    void Cut( const double& );
    
    const key_type& TrackId( const size_type ) const;
    mapped_type Particle( const size_type ) const;
    mapped_type Particle( const size_type );
    
    bool IsPrimary( int trackID ) const;
    int NumberOfPrimaries() const;
    const sim::Particle* Primary( const int ) const;
    sim::Particle* Primary( const int );
    
    // Implementation note: we haven't defined a "+=" for adding the
    // integer offset, because it would involve many insertions and
    // deletions into the same map, or be equivalent to creating a new
    // map and deleting an old one.
    ParticleNavigator Add(const int& offset) const;
    ParticleNavigator operator+(const int& value) const 
    {
      return Add(value);
    }
    
    // Just in case: define the result of "scalar + ParticleNavigator" to be
    // the same as "ParticleNavigator + scalar".
    friend ParticleNavigator operator+(const int& value, const ParticleNavigator& list);
    
    // Standard STL methods, to make this class look like an STL map.
    // Again, if you don't know STL, you can just ignore these
    // methods.
    iterator               begin()        { return fParticleList.begin();  }
    const_iterator         begin()  const { return fParticleList.begin();  }
    iterator               end()          { return fParticleList.end();    }
    const_iterator         end()    const { return fParticleList.end();    }
    reverse_iterator       rbegin()       { return fParticleList.rbegin(); }
    const_reverse_iterator rbegin() const { return fParticleList.rbegin(); }
    reverse_iterator       rend()         { return fParticleList.rend();   }
    const_reverse_iterator rend()   const { return fParticleList.rend();   }
    
    size_type size()           const { return fParticleList.size();  }
    bool empty()               const { return fParticleList.empty(); }
    void swap( ParticleNavigator& other ) { fParticleList.swap( other.fParticleList ); }
    
    iterator       find(const key_type& key)              { return fParticleList.find(key); }
    const_iterator find(const key_type& key)        const { return fParticleList.find(key); }
    iterator       upper_bound(const key_type& key)       { return fParticleList.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const { return fParticleList.upper_bound(key); }
    iterator       lower_bound(const key_type& key)       { return fParticleList.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const { return fParticleList.lower_bound(key); }
    
    /// Be careful when using operator[] here! It takes the track ID as the argument:
    ///   sim::ParticleNavigator partList;
    ///   const sim::Particle* = partList[3]; 
    /// The above line means the particle with trackID==3, NOT the third
    /// particle in the list!  Use partList.Particle(3) if you want to
    /// get the particles by index number instead of track ID.
    /// Note that this only works in a const context.  Use the insert() 
    /// or Add() methods to add a new particle to the list.
    mapped_type operator[]( const key_type& key ) const;
    /// This non-const version of operator[] does NOT permit you to insert
    /// Particles into the list.  Use Add() or insert() for that.
    mapped_type operator[]( const key_type& key );
    mapped_type at(const key_type& key) { return operator[](key); }
    mapped_type at(const key_type& key) const { return operator[](key); }
    
    /// These two methods (insert and Add) do the same thing:
    /// - Add the Particle to the list, using the track ID as the key.
    /// - Update the list of primary particles as needed.
    /// Note that when you insert a Particle* into a ParticleNavigator, it
    /// takes over management of the pointer.  Don't delete it yourself!
    void insert( sim::Particle* value );
    void Add( sim::Particle* value ) { insert(value); }
    
    void clear();
    size_type erase( const key_type& key );
    
    friend std::ostream& operator<< ( std::ostream& output, const ParticleNavigator& );
    
    // Methods associated with the eve ID calculation.
    // Calculate the eve ID.
    int EveId ( const int trackID ) const;
    /// Set a pointer to a different eve ID calculation. The name
    /// begins with "Adopt" because it accepts control of the ponters;
    /// do NOT delete the pointer yourself if you use this method.
    static void AdoptEveIdCalculator( EveIdCalculator* );
    
  private:
    list_type fParticleList;                           ///< Sorted list of particles in the event
    
    typedef std::set< int >                primaries_type;
    typedef primaries_type::iterator       primaries_iterator;
    typedef primaries_type::const_iterator primaries_const_iterator;
    primaries_type                         fPrimaries; ///< Sorted list of the track IDs of primary particles.
    
  public:
  };
}

#endif // SIM_PARTICLENAVIGATOR_H
///////////////////////////////////////////////////////////////////////////
