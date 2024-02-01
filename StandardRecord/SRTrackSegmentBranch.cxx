////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegmentBranch.cxx
// \brief   An SRTrackSegmentBranch contains vectors of SRTrackSegments.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRTrackSegmentBranch.h"

namespace caf
{
  SRTrackSegmentBranch::SRTrackSegmentBranch():
    nseg(-1)
  {
    seg.clear();
  }

  //--------------------------------------------------------------------
  SRTrackSegmentBranch::~SRTrackSegmentBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRTrackSegmentBranch::fillSizes()
  {
    nseg = seg.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
