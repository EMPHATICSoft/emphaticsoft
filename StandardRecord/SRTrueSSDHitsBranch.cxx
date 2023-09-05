////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHitsBranch.cxx
// \brief   An SRTrueSSDHitsBranch contains vectors of SRTrueSSDHits.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRTrueSSDHitsBranch.h"


namespace caf
{
  SRTrueSSDHitsBranch::SRTrueSSDHitsBranch():
    ntruehits(-1)
  {
  }

  //--------------------------------------------------------------------
  SRTrueSSDHitsBranch::~SRTrueSSDHitsBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRTrueSSDHitsBranch::fillSizes()
  {
    ntruehits = truehits.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
