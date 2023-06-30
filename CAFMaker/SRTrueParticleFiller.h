////////////////////////////////////////////////////////////////////////
// \file    SRTrueParticleFiller.h
// \note    Class to fill CAF MC particle info
////////////////////////////////////////////////////////////////////////
#ifndef SRTRUEPARTICLEFILLER_H
#define SRTRUEPARTICLEFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class SRTrueParticleFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

  };

} // end namespace

#endif // SRTRUEPARTICLEFILLER_H
