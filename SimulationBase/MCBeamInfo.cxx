////////////////////////////////////////////////////////////////////////
/// \brief   Definition of the simulated beam
/// \author  laliaga@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "SimulationBase/MCBeamInfo.h"

#include <iomanip>
#include <cassert>

namespace simb {
  
  //----------------------------------------------------------------------
  
  MCBeamInfo::MCBeamInfo()   
  {
    
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, MCBeamInfo& b)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " PDG = "              << std::setw(5) << std::right << b.pdgId()
      << " Start Position = ("   << std::setw(5) << b.startPosition().x()
      << ", " << b.startPosition().y()<<", "<<b.startPosition().z() <<" )"
      << " Start Momentum = ("   << std::setw(5) << b.startMomentum().x()
      << ", " << b.startMomentum().y()<<", "<<b.startMomentum().z() << ")";
    return o;
  }
  
} // end namespace sb
//////////////////////////////////////////////////////////////////////////////
