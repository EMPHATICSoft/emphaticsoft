#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    truth(),	 
    hits(),
    cluster(),
    lineseg(),
    backov(),
    spcpts(),
    sgmnts(),
    trks(),
    gasckov()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
