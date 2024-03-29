////////////////////////////////////////////////////////////////////////
// \file    SRSSDClustBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSSDCLUSTBRANCH_H
#define SRSSDCLUSTBRANCH_H
#include "StandardRecord/SRSSDClust.h"

#include <vector>

namespace caf
{
  /// Reconstructed ssd clusters found by various algorithms
    class SRSSDClustBranch
    {
    public:
      SRSSDClustBranch();
      ~SRSSDClustBranch();

      std::vector<SRSSDClust>  clust;  ///< SSDClusts produced by MakeSSDClusters
      size_t                nclust; ///< number of ssdclusters 

      void fillSizes();
      
    };

} // end namespace

#endif // SRSSDCLUSTERBRANCH_H
///////////////////////////////////////////////////////////////////////////
