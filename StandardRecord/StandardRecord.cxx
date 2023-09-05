#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    backov(),
    truth(),	 
    hits()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
