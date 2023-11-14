////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegmentBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACKSEGMENTBRANCH_H
#define SRTRACKSEGMENTBRANCH_H
#include "StandardRecord/SRTrackSegment.h"

#include <vector>

namespace caf
{
  /// Reconstructed track segments found by various algorithms
    class SRTrackSegmentBranch
    {
    public:
      SRTrackSegmentBranch();
      ~SRTrackSegmentBranch();

      std::vector<SRTrackSegment> seg;
      size_t               nseg;     

      void fillSizes();
      
    };

} // end namespace

#endif // SRTRACKBRANCH_H
///////////////////////////////////////////////////////////////////////////
