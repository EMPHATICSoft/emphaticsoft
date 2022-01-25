#include "emphatic-artdaq/Overlays/CAENV1720Fragment.hh"
#include "cetlib_except/exception.h"

bool emphaticdaq::CAENV1720Fragment::Verify() const {

  if(Metadata()->nChannels!=CAEN_V1720_MAX_CHANNELS)
    return false;

  if(ExpectedDataSize() != DataPayloadSize())
    return false;

  return true;
}
