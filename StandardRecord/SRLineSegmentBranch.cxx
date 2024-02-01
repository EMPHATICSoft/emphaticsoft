////////////////////////////////////////////////////////////////////////
// \file    SRLineSegmentBranch.cxx
// \brief   An SRLineSegmentBranch contains vectors of SRLineSegment.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRLineSegmentBranch.h"


namespace caf
{
  SRLineSegmentBranch::SRLineSegmentBranch():
    nlineseg(-1)
  {
  }

  //--------------------------------------------------------------------
  SRLineSegmentBranch::~SRLineSegmentBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRLineSegmentBranch::fillSizes()
  {
    nlineseg = lineseg.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
