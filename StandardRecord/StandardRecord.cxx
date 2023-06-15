#include "StandardRecord/StandardRecord.h"

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr(),
    ring()
    // truth(p, ptype, targetmaterial)	 // I wanted to have an associated function for the SRTruth object, but it requires 
					 // a bit more involvement (adding passed variables in the StandardRecord() function), 
					 // and I don't want to mess stuff up with that
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
