#include "RawData/CAENV1720Fragment.h"
#include "cetlib_except/exception.h"

bool emphaticdaq::CAENV1720Fragment::Verify() const {

  if(Metadata()->nChannels!=CAEN_V1720_MAX_CHANNELS)
    return false;

  if(ExpectedDataSize() != DataPayloadSize())
    return false;

  return true;
}
