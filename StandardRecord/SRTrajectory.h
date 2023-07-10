////////////////////////////////////////////////////////////////////////
/// \file  SRTrajectory.h
////////////////////////////////////////////////////////////////////////

/// This class describes the trajectory of a particle created in the
/// Monte Carlo simulation.  It generally behaves like a
/// vector< pair<TLorentzVector,TLorentzVector> >, where the first
/// TLorentzVector is the position and the seoond is the momentum,
/// with the following additions:

/// - Methods Position(int) and Momentum(int) for those who are unfamiliar with the
///   concept of "first" and "second" as used with STL pairs:
///      sim::Trajectory* trajectory = caf::MCParticle.Trajectory();
///      int numberOfPonts = trajectory->size();
///      for (int i=0; i<numberOfPoints; ++i)
///        {
///           TLorentzVector position = trajectory->Position(i);
///           TLorentzVector momentum = trajectory->Momentum(i);
///        }
///   The STL equivalent to the above statements (more efficient):
///      sim::Trajectory* trajectory = caf::MCParticle.Trajectory();
///      for ( sim::Trajectory::const_iterator i = trajectory->begin();
///            i != trajectory->end(); ++i )
///        {
///            const TLorentzVector& position = (*i).first;
///            const TLorentzVector& momentum = (*i).second;
///        }

/// - As above, but for each position or momentum component; e.g.,
///   trajectory->X(i).

/// - In addition to push_back(pair< TLorentzVector, TLorentzVector>),
///   there's also push_back(TLorentzVector,TLorentzVector) and
///   Add(TLorentzVector,TLorentzVector).  They all do the same thing:
///   add another point to the trajectory.

/// - Print() and operator<< methods for ROOT display and ease of
///   debugging.

/// There are no units defined in this class.  If it's used with
/// Geant4, the units will be (mm,ns,GeV), but this class does not
/// enforce this.

#ifndef SRTRAJECTORY_H
#define SRTRAJECTORY_H

#include <vector>
#include <iostream>
#include <TLorentzVector.h>
#include "StandardRecord/SRVector3D.h"

namespace caf {

  class SRTrajectory {
  public:
    /// Some type definitions to make life easier, and to help "hide"
    /// the implementation details.  (If you're not familiar with STL,
    /// you can ignore these definitions.)
    typedef std::vector< std::pair<TLorentzVector, TLorentzVector> >  list_type;
    typedef list_type::value_type                                     value_type;
    typedef list_type::iterator                                       iterator;
    typedef list_type::const_iterator                                 const_iterator;
    typedef list_type::reverse_iterator                               reverse_iterator;
    typedef list_type::const_reverse_iterator                         const_reverse_iterator;
    typedef list_type::size_type                                      size_type;
    typedef list_type::difference_type                                difference_type;
    typedef std::vector< std::pair<size_t, unsigned char> >           ProcessMap;
    /// Standard constructor: Start with initial position and momentum
    /// of the particle.
    SRTrajectory();
    //virtual ~SRTrajectory(){std::cerr << "Destructing caf::SRTrajectory\n";};
  private:
    list_type  ftrajectory;        ///< The list of trajectory points
    ProcessMap fTrajectoryProcess; ///< map of the scattering process to index
                                   ///< in ftrajectory for a given point

  public:

    SRTrajectory( const TLorentzVector& vertex,
                  const TLorentzVector& momentum );

    /// The accessor methods described above.
    const TLorentzVector& Position( const size_type ) const;
    const TLorentzVector& Momentum( const size_type ) const;
    double  X( const size_type i ) const;
    double  Y( const size_type i ) const;
    double  Z( const size_type i ) const;
    double  T( const size_type i ) const;
    double Px( const size_type i ) const;
    double Py( const size_type i ) const;
    double Pz( const size_type i ) const;
    double  E( const size_type i ) const;

    double TotalLength() const;

    friend std::ostream& operator<< ( std::ostream& output, const SRTrajectory& );

    /// Standard STL methods, to make this class look like an STL map.
    /// Again, if you don't know STL, you can just ignore these
    /// methods.
    iterator               begin();
    const_iterator         begin()      const;
    iterator               end();
    const_iterator         end()        const;
    reverse_iterator       rbegin();
    const_reverse_iterator rbegin()     const;
    reverse_iterator       rend();
    const_reverse_iterator rend()       const;

    size_type size()                    const;
    bool      empty()                   const;
    void      swap(caf::SRTrajectory& other);
    void      clear();

    // Note that there's no non-const version of operator[] or at() here; once
    // you've added a point to a trajectory, you can't modify it.
    const value_type& operator[](const size_type i) const;
    const value_type& at(const size_type i)         const;

    /// The only "set" methods for this class; once you've added a
    /// trajectory point, you can't take it back.
    void push_back(value_type const& v );
    void push_back(TLorentzVector const& p,
                   TLorentzVector const& m );
    void Add(TLorentzVector const& p,
             TLorentzVector const& m );
    void Add(TLorentzVector const& p,
             TLorentzVector const& m,
             std::string    const& process,
             bool keepTransportation = false);

    unsigned char        ProcessToKey(std::string   const& process) const;
    std::string          KeyToProcess(unsigned char const& key)     const;
    ProcessMap    const& TrajectoryProcesses()                      const;

    /// Remove points from trajectory. Straight line interpolation between the
    /// remaining points will pass no further than \a margin from removed
    /// points.
    void Sparsify(double margin = .1, bool keep_second_to_last = false);

  };

} // namespace caf

inline double           caf::SRTrajectory::X ( const size_type i ) const { return Position(i).X();      }
inline double           caf::SRTrajectory::Y ( const size_type i ) const { return Position(i).Y();      }
inline double           caf::SRTrajectory::Z ( const size_type i ) const { return Position(i).Z();      }
inline double           caf::SRTrajectory::T ( const size_type i ) const { return Position(i).T();      }
inline double           caf::SRTrajectory::Px( const size_type i ) const { return Momentum(i).Px();     }
inline double           caf::SRTrajectory::Py( const size_type i ) const { return Momentum(i).Py();     }
inline double           caf::SRTrajectory::Pz( const size_type i ) const { return Momentum(i).Pz();     }
inline double           caf::SRTrajectory::E ( const size_type i ) const { return Momentum(i).E();      }

inline caf::SRTrajectory::iterator               caf::SRTrajectory::begin()             { return ftrajectory.begin();  }
inline caf::SRTrajectory::const_iterator         caf::SRTrajectory::begin()       const { return ftrajectory.begin();  }
inline caf::SRTrajectory::iterator               caf::SRTrajectory::end()               { return ftrajectory.end();    }
inline caf::SRTrajectory::const_iterator         caf::SRTrajectory::end()         const { return ftrajectory.end();    }
inline caf::SRTrajectory::reverse_iterator       caf::SRTrajectory::rbegin()            { return ftrajectory.rbegin(); }
inline caf::SRTrajectory::const_reverse_iterator caf::SRTrajectory::rbegin()      const { return ftrajectory.rbegin(); }
inline caf::SRTrajectory::reverse_iterator       caf::SRTrajectory::rend()              { return ftrajectory.rend();   }
inline caf::SRTrajectory::const_reverse_iterator caf::SRTrajectory::rend()        const { return ftrajectory.rend();   }
inline caf::SRTrajectory::size_type              caf::SRTrajectory::size()        const { return ftrajectory.size();   }
inline bool                                       caf::SRTrajectory::empty()       const { return ftrajectory.empty();  }
inline void                                       caf::SRTrajectory::clear()             { ftrajectory.clear();         }
inline void                                       caf::SRTrajectory::swap(caf::SRTrajectory& other)
                                                                                { ftrajectory.swap( other.ftrajectory ); }

inline const caf::SRTrajectory::value_type&      caf::SRTrajectory::operator[](const caf::SRTrajectory::size_type i) const
                                                                                                { return ftrajectory[i]; }

inline const caf::SRTrajectory::value_type&      caf::SRTrajectory::at(const caf::SRTrajectory::size_type i)         const
                                                                                             { return ftrajectory.at(i); }

inline void                                       caf::SRTrajectory::push_back(const caf::SRTrajectory::value_type& v )
                                                                                             { ftrajectory.push_back(v); }

inline void                                       caf::SRTrajectory::push_back(const TLorentzVector& p,
                                                                                const TLorentzVector& m )
                                                         { ftrajectory.push_back( caf::SRTrajectory::value_type(p,m) ); }

inline void                                       caf::SRTrajectory::Add(const TLorentzVector& p,
                                                                          const TLorentzVector& m )    { push_back(p,m); }

inline caf::SRTrajectory::ProcessMap    const&   caf::SRTrajectory::TrajectoryProcesses() const { return fTrajectoryProcess; }

#endif // SRTrajectory_H
