////////////////////////////////////////////////////////////////////////
// \file    GasCkovFiller.h
// \note    Class to fill CAF GasCkov info
////////////////////////////////////////////////////////////////////////
#ifndef GASCKOVFILLER_H
#define GASCKOVFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class GasCkovFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // GASCKOVFILLER_H
