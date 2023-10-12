#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    truth(),	 
    hits(),
    backov(),
    gasckov()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
