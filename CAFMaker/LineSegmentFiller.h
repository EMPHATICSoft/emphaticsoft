////////////////////////////////////////////////////////////////////////
// \file     LineSegmentFiller.h
// \note    Class to fill CAF  LineSegment info
////////////////////////////////////////////////////////////////////////
#ifndef  LINESEGMENTFILLER_H
#define  LINESEGMENTFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class LineSegmentFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif //  LINESEGMENTFILLER_H
