////////////////////////////////////////////////////////////////////////
// \file     TrackFiller.h
// \note    Class to fill CAF  Track info
////////////////////////////////////////////////////////////////////////
#ifndef  TRACKFILLER_H
#define  TRACKFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class TrackFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif //  TRACKFILLER_H
