//////////////////////////////////////////////////////////////////////////
/// \brief  Event level data quality flags
/// \author Teresa Lackey <lackey32@fnal.gov>
//////////////////////////////////////////////////////////////////////////
#ifndef EVENTQUALITY_H
#define EVENTQUALITY_H

#include <iostream>
#include <cstdint>

namespace emph {
  namespace dq {
    class EventQuality
    {
    public:
      EventQuality();
      ~EventQuality() {};

      bool hasTrigger;      ///< Does the event have Trigger hits?
      bool hasT0CAEN;       ///< Does the event have T0CAEN hits?
      bool hasLGCaloHits;   ///< Does the event have LGCalo hits?
      bool hasBACkovHits;   ///< Does the event have BACkov hits?
      bool hasGasCkovHits;  ///< Does the event have GasCkov hits?
      bool hasT0TRB3;       ///< Does the event have T0TRB3 hits?
      bool hasARICHHits;    ///< Does the event have ARICH hits?
      bool hasRPCHits;      ///< Does the event have RPC hits?
      bool hasSSDHits;      ///< Does the event have SSD hits?
      int  trigCoincLevel;  ///< How many trigger PMTSSDDataLabel: "raw:SSD"s had a peak above threshold?

      // Time sync (from the unpacker's EventSync; why: the spill's SyncQuality flags). Masks: bit (1 << emph::geo::DetectorType)
      bool     hasSyncInfo;          ///< Was the time-sync record found? (false for files unpacked before it existed)
      uint32_t notSyncedDetectors;   ///< Detectors with no readout in this event because of the time sync / DAQ: no hits from them, whatever the beam did
      uint32_t syncDegradedDetectors;///< Detectors whose hits may be missing or misassigned in this event (part of the readout missing, board reset, masked SSD timestamp)

      bool isDetectorSynced(int det) const { return !((notSyncedDetectors | syncDegradedDetectors) & (1u << det)); }
      bool isSyncComplete()          const { return hasSyncInfo && !(notSyncedDetectors | syncDegradedDetectors); }

      //Add all quality metrics to overall event metric here
      bool isEventGood()  const { return hasTrigger && hasT0CAEN && hasLGCaloHits && hasBACkovHits && hasGasCkovHits && hasT0TRB3 && hasARICHHits && hasRPCHits && hasSSDHits; }

      friend std::ostream& operator << (std::ostream& o, const EventQuality& eq);

    };

  }// end namespace dq
}// end namespace emph

#endif //EVENTQUALITY_H
///////////////////////////////////////////////////////////////////////////////
