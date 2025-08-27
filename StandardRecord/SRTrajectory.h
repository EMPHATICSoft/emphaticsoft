////////////////////////////////////////////////////////////////////////
/// \file  SRTrajectory.h
////////////////////////////////////////////////////////////////////////

/// This class describes the trajectory of a particle created in the
/// Monte Carlo simulation.  It contains vectors of positions and momenta.
/// - Print() and operator<< methods for ROOT display and ease of
///   debugging.

/// There are no units defined in this class.  If it's used with
/// Geant4, the units will be (mm,ns,GeV), but this class does not
/// enforce this.

#ifndef SRTRAJECTORY_H
#define SRTRAJECTORY_H

#include <cstddef>
#include <vector>
#include <iostream>
#include <TLorentzVector.h>
#include "StandardRecord/SRVector3D.h"

namespace caf {

  class SRTrajectory {
  public:
    /// Standard constructor: Start with initial position and momentum
    /// of the particle.
    SRTrajectory();
    ~SRTrajectory() {};

    //virtual ~SRTrajectory(){std::cerr << "Destructing caf::SRTrajectory\n";};
  private:
    std::vector<TLorentzVector> fPos;
    std::vector<TLorentzVector> fMom;

    ///< map of the scattering process to index
    ///< in ftrajectory for a given point
    std::vector< std::pair<size_t, unsigned char> > fTrajectoryProcess; 
      
  public:

    SRTrajectory( const TLorentzVector& vertex,
                  const TLorentzVector& momentum );

    /// The accessor methods described above.
    const TLorentzVector& Position( const size_t ) const;
    const TLorentzVector& Momentum( const size_t ) const;
    double  X( const size_t i ) const;
    double  Y( const size_t i ) const;
    double  Z( const size_t i ) const;
    double  T( const size_t i ) const;
    double Px( const size_t i ) const;
    double Py( const size_t i ) const;
    double Pz( const size_t i ) const;
    double  E( const size_t i ) const;

    double TotalLength() const;

    friend std::ostream& operator<< ( std::ostream& output, const SRTrajectory& );

    size_t    size()                    const;
    bool      empty()                   const;    
    void      clear();

    /// The only "set" methods for this class; once you've added a
    /// trajectory point, you can't take it back.
    void Add(TLorentzVector const& p,
             TLorentzVector const& m );
    void Add(TLorentzVector const& p,
             TLorentzVector const& m,
             std::string    const& process,
             bool keepTransportation = false);

    unsigned char        ProcessToKey(std::string   const& process) const;
    std::string          KeyToProcess(unsigned char const& key)     const;
    std::vector< std::pair<size_t, unsigned char> > const& TrajectoryProcesses() const;

  };

} // namespace caf

inline double           caf::SRTrajectory::X ( const size_t i ) const { return fPos[i].X();      }
inline double           caf::SRTrajectory::Y ( const size_t i ) const { return fPos[i].Y();      }
inline double           caf::SRTrajectory::Z ( const size_t i ) const { return fPos[i].Z();      }
inline double           caf::SRTrajectory::T ( const size_t i ) const { return fPos[i].T();      }
inline double           caf::SRTrajectory::Px( const size_t i ) const { return fMom[i].Px();     }
inline double           caf::SRTrajectory::Py( const size_t i ) const { return fMom[i].Py();     }
inline double           caf::SRTrajectory::Pz( const size_t i ) const { return fMom[i].Pz();     }
inline double           caf::SRTrajectory::E ( const size_t i ) const { return fMom[i].E();      }

inline size_t  caf::SRTrajectory::size()        const { return fPos.size();   }
inline bool    caf::SRTrajectory::empty()       const { return fPos.empty();  }
inline void    caf::SRTrajectory::clear()  { fPos.clear(); fMom.clear();  }

inline void    caf::SRTrajectory::Add(const TLorentzVector& p,
				      const TLorentzVector& m )    
{ fPos.push_back(p); fMom.push_back(m); }

inline std::vector< std::pair<size_t, unsigned char> > const&   caf::SRTrajectory::TrajectoryProcesses() const { return fTrajectoryProcess; }

#endif // SRTrajectory_H
