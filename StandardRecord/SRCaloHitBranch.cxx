////////////////////////////////////////////////////////////////////////
// \file    SRCaloHitBranch.cxx
// \brief   An SRCaloHitBranch contains vectors of SRCaloHits.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRCaloHitBranch.h"


namespace caf
{
  SRCaloHitBranch::SRCaloHitBranch():
    ncalohit(-1)
  {
  }

  //--------------------------------------------------------------------
  SRCaloHitBranch::~SRCaloHitBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRCaloHitBranch::fillSizes()
  {
    ncalohit = calohit.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
