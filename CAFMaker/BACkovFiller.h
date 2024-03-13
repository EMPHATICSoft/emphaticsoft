////////////////////////////////////////////////////////////////////////
// \file    BACkovFiller.h
// \note    Class to fill CAF BACkov info
////////////////////////////////////////////////////////////////////////
#ifndef BACKOVFILLER_H
#define BACKOVFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class BACkovFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // BACKOVFILLER_H
