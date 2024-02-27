////////////////////////////////////////////////////////////////////////
// \file    SREventQual.cxx
// \brief   An SREventQual contains data quality metrics for the event
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SREventQual.h"

#include <limits>

namespace caf
{
  SREventQual::SREventQual():
    hasssdhits(std::numeric_limits<bool>::signaling_NaN()),
    trigcoinc(std::numeric_limits<int>::signaling_NaN())
  {
  }

  SREventQual::~SREventQual()
  {
  }

  void SREventQual::setDefault()
  {
  }

} // end namespace caf
