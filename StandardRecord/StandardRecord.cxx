#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    truth(),	 
    hits(),
    backov()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
