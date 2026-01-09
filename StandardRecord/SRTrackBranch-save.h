////////////////////////////////////////////////////////////////////////
// \file    SRTrackBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACKBRANCH_H
#define SRTRACKBRANCH_H
#include "StandardRecord/SRTrack.h"

#include <cstddef>
#include <vector>

namespace caf
{
  /// Reconstructed rings found by various algorithms
    class SRTrackBranch
    {
    public:
      SRTrackBranch();
      ~SRTrackBranch();

      std::vector<SRTrack> trk; 
      size_t               ntrk;     

      void fillSizes();
      
    };

} // end namespace

#endif // SRTRACKBRANCH_H
///////////////////////////////////////////////////////////////////////////
