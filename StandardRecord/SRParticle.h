////////////////////////////////////////////////////////////////////////
/// \file  SRParticle.h
/// \brief Particle class
////////////////////////////////////////////////////////////////////////

/// This class describes a particle created in the detector Monte
/// Carlo simulation.

#ifndef SRPARTICLE_H
#define SRPARTICLE_H

#include "StandardRecord/SRTrajectory.h"
#include <set>
#include <string>
#include <iostream>
#include "TVector3.h"
#include "TLorentzVector.h"
namespace caf {

  class SRParticle  {
  public:

    // An indicator for an uninitialized variable (see SRParticle.cxx).
    static const int s_uninitialized; //! Don't write this as ROOT output

    SRParticle();

  protected:
    typedef std::set<int>   daughters_type;

    int                     fstatus;        ///< Status code from generator, geant, etc
    int                     ftrackId;       ///< TrackId
    int                     fpdgCode;       ///< PDG code
    int                     fmother;        ///< Mother
    std::string             fprocess;       ///< Detector-simulation physics process that created the particle
    std::string             fendprocess;    ///< end process for the particle
    caf::SRTrajectory      ftrajectory;    ///< particle trajectory (position,momentum)
    double                  fmass;          ///< Mass; from PDG unless overridden Should be in GeV
    TVector3              fpolarization;  ///< Polarization
    daughters_type          fdaughters;     ///< Sorted list of daughters of this particle.
    double                  fWeight;        ///< Assigned weight to this particle for MC tests
    TLorentzVector          fGvtx;          ///< Vertex needed by generater (genie) to rebuild 
                                            ///< genie::EventRecord for event reweighting
    int                     frescatter;     ///< rescatter code

  public:

    // Standard constructor.  If the mass is not supplied in the
    // argument, then the PDG mass is used.
    // status code = 1 means the particle is to be tracked, default it to be tracked
    // mother = -1 means that this particle has no mother
    SRParticle(const int trackId,
               const int pdg,
               const std::string process,
               const int mother  = -1,
               const double mass = s_uninitialized,
               const int status  = 1);
//    virtual ~SRParticle() = default;

    // our own copy and move assignment constructors (default)
    SRParticle(SRParticle const &)            = default; // Copy constructor.
    SRParticle& operator=( const SRParticle&) = default;
    SRParticle(SRParticle&&) = default;
    SRParticle& operator= (SRParticle&&) = default;


    // constructor for copy from SRParticle, but with offset trackID
    SRParticle(SRParticle const&, int);

    // Accessors.
    //
    // The track ID number assigned by the Monte Carlo.  This will be
    // unique for each Particle in an event. - 0 for primary particles
    int TrackId() const;

    // Get at the status code returned by GENIE, Geant4, etc
    int StatusCode() const;

    // The PDG code of the particle.  Note that Geant4 uses the
    // "extended" system for encoding nuclei; e.g., 1000180400 is an
    // Argon nucleus.  See "Monte Carlo PArticle Numbering Scheme" in
    // any Review of Particle Physics.
    int PdgCode() const;

    // The track ID of the mother particle.  Note that it's possible
    // for a particle to have a mother that's not recorded in the
    // Particle list; e.g., an excited nucleus with low kinetic energy
    // emits a photon with high kinetic energy.
    int Mother() const;

    const TVector3& Polarization() const;
    void            SetPolarization( const TVector3& p );

    // The detector-simulation physics process that created the
    // particle. If this is a primary particle, it will have the
    // value "primary"
    std::string Process()   const;

    std::string EndProcess()   const;
    void SetEndProcess(std::string s);

    // Accessors for daughter information.  Note that it's possible
    // (even likely) for a daughter track not to be included in a
    // Particle list, if that daughter particle falls below the energy cut.
    void AddDaughter( const int trackID );
    int  NumberDaughters()               const;
    int  Daughter(const int i)           const; //> Returns the track ID for the "i-th" daughter.

    // Accessors for trajectory information.
    unsigned int NumberTrajectoryPoints() const;

    // To avoid confusion with the X() and Y() methods of MCTruth
    // (which return Feynmann x and y), use "Vx,Vy,Vz" for the
    // vertex.
    const TLorentzVector& Position( const int i = 0 ) const;
    double                Vx(const int i = 0)         const;
    double                Vy(const int i = 0)         const;
    double                Vz(const int i = 0)         const;
    double                 T(const int i = 0)         const;

    const TLorentzVector& EndPosition() const;
    double                EndX()        const;
    double                EndY()        const;
    double                EndZ()        const;
    double                EndT()        const;

    const TLorentzVector& Momentum( const int i = 0 ) const;
    double                Px(const int i = 0)         const;
    double                Py(const int i = 0)         const;
    double                Pz(const int i = 0)         const;
    double                 E(const int i = 0)         const;
    double                 P(const int i = 0)         const;
    double                Pt(const int i = 0)         const;
    double                Mass()                      const;

    const TLorentzVector& EndMomentum() const;
    double                EndPx()       const;
    double                EndPy()       const;
    double                EndPz()       const;
    double                 EndE()       const;

    // Getters and setters for the generator vertex
    // These are for setting the generator vertex.  In the case of genie
    // the generator assumes a cooridnate system with origin at the nucleus.
    // These variables save the particle vertexs in this cooridnate system.
    // After genie generates the event, a cooridnate transformation is done
    // to place the event in the detector cooridnate system.  These variables
    // store the vertex before that cooridnate transformation happens.
    void     SetGvtx(double *v);
    void     SetGvtx(float  *v);
    void     SetGvtx(TLorentzVector v);
    void     SetGvtx(double x,
                     double y,
                     double z,
                     double t);
    TLorentzVector GetGvtx()     const;
    double             Gvx()     const;
    double             Gvy()     const;
    double             Gvz()     const;
    double             Gvt()     const;

    //Getters and setters for first and last daughter data members
    int FirstDaughter() const;
    int LastDaughter()  const;

    //Getters and setters for rescatter status
    void SetRescatter(int code);
    int  Rescatter() const;

    // Access to the trajectory in both a const and non-const context.
    const caf::SRTrajectory& Trajectory() const;

    // Make it easier to add a (position,momentum) point to the
    // trajectory. You must add this information for every point you wish to keep
    void AddTrajectoryPoint(TLorentzVector const& position,
                            TLorentzVector const& momentum );
    void AddTrajectoryPoint(TLorentzVector const& position,
                            TLorentzVector const& momentum,
                            std::string    const& process,
                            bool keepTransportation = false);

    // methods for giving/accessing a weight to this particle for use
    // in studies of rare processes, etc
    double Weight() const;
    void   SetWeight(double wt);

    void SparsifyTrajectory(double margin = 0.1, bool keep_second_to_last = false);

    // Define a comparison operator for particles.  This allows us to
    // keep them in sets or maps.  It makes sense to order a list of
    // particles by track ID... but take care!  After we get past the
    // primary particles in an event, it is NOT safe to assume that a
    // particle with a lower track ID is "closer" to the event
    // vertex.
    bool operator<( const caf::SRParticle& other ) const;

    friend std::ostream& operator<< ( std::ostream& output, const caf::SRParticle& );
  };

} // namespace caf

#include <functional> // so we can redefine less<> below
#include <math.h>

// methods to access data members and other information
inline       int             caf::SRParticle::TrackId()                const { return ftrackId;                 }
inline       int             caf::SRParticle::StatusCode()             const { return fstatus;                  }
inline       int             caf::SRParticle::PdgCode()                const { return fpdgCode;                 }
inline       int             caf::SRParticle::Mother()                 const { return fmother;                  }
inline const TVector3&     caf::SRParticle::Polarization()           const { return fpolarization;            }
inline       std::string     caf::SRParticle::Process()                const { return fprocess;                 }
inline       std::string     caf::SRParticle::EndProcess()             const { return fendprocess;              }
inline       int             caf::SRParticle::NumberDaughters()        const { return fdaughters.size();        }
inline       unsigned int    caf::SRParticle::NumberTrajectoryPoints() const { return ftrajectory.size();       }
inline const TLorentzVector& caf::SRParticle::Position( const int i )  const { return ftrajectory.Position(i);  }
inline const TLorentzVector& caf::SRParticle::Momentum( const int i )  const { return ftrajectory.Momentum(i);  }
inline       double          caf::SRParticle::Vx(const int i)          const { return Position(i).X();          }
inline       double          caf::SRParticle::Vy(const int i)          const { return Position(i).Y();          }
inline       double          caf::SRParticle::Vz(const int i)          const { return Position(i).Z();          }
inline       double          caf::SRParticle::T(const int i)           const { return Position(i).T();          }
inline const TLorentzVector& caf::SRParticle::EndPosition()            const { return Position(ftrajectory.size()-1);     }
inline       double          caf::SRParticle::EndX()                   const { return Position(ftrajectory.size()-1).X(); }
inline       double          caf::SRParticle::EndY()                   const { return Position(ftrajectory.size()-1).Y(); }
inline       double          caf::SRParticle::EndZ()                   const { return Position(ftrajectory.size()-1).Z(); }
inline       double          caf::SRParticle::EndT()                   const { return Position(ftrajectory.size()-1).T(); }
inline       double          caf::SRParticle::Px(const int i)          const { return Momentum(i).Px();         }
inline       double          caf::SRParticle::Py(const int i)          const { return Momentum(i).Py();         }
inline       double          caf::SRParticle::Pz(const int i)          const { return Momentum(i).Pz();         }
inline       double          caf::SRParticle::E(const int i)           const { return Momentum(i).E();          }
inline       double          caf::SRParticle::P(const int i)           const { return std::sqrt(std::pow(Momentum(i).E(),2.)
                                                                                                 - std::pow(fmass,2.));            }
inline       double          caf::SRParticle::Pt(const int i)          const { return std::sqrt(  std::pow(Momentum(i).Px(),2.)
                                                                                                 + std::pow(Momentum(i).Py(),2.)); }

inline       double          caf::SRParticle::Mass()                   const { return fmass;                              }
inline const TLorentzVector& caf::SRParticle::EndMomentum()            const { return Momentum(ftrajectory.size()-1);     }
inline       double          caf::SRParticle::EndPx()                  const { return Momentum(ftrajectory.size()-1).X(); }
inline       double          caf::SRParticle::EndPy()                  const { return Momentum(ftrajectory.size()-1).Y(); }
inline       double          caf::SRParticle::EndPz()                  const { return Momentum(ftrajectory.size()-1).Z(); }
inline       double          caf::SRParticle::EndE()                   const { return Momentum(ftrajectory.size()-1).T(); }
inline       TLorentzVector  caf::SRParticle::GetGvtx()                const { return fGvtx;                              }
inline       double          caf::SRParticle::Gvx()                    const { return fGvtx.X();                          }
inline       double          caf::SRParticle::Gvy()                    const { return fGvtx.Y();                          }
inline       double          caf::SRParticle::Gvz()                    const { return fGvtx.Z();                          }
inline       double          caf::SRParticle::Gvt()                    const { return fGvtx.T();                          }
inline       int             caf::SRParticle::FirstDaughter()          const { return *(fdaughters.begin());              }
inline       int             caf::SRParticle::LastDaughter()           const { return *(fdaughters.rbegin());             }
inline       int             caf::SRParticle::Rescatter()              const { return frescatter;                         }
inline const caf::SRTrajectory& caf::SRParticle::Trajectory()         const { return ftrajectory;                        }
inline       double          caf::SRParticle::Weight()                 const { return fWeight;                            }

// methods to set information
inline       void            caf::SRParticle::AddTrajectoryPoint(TLorentzVector const& position,
                                                                  TLorentzVector const& momentum )
                                                                              { ftrajectory.Add( position, momentum );     }
inline       void            caf::SRParticle::AddTrajectoryPoint(TLorentzVector const& position,
                                                                  TLorentzVector const& momentum,
                                                                  std::string    const& process,
                                                                  bool keepTransportation)
                                                                              { ftrajectory.Add( position, momentum, process, keepTransportation); }
inline       void            caf::SRParticle::SparsifyTrajectory(double margin,
                                                                  bool keep_second_to_last)
                                                                              { ftrajectory.Sparsify( margin, keep_second_to_last );            }
inline       void            caf::SRParticle::AddDaughter(int const trackID)     { fdaughters.insert(trackID);                }
inline       void            caf::SRParticle::SetPolarization(TVector3 const& p) { fpolarization = p;                         }
inline       void            caf::SRParticle::SetRescatter(int code)             { frescatter    = code;                      }
inline       void            caf::SRParticle::SetWeight(double wt)               { fWeight       = wt;                        }

// definition of the < operator
inline       bool            caf::SRParticle::operator<( const caf::SRParticle& other ) const { return ftrackId < other.ftrackId; }

// A potentially handy definition: At this stage, I'm not sure
// whether I'm going to be keeping a list based on Particle or on
// Particle*.  We've already defined operator<(Particle,Particle),
// that is, how to compare two Particle objects; by default that also
// defines less<Particle>, which is what the STL containers use for
// comparisons.

// The following defines less<Particle*>, that is, how to compare two
// Particle*: by looking at the objects, not at the pointer
// addresses.  The result is that, e.g., a set<Particle*> will be
// sorted in the order I expect.

namespace std {
  template <>
  class less<caf::SRParticle*>
  {
  public:
    bool operator()( const caf::SRParticle* lhs, const caf::SRParticle* rhs )
    {
      return (*lhs) < (*rhs);
    }
  };
} // std

#endif // caf_SRParticle_H
