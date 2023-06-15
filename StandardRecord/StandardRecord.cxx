#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring(),
    truth()	 // Added truth, I really don't know if this does something
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
