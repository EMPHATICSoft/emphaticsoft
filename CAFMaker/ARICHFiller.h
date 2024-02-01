////////////////////////////////////////////////////////////////////////
// \file    ARICHFiller.h
// \note    Class to fill CAF ARICH info
////////////////////////////////////////////////////////////////////////
#ifndef ARICHFILLER_H
#define ARICHFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class ARICHFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // ARICHFILLER_H
