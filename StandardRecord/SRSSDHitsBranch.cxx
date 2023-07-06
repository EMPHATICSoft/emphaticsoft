////////////////////////////////////////////////////////////////////////
// \file    SRSSDHitsBranch.cxx
// \brief   An SRSSDHitsBranch contains vectors of SRSSDHits.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRSSDHitsBranch.h"

namespace caf
{

  SRSSDHitsBranch::SRSSDHitsBranch():
    nhits(-1)
  {
  }

  //--------------------------------------------------------------------
  SRSSDHitsBranch::~SRSSDHitsBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRSSDHitsBranch::fillSizes()
  {
    nhits = hits.size(); 
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
