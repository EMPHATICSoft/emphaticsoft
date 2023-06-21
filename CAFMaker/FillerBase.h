////////////////////////////////////////////////////////////////////////
// \file    FillerBase.h
// \note    A pure virtual base class to be used for CAF "filler" classes
////////////////////////////////////////////////////////////////////////
#ifndef FILLERBASE_H
#define FILLERBASE_H

#include "art/Framework/Principal/Event.h"
#include "StandardRecord/StandardRecord.h"

namespace caf
{
  /// Header representing overview information for the current event/spill
  class FillerBase
  {
  public:

    virtual void Fill(art::Event&, caf::StandardRecord&) = 0;

  };

} // end namespace

#endif // SRHEADER_H
