////////////////////////////////////////////////////////////////////////
// \file    ClusterFiller.h
// \note    Class to fill CAF Cluster info
////////////////////////////////////////////////////////////////////////
#ifndef CLUSTERFILLER_H
#define CLUSTERFILLER_H

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

#endif // CLUSTERFILLER_H
