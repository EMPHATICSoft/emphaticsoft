////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in a SSD plane
///           With added information to generate a set of rawSSDDigit. 
/// \author  lebrun@fnal.gov, jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "Simulation/SSDHitAlgo1.h"

namespace sim {

  SSDHitAlgo1::SSDHitAlgo1() :
    fX(0), fTLocal0(0.), fTLocal1(0), fEDep(0.), fP(0), fPId(0), fStation(255), fPlane(255), fSensor(255), fStrip(1023), fTrackID(-1)
  {

  }
  
}

