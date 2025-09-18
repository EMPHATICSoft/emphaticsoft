#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    backov(),
    evtqual(),
    gasckov(),
    lineseg(),
    spcpts(),
    cluster(),
    hits(),
    //    trks(),
    sgmnts(),
    vtxs(),
    truth()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
