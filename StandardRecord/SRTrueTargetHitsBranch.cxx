////////////////////////////////////////////////////////////////////////
// \file    SRTrueTargetHitsBranch.cxx
// \brief   An SRTrueTargetHitsBranch contains vectors of SRTrueTargetHits.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRTrueTargetHitsBranch.h"


namespace caf
{
  SRTrueTargetHitsBranch::SRTrueTargetHitsBranch():
    ntruehits(-1)
  {
  }

  //--------------------------------------------------------------------
  SRTrueTargetHitsBranch::~SRTrueTargetHitsBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRTrueTargetHitsBranch::fillSizes()
  {
    ntruehits = truehits.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
