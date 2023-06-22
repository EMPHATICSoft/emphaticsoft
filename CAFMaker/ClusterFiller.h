////////////////////////////////////////////////////////////////////////
// \file    ClusterFiller.h
// \note    Class to fill CAF Cluster info
////////////////////////////////////////////////////////////////////////
#ifndef ClusterFILLER_H
#define ClusterFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class ClusterFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // ClusterFILLER_H
