#include "sbndaq-artdaq-core/Overlays/Common/CAENV1730Fragment.hh"
#include "cetlib_except/exception.h"

bool sbndaq::CAENV1730Fragment::Verify() const {

  if(Metadata()->nChannels!=CAEN_V1730_MAX_CHANNELS) 
    return false;

  if(ExpectedDataSize() != DataPayloadSize())
    return false;

  return true;
}
