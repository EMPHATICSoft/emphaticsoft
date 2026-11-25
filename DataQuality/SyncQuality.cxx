////////////////////////////////////////////////////////////////////////
/// \brief   SyncQuality class: time-sync status of one spill
////////////////////////////////////////////////////////////////////////
#include "DataQuality/SyncQuality.h"

#include <iostream>

namespace emph {
namespace dq {

  //----------------------------------------------------------------------

  BoardSync::BoardSync() :
    fragId(0)
    , fileOffset(0)
    , nEvents(0)
    , nMatched(0)
    , nResets(0)
    , nUnpinned(0)
    , isSeeded(false)
    , isUsed(true)
    , detectors(0)
  {
  }

  //----------------------------------------------------------------------

  SyncQuality::SyncQuality() :
    flags(0)
    , grandfather(0)
    , missingDetectors(0)
  {
  }

  //----------------------------------------------------------------------

  BoardSync& SyncQuality::board(uint32_t fragId)
  {
    for (auto& b : boards)
      if (b.fragId == fragId) return b;
    boards.emplace_back();
    boards.back().fragId = fragId;
    return boards.back();
  }

  //----------------------------------------------------------------------

  EventSync::EventSync() :
    notSyncedDetectors(0)
    , degradedDetectors(0)
  {
  }

  //----------------------------------------------------------------------

  namespace {
    // detectors missing a readout (board left out, or no data), and detectors still read out by a used board
    void readoutMasks(const SyncQuality& s, uint32_t& missing, uint32_t& served)
    {
      missing = s.missingDetectors;
      served = 0;
      for (const auto& b : s.boards) {
        if (b.isUsed) served |= b.detectors;
        else          missing |= b.detectors;
      }
    }
  }

  uint32_t SyncQuality::lostDetectors() const
  {
    uint32_t missing, served;
    readoutMasks(*this, missing, served);
    return missing & ~served;
  }

  uint32_t SyncQuality::partialDetectors() const
  {
    uint32_t missing, served;
    readoutMasks(*this, missing, served);
    return missing & served;
  }

  //----------------------------------------------------------------------

  uint32_t SyncQuality::degradedDetectors() const
  {
    const uint32_t ssdId = 773;
    uint32_t mask = partialDetectors();
    for (const auto& b : boards) {
      if (!b.isUsed) continue;
      if (b.nUnpinned > 0 || (b.fragId == ssdId && (flags & kSSDTimestampMasked))) mask |= b.detectors;
    }
    return mask;
  }

  //----------------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const BoardSync& b)
  {
    o << "board " << b.fragId << " (detectors 0x" << std::hex << b.detectors << std::dec << "): " << b.nMatched << " of " << b.nEvents << " matched";
    if (b.fileOffset != 0) o << ", data from the " << (b.fileOffset < 0 ? "previous" : "next") << " file";
    if (b.nResets > 0)     o << ", " << b.nResets << " resets (" << b.nUnpinned << " blocks unpinned)";
    if (!b.isSeeded)       o << ", not seeded";
    if (!b.isUsed)         o << ", LEFT OUT";
    return o;
  }

  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const SyncQuality& s)
  {
    static const char* names[] = {"CAENWrapped", "CAENReset", "TRB3FromNeighbour", "GrandfatherReplaced", "SeedFailed",
                                  "CAENResetUnpinned", "TRB3Missing", "BoardDropped", "SSDNotSynced", "SSDTimestampMasked"};
    o << "Sync status: flags 0x" << std::hex << s.flags << std::dec << " (";
    bool isFirst = true;
    for (size_t bit = 0; bit < sizeof(names)/sizeof(names[0]); ++bit) {
      if (!(s.flags & (1u << bit))) continue;
      o << (isFirst ? "" : " ") << names[bit];
      isFirst = false;
    }
    o << (isFirst ? "clean" : "") << (s.isComplete() ? "" : "; DATA LOST") << "), grandfather " << s.grandfather
      << std::hex << ", detectors lost 0x" << s.lostDetectors() << ", partial 0x" << s.partialDetectors()
      << ", degraded 0x" << s.degradedDetectors() << std::dec;
    for (const auto& b : s.boards)
      o << "\n  " << b;
    return o;
  }

} // end namespace dq
} // end namespace emph
