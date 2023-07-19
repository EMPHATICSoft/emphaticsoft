////////////////////////////////////////////////////////////////////////
// \file    CaloFiller.h
// \note    Class to fill CAF calorimeter info
////////////////////////////////////////////////////////////////////////
#ifndef CALOFILLER_H
#define CALOFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class CaloFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // CALOFILLER_H
