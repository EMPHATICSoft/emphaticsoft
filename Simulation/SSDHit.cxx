////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in a SSD plane
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "Simulation/SSDHit.h"

namespace sim {

  SSDHit::SSDHit() :
    fX(0), fP(0), fDE(0), fPId(0), fStation(-1), 
    fPlane(-1), fSensor(-1), fStrip(-1), fTrackID(-1)
  {

  }

}
