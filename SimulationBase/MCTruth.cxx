/// \brief MC truth class, holds the information about the event generator information and a vector of GenParticle 
/// \author  laliaga@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "SimulationBase/MCTruth.h"

#include <iomanip>
#include "canvas/Utilities/Exception.h"
#include <cassert>
#include <iostream>

namespace sb{

  //......................................................................
  MCTruth::MCTruth() 
    : fPartList()
    , fMCBeamInfo()
  { 
  }

  std::ostream& operator<< (std::ostream& o, sb::MCTruth const& t)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Number of particles = "     << std::setw(5) << std::right << t.NParticles();
    //needs to be implemented
    return o;
  }
}
////////////////////////////////////////////////////////////////////////
