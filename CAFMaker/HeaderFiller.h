////////////////////////////////////////////////////////////////////////
// \file    HeaderFiller.h
// \note    Class to fill CAF Header info
////////////////////////////////////////////////////////////////////////
#ifndef HEADERFILLER_H
#define HEADERFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class HeaderFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

  };

} // end namespace

#endif // HEADERFILLER_H
