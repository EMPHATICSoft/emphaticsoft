////////////////////////////////////////////////////////////////////////
// \file    SRTrackBranch.cxx
// \brief   An SRTrackBranch contains vectors of SRTracks.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRTrackBranch.h"

namespace caf
{
  SRTrackBranch::SRTrackBranch():
    ntrk(-1)
  {
    trk.clear();
  }

  //--------------------------------------------------------------------
  SRTrackBranch::~SRTrackBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRTrackBranch::fillSizes()
  {
    ntrk = trk.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
