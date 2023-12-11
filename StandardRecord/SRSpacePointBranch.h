////////////////////////////////////////////////////////////////////////
// \file    SRSpacePointBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSPACEPOINTBRANCH_H
#define SRSPACEPOINTBRANCH_H
#include "StandardRecord/SRSpacePoint.h"

#include <vector>

namespace caf
{
  /// Reconstructed spacepoints found by various algorithms
    class SRSpacePointBranch
    {
    public:
      SRSpacePointBranch();
      ~SRSpacePointBranch();

      std::vector<SRSpacePoint> sp;  ///< SpacePoints produced by SingleTrackReco
      size_t                nsp;     ///< number of spacepoints

      void fillSizes();
      
    };

} // end namespace

#endif // SRSPACEPOINTBRANCH_H
///////////////////////////////////////////////////////////////////////////
