#ifndef emphatic_artdaq_Overlays_CAENV1720Fragment_hh
#define emphatic_artdaq_Overlays_CAENV1720Fragment_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib_except/exception.h"

#define CAEN_V1720_MAX_CHANNELS 8

namespace emphaticdaq {

  class CAENV1720Fragment;

  struct CAENV1720FragmentMetadata;
  struct CAENV1720EventHeader;
  struct CAENV1720Event;

}

struct emphaticdaq::CAENV1720EventHeader{

  uint32_t eventSize : 28;
  uint32_t marker    : 4;

  uint32_t channelMask    : 8;
  uint32_t pattern        : 16; // Unused for EMPHATIC
  uint32_t eventFormat    : 1;
  uint32_t reserved       : 1;
  uint32_t boardFail      : 1;
  uint32_t boardID        : 5;

  uint32_t eventCounter   : 24;

  uint32_t triggerTimeTag : 32;


  uint32_t ChannelMask() const { return (channelMask & 0xff) ; }

  // with ETTT disabled, trigger tag is 31+1 bits and pattern is used for LVDS info
  uint32_t triggerTime() const { return triggerTimeTag & 0x7fff'ffffU; }
  bool triggerTimeRollOver() const { return bool( triggerTimeTag & 0x8000'0000U ); }

  // with ETTT enabled, trigger tag 48 bits, pattern + triggerTimeTag; no LVDS info
  uint64_t extendedTriggerTime() const
    { return triggerTimeTag + (static_cast<uint64_t>(pattern) << 32U); }

};
static_assert(sizeof(emphaticdaq::CAENV1720EventHeader)==4*sizeof(uint32_t),"CAENV1720EventHeader not correct size.");


struct emphaticdaq::CAENV1720Event{
  CAENV1720EventHeader Header;
  uint16_t             DataBlock;
};

struct emphaticdaq::CAENV1720FragmentMetadata {

  uint32_t  boardID;
  uint32_t  nChannels;
  uint32_t  nSamples;
  uint32_t  timeStampSec;
  uint32_t  timeStampNSec;

  uint32_t  chTemps[CAEN_V1720_MAX_CHANNELS];

  CAENV1720FragmentMetadata() {}

  CAENV1720FragmentMetadata(uint32_t bid, uint32_t nc, 
			    uint32_t ns, uint32_t ts_s, 
			    uint32_t ts_ns,
			    uint32_t chtemp[CAEN_V1720_MAX_CHANNELS]) :
    boardID(bid), nChannels(nc),
    nSamples(ns), timeStampSec(ts_s),
    timeStampNSec(ts_ns)
  {
    memcpy(chTemps,chtemp,CAEN_V1720_MAX_CHANNELS*sizeof(uint32_t));
  }

  size_t ExpectedDataSize() const
  { return (sizeof(CAENV1720EventHeader) +
	    nChannels * nSamples * sizeof(uint16_t)); }

};

class emphaticdaq::CAENV1720Fragment{

public:

  CAENV1720Fragment(artdaq::Fragment const& f) : fFragment(f) {}

  CAENV1720FragmentMetadata const* Metadata() const
  { return fFragment.metadata<emphaticdaq::CAENV1720FragmentMetadata>(); }

  CAENV1720Event const* Event() const
  { return reinterpret_cast<CAENV1720Event const*>(fFragment.dataBeginBytes()); }

  size_t DataPayloadSize() const
  { return fFragment.dataSizeBytes(); }

  size_t ExpectedDataSize() const
  { return Metadata()->ExpectedDataSize(); }

  bool Verify() const;

private:
  artdaq::Fragment fFragment;


};

#endif /* emphaticdaq_datatypes_Overlays_CAENV1720Fragment_hh */
