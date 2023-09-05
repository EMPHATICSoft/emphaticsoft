////////////////////////////////////////////////////////////////////////
/// \file  MCTrajectory.cxx
/// \brief Container of trajectory information for a particle
///
/// \author  seligman@nevis.columbia.edu changes by jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "cetlib_except/exception.h"

#include "SimulationBase/MCTrajectory.h"

#include <TLorentzVector.h>

#include <cmath>
#include <deque>
#include <iterator>
#include <vector>
#include <set>
#include <map>

namespace simb{

  MCTrajectory::MCTrajectory(): caf::SRTrajectory(){}
  
  MCTrajectory::MCTrajectory(const TLorentzVector& position, const TLorentzVector& momentum) 
    : caf::SRTrajectory(position, momentum){}  
  
}
