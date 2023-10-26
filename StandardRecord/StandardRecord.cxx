#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    truth(),	 
    hits(),
    cluster(),
    backov(),
    spcpts(),
    sgmnts(),
    trks()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
