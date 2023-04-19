////////////////////////////////////////////////////////////////////////
/// \brief   Implemantation of ARICHHit
/// \author  wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////

#include <climits>
#include <cfloat>
#include <iostream>
#include <fstream>

#include "Simulation/ARICHHit.h"

namespace sim {

  ARICHHit::ARICHHit() :
  fBlock(INT_MAX), // See LeadGlass block numerology in the GDML file. 
  fAncestorTrackID(0), // A track id, set when a high track is entering the block  Not yet implemented. 
  fTime(0)
  {

  }
  void ARICHHit::Reset() {
    fTime = 0; 
  }

}

