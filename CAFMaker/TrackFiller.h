////////////////////////////////////////////////////////////////////////
// \file     TrackFiller.h
// \note    Class to fill CAF  Track info
////////////////////////////////////////////////////////////////////////
#ifndef  TrackFILLER_H
#define  TrackFILLER_H

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

#endif //  TrackFILLER_H
