////////////////////////////////////////////////////////////////////////
// \file     SpacePointFiller.h
// \note    Class to fill CAF  SpacePoint info
////////////////////////////////////////////////////////////////////////
#ifndef  SpacePointFILLER_H
#define  SpacePointFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class SpacePointFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif //  SpacePointFILLER_H
