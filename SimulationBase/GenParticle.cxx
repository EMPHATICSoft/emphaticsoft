////////////////////////////////////////////////////////////////////////
/// \brief   Definition of generated particle
/// \author  laliaga@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "SimulationBase/GenParticle.h"

#include <iomanip>
#include <cassert>

namespace sb {
  
  //----------------------------------------------------------------------
  
  GenParticle::GenParticle()
  {

  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, GenParticle& p)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " PDG = "             << std::setw(5) << std::right << p.pdgId()
      << " ParentID = "        << std::setw(5) << std::right << p.parentId()
      << " TrackID = "         << std::setw(5) << std::right << p.trackId()
      << " Start Position = (" << std::setw(5) << p.startPosition().x() <<", "
      << p.startPosition().y() << p.startPosition().z()<<" )"
      << " Stop Position = ("  << std::setw(5) << p.stopPosition().x() <<", "
      << p.stopPosition().y()  << p.stopPosition().z()<<" )"
      << " Start Momentum = (" << std::setw(5) << p.startMomentum().x() <<", "
      << p.startMomentum().y() << p.startMomentum().z()<<" )"
      << " Stop Momentum = ("  << std::setw(5) << p.stopMomentum().x() <<", "
      << p.stopMomentum().y()  << p.stopMomentum().z()<<" )"
      << " Start Process = "   << std::setw(5) << std::right << p.startProcess()
      << " Stop Process = "    << std::setw(5) << std::right << p.stopProcess()
      << " Start Volume = "    << std::setw(5) << std::right << p.startVolume()
      << " Stop Volume = "     << std::setw(5) << std::right << p.stopVolume();
    return o;
  }
  
} // end namespace sb
//////////////////////////////////////////////////////////////////////////////
