////////////////////////////////////////////////////////////////////////
// \file    SRSSDClustBranch.cxx
// \brief   An SRSSDClustBranch contains vectors of SRSSDCluster.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRSSDClustBranch.h"


namespace caf
{
  SRSSDClustBranch::SRSSDClustBranch():
    nclust(-1)
  {
  }

  //--------------------------------------------------------------------
  SRSSDClustBranch::~SRSSDClustBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRSSDClustBranch::fillSizes()
  {
    nclust = clust.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
