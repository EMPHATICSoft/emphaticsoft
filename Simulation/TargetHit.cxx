////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in the target
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "Simulation/TargetHit.h"

namespace sim {

  TargetHit::TargetHit() :
    fPreX{0, 0, 0}, fPreP{0, 0, 0}, fPostX{0, 0, 0}, fPostP{0, 0, 0}, fProcess(""), fDE(0), fPId(0), fTrackID(-1)
  {

  }
  
}

