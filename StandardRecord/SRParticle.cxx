////////////////////////////////////////////////////////////////////////
/// \file  SRParticle.cxx
/// \brief Description of a particle passed to Geant4
///
/// \version $Id: SRParticle.cxx,v 1.12 2012-11-20 17:39:38 brebel Exp $
/// \author  seligman@nevis.columbia.edu
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRParticle.h"

#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TLorentzVector.h>

#include <iterator>
#include <iostream>
#include <climits>

namespace caf {

  // static variables

  /// How do we indicate an uninitialized variable?  I don't want to
  /// use "0" for PDG, because that's potentially a valid value.
  /// Instead, let the compiler give us a value.  The following
  /// template (from climits) evaluates the lower possible negative
  /// number that you can store in an int.

  const int SRParticle::s_uninitialized = std::numeric_limits<int>::min();

  //------------------------------------------------------------
  SRParticle::SRParticle()
    : fstatus(s_uninitialized)
    , ftrackId(s_uninitialized)
    , fpdgCode(s_uninitialized)
    , fmother(s_uninitialized)
    , fprocess()
    , fendprocess()
    , fmass(s_uninitialized)
    , fpolarization()
    , fdaughters()
    , fWeight(s_uninitialized)
    , fGvtx()
    , frescatter(s_uninitialized)
  {
  }

  //------------------------------------------------------------
  /// Standard constructor.
  SRParticle::SRParticle(const int trackId, 
			 const int pdg, 
			 const std::string process,
			 const int mother, 
			 const double mass,
			 const int status)
    : fstatus(status)
    , ftrackId(trackId)
    , fpdgCode(pdg)
    , fmother(mother)
    , fprocess(process)
    , fendprocess(std::string())
    , fmass(mass)
    , fpolarization()
    , fdaughters()
    , fWeight(0.)
    , fGvtx()
    , frescatter(s_uninitialized)
  {
    // If the user has supplied a mass, use it.  Otherwise, get the
    // particle mass from the PDG table.
    if ( mass < 0 ){
      const TDatabasePDG* databasePDG = TDatabasePDG::Instance();
      const TParticlePDG* definition = databasePDG->GetParticle( pdg );
      // Check that the particle is known to ROOT.  If not, this is
      // not a major error; Geant4 has an internal particle coding
      // scheme for nuclei that ROOT doesn't recognize.
      if ( definition != 0 ){
        fmass = definition->Mass();
      }
    }
    else fmass = mass;
    SetGvtx(0, 0, 0, 0);
  }


  SRParticle::SRParticle(SRParticle const& p, int offset)
    : fstatus(p.StatusCode())
    , ftrackId(p.TrackId()+offset)
    , fpdgCode(p.PdgCode())
    , fmother(p.Mother()+offset)
    , fprocess(p.Process())
    , fendprocess(p.EndProcess())
    , ftrajectory(p.Trajectory())
    , fmass(p.Mass())
    , fWeight(p.Weight())
    , fGvtx(p.GetGvtx())
    , frescatter(p.Rescatter())
  {
    for(int i=0; i<p.NumberDaughters(); i++)
      fdaughters.insert(p.Daughter(i)+offset);
  }


  //----------------------------------------------------------------------------
  void SRParticle::SetEndProcess(std::string s)
  {
    fendprocess = s;
  }

  //------------------------------------------------------------
  // Return the "index-th' daughter in the list.
  int SRParticle::Daughter( const int index ) const
  {
    std::set<int>::const_iterator i = fdaughters.begin();
    std::advance( i, index );
    return *i;
  }

  //----------------------------------------------------------------------------
  void SRParticle::SetGvtx(double *v) 
  {
    for(int i = 0; i < 4; i++) {
      fGvtx[i] = v[i];
    }
  }
  
  //----------------------------------------------------------------------------
  void SRParticle::SetGvtx(float *v) 
  {
    for(int i = 0; i < 4; i++) {
      fGvtx[i] = v[i];
    }
  }
  
  //----------------------------------------------------------------------------
  void SRParticle::SetGvtx(TLorentzVector v)
  {
    fGvtx = v;
  }
  
  //----------------------------------------------------------------------------
  void SRParticle::SetGvtx(double x, double y, double z, double t) 
  {
    fGvtx.SetX(x);
    fGvtx.SetY(y);
    fGvtx.SetZ(z);
    fGvtx.SetT(t);
  }

  //------------------------------------------------------------
  std::ostream& operator<< ( std::ostream& output, const SRParticle& particle )
  {
    output << "ID=" << particle.TrackId() << ", ";
    int pdg =  particle.PdgCode();

    // Try to translate the PDG code into text.
    const TDatabasePDG* databasePDG = TDatabasePDG::Instance();
    const TParticlePDG* definition = databasePDG->GetParticle( pdg );
    // Check that the particle is known to ROOT.  If not, this is
    // not a major error; Geant4 has an internal particle coding
    // scheme for nuclei that ROOT doesn't recognize.
    if ( definition != 0 ) output << definition->GetName();
    else output << "PDG=" << pdg;
    
    output << ", mass="      << particle.Mass()
	   << ", Mother ID=" << particle.Mother()
	   << ", Process="   << particle.Process()
	   << ", Status="    << particle.StatusCode()
	   << "\nthere are " << particle.NumberTrajectoryPoints() << " trajectory points";

    if(particle.NumberTrajectoryPoints() > 0 )
      output << "\nInitial vtx (x,y,z,t)=(" << particle.Vx()
	     << "," << particle.Vy()
	     << "," << particle.Vz()
	     << "," << particle.T()
	     << "),\n Initial mom (Px,Py,Pz,E)=(" << particle.Px()
	     << "," << particle.Py()
	     << "," << particle.Pz()
	     << "," << particle.E()
	     << ")" << std::endl;
    else
      output << std::endl;

    return output;
  }

} // namespace sim
