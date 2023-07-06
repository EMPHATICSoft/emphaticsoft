////////////////////////////////////////////////////////////////////////
// \file    SRCaloHitBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRCALOHITBRANCH_H
#define SRCALOHITBRANCH_H
#include "StandardRecord/SRCaloHit.h"

#include <vector>

namespace caf
{
  /// Reconstructed rings found by various algorithms
    class SRCaloHitBranch
    {
    public:
      SRCaloHitBranch();
      ~SRCaloHitBranch();

      std::vector<SRCaloHit>  calohit;  ///< CaloHits produced by CaloHit_Reco (?)
      size_t                ncalohit; ///< number of calohits 

      void fillSizes();
      
    };

} // end namespace

#endif // SRCALOHITBRANCH_H
///////////////////////////////////////////////////////////////////////////
