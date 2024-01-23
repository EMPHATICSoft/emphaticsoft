////////////////////////////////////////////////////////////////////////
// \file    EventQualFiller.cxx
// \brief   Class that does the work to extract event-level data quality 
//          metrics from the art event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/EventQualFiller.h"
#include "DataQuality/EventQuality.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void EventQualFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto eventqual = evt.getHandle<emph::dq::EventQuality>(fLabel);

    if(!fLabel.empty() && eventqual.failedToGet())
      return;

    stdrec.evtqual.hasssdhits = eventqual->hasSSDHits;
    stdrec.evtqual.trigcoinc  = eventqual->trigCoincLevel;

  }

} // end namespace caf
