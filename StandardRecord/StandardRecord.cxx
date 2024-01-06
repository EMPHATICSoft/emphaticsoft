#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    backov(),
    evtqual(),
    gasckov(),
    spcpts(),
    cluster(),
    lineseg(),
    hits(),
    trks(),
    sgmnts(),
    truth()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
