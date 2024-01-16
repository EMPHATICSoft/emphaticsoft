////////////////////////////////////////////////////////////////////////
// \file    EventQualFiller.h
// \note    Class to fill event-level data quality metrics
////////////////////////////////////////////////////////////////////////
#ifndef EVENTQUALFILLER_H
#define EVENTQUALFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class EventQualFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // EVENTQUALFILLER_H
