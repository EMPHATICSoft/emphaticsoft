#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    hits(),
    truehits(),
    backov(),
    truth()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
