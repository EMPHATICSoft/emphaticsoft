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

    stdrec.evtqual.hasssdhits     = eventqual->hasSSDHits;
    stdrec.evtqual.trigcoinc      = eventqual->trigCoincLevel;
    stdrec.evtqual.hastrigger     = eventqual->hasTrigger;
    stdrec.evtqual.hast0caen      = eventqual->hasT0CAEN;
    stdrec.evtqual.haslgcalohits  = eventqual->hasLGCaloHits;
    stdrec.evtqual.hasbackovhits  = eventqual->hasBACkovHits;
    stdrec.evtqual.hasgasckovhits = eventqual->hasGasCkovHits;
    stdrec.evtqual.hast0trb3      = eventqual->hasT0TRB3;
    stdrec.evtqual.hasarichhits   = eventqual->hasARICHHits;
    stdrec.evtqual.hasrpchits     = eventqual->hasRPCHits;

  }

} // end namespace caf
