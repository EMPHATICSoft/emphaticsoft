////////////////////////////////////////////////////////////////////////
// \file    SRSpacePointBranch.cxx
// \brief   An SRSpacePointBranch contains vectors of SRSpacePoints.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRSpacePointBranch.h"

namespace caf
{
  SRSpacePointBranch::SRSpacePointBranch():
    nsp(-1)
  {
    sp.clear();
  }

  //--------------------------------------------------------------------
  SRSpacePointBranch::~SRSpacePointBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRSpacePointBranch::fillSizes()
  {
    nsp = sp.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
