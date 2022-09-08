#include "StandardRecord/StandardRecord.h"

#include <limits>

const float kNaN = std::numeric_limits<float>::signaling_NaN();

namespace caf
{
  StandardRecord::StandardRecord() :
    hdr()
  {
  }
  
  StandardRecord::~StandardRecord()
  {
  }

} // end namespace caf
