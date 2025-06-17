////////////////////////////////////////////////////////////////////////
// \file    SRLineSegmentBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRLINESEGMENTBRANCH_H
#define SRLINESEGMENTBRANCH_H
#include "StandardRecord/SRLineSegment.h"

#include <cstddef>
#include <vector>

namespace caf
{
  /// Reconstructed line segments from ssd clusters
    class SRLineSegmentBranch
    {
    public:
      SRLineSegmentBranch();
      ~SRLineSegmentBranch();

      std::vector<SRLineSegment>  lineseg;  ///< LineSegments produced by MakeSSDClusters
      size_t                     nlineseg;  ///< number of LineSegments

      void fillSizes();
      
    };

} // end namespace

#endif // SRLINESEGMENTERBRANCH_H
///////////////////////////////////////////////////////////////////////////
