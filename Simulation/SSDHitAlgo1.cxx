////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in a SSD plane
///           With added information to generate a set of radSSDDigit. 
/// \author  lebrun@fnal.gov, jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "Simulation/SSDHitAlgo1.h"

namespace sim {

  SSDHitAlgo1::SSDHitAlgo1() :
    fX(0), fYLocal0(0.), fYLocal1(0), fG4VolName("?"), fP(0), fPId(0), fPlane(-1), fSensor(-1), fStrip(-1), fTrackID(-1)
  {

  }
  
}

