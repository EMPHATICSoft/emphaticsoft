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
    gasckov()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
