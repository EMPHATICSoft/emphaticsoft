////////////////////////////////////////////////////////////////////////
// \file    SREventQual.h
////////////////////////////////////////////////////////////////////////
#ifndef SREVENTQUAL_H
#define SREVENTQUAL_H

#include <stdint.h>

namespace caf
{
  /// Event Quality metrics
  class SREventQual
  {
  public:
    SREventQual();
    ~SREventQual();

    bool hastrigger;      ///< Does the event have Trigger hits?
    bool hast0caen;       ///< Does the event have T0CAEN hits?
    bool haslgcalohits;   ///< Does the event have LGCalo hits?
    bool hasbackovhits;   ///< Does the event have BACkov hits?
    bool hasgasckovhits;  ///< Does the event have GasCkov hits?
    bool hast0trb3;       ///< Does the event have T0TRB3 hits?
    bool hasarichhits;    ///< Does the event have ARICH hits?
    bool hasrpchits;      ///< Does the event have RPC hits?
    bool hasssdhits; ///< Does the event have any SSD Raw Digits?
    int  trigcoinc;  ///< Number of trigger PMTs above threshold.

    // Time sync (detector masks: bit 1 << emph::geo::DetectorType: Trigger 0, GasCkov 1, BACkov 2, T0 3, RPC 4, SSD 5, ARICH 6, LGCalo 7, LAPPD 8)
    bool         hassyncinfo;      ///< Was the time-sync record found?
    unsigned int notsynceddets;    ///< Detectors with no readout in this event (time sync / DAQ): no hits from them
    unsigned int syncdegradeddets; ///< Detectors whose hits may be missing or misassigned in this event

    void setDefault();
  };

} // end namespace

#endif // SREVENTQUAL_H
