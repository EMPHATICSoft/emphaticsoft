////////////////////////////////////////////////////////////////////////
/// \brief   A single track, filled in our EMPH Geant4 UserAction 
///
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include "Simulation/Track.h"

namespace sim {

  Track::Track() :
    fX(3, 0.), fP(3, 0.), fPId(0),  fTrackID(-1), fParentTrackID(-1)
  {

  }
  
}

