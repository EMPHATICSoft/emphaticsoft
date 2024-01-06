////////////////////////////////////////////////////////////////////////
// \file     SSDHitsFiller.h
// \note    Class to fill CAF  SSDHits info
////////////////////////////////////////////////////////////////////////
#ifndef  SSDHITSFILLER_H
#define  SSDHITSFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class SSDHitsFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif //  SSDHITSFILLER_H
