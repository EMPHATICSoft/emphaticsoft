////////////////////////////////////////////////////////////////////////
// \file     TrackSegmentFiller.h
// \note    Class to fill CAF  TrackSegment info
////////////////////////////////////////////////////////////////////////
#ifndef  TrackSegmentFILLER_H
#define  TrackSegmentFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class TrackSegmentFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif //  TrackSegmentFILLER_H
