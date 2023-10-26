////////////////////////////////////////////////////////////////////////
// \file    SRSpacePointBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSPACEPOINTBRANCH_H
#define SRSPACEPOINTBRANCH_H
#include "StandardRecord/SRSpacePoint.h"

#include <vector>

namespace caf
{
  /// Reconstructed rings found by various algorithms
    class SRSpacePointBranch
    {
    public:
      SRSpacePointBranch();
      ~SRSpacePointBranch();

      std::vector<SRSpacePoint> sp;  ///< SpacePoints produced by ARichReco
      size_t                nsp;     ///< number of arings 

      void fillSizes();
      
    };

} // end namespace

#endif // SRSPACEPOINTBRANCH_H
///////////////////////////////////////////////////////////////////////////
