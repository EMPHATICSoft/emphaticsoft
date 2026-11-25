////////////////////////////////////////////////////////////////////////
/// \brief   Time-sync quality of one spill (subrun), written by the
///          RawDataUnpacker as a subrun product ("raw", "TimeSync").
///          Flags say which time-sync problems were met and whether they
///          were repaired or cost data; per board: where its data came
///          from and how many events matched.
////////////////////////////////////////////////////////////////////////
#ifndef SYNCQUALITY_H
#define SYNCQUALITY_H

#include <vector>
#include <cstdint>
#include <iostream>

namespace emph {
namespace dq {

  class BoardSync {
  public:
    BoardSync();
    ~BoardSync() {};

    uint32_t fragId;     ///< board (fragment id); the SSDs are 773
    int32_t  fileOffset; ///< file its data came from: 0 own subrun, -1 previous, +1 next (misfiled TRB3)
    uint32_t nEvents;    ///< events in the spill (without the spill-start marker)
    uint32_t nMatched;   ///< events matched to the grandfather after calibration
    uint32_t nResets;    ///< mid-spill board resets (CAEN), repaired
    uint32_t nUnpinned;  ///< blocks after a reset placed by the readout PC clock only (~1 ms), not anchored to the SSDs
    bool     isSeeded;   ///< start offset/drift from the gap seed (the grandfather counts as seeded)
    bool     isUsed;     ///< in the events (false: left out, not synced)
    uint32_t detectors;  ///< detectors this board reads out: bit (1 << emph::geo::DetectorType), from the channel map

    friend std::ostream& operator << (std::ostream& o, const BoardSync& b);
  };

  class SyncQuality {
  public:
    SyncQuality();
    ~SyncQuality() {};

    enum Flag : uint32_t {
      // repaired: the data are fine
      kCAENWrapped         = 1 << 0, ///< CAEN 31-bit trigger time tag wrapped in the spill, unwrapped (exact)
      kCAENReset           = 1 << 1, ///< CAEN board(s) reset mid-spill, blocks re-anchored (readout PC clock + SSDs)
      kTRB3FromNeighbour   = 1 << 2, ///< TRB3 data of this spill taken from the previous/next subrun's file (DAQ misfiling)
      kGrandfatherReplaced = 1 << 3, ///< board with most events failed the seed, another board is the reference
      kSeedFailed          = 1 << 4, ///< a board didn't seed; its matching started from offset 0, drift 0
      // data lost or degraded
      kCAENResetUnpinned   = 1 << 5, ///< blocks after a CAEN reset only placed to ~1 ms: their events mostly unmatched
      kTRB3Missing         = 1 << 6, ///< TRB3 read, but no TRB3 data of this spill in this or the neighbouring files
      kBoardDropped        = 1 << 7, ///< a board left out of the events (not synced)
      kSSDNotSynced        = 1 << 8, ///< SSDs left out of the events (not synced)
      kSSDTimestampMasked  = 1 << 9  ///< SSD timestamps that jumped by >= 2^32 ticks kept only their lower 32 bits (those events' SSD degraded)
    };
    static constexpr uint32_t kLossFlags = kCAENResetUnpinned | kTRB3Missing | kBoardDropped | kSSDNotSynced; ///< kSSDTimestampMasked keeps the events (degraded, not lost)

    uint32_t flags;                ///< OR of Flag
    uint32_t grandfather;          ///< reference board (fragment id)
    uint32_t missingDetectors;     ///< detectors of readouts with no data at all in this spill (TRB3 missing: from the channel map)
    std::vector<BoardSync> boards; ///< every board read, and the SSDs (773)

    bool hasFlag(Flag flag) const { return flags & flag; }
    bool isComplete()       const { return !(flags & kLossFlags); } ///< every board's events are in the events (repairs allowed)
    bool isClean()          const { return flags == 0; }            ///< no time-sync problem met at all

    BoardSync& board(uint32_t fragId); ///< entry of a board, added if missing

    // Detector masks: bit (1 << emph::geo::DetectorType). A detector read out by several boards (T0: CAEN and TRB3)
    // is lost only if none of its boards is in the events, otherwise it is partial.
    uint32_t lostDetectors() const;     ///< no readout of the detector in the events: no hits from it at all
    uint32_t partialDetectors() const;  ///< part of the detector's readout missing (in every event of the spill)
    uint32_t degradedDetectors() const; ///< partial, or some events affected (CAEN resets, masked SSD timestamps); per event see EventSync

    friend std::ostream& operator << (std::ostream& o, const SyncQuality& s);
  };

  /// Time-sync quality of one event, written by the RawDataUnpacker ("raw", "TimeSync"); masks as in SyncQuality
  class EventSync {
  public:
    EventSync();
    ~EventSync() {};

    uint32_t notSyncedDetectors; ///< detectors with no readout in the events of this spill (lost)
    uint32_t degradedDetectors;  ///< detectors whose hits in this event may be missing or misassigned
  };

} // end namespace dq
} // end namespace emph

#endif // SYNCQUALITY_H
