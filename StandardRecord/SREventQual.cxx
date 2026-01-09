////////////////////////////////////////////////////////////////////////
// \file    SREventQual.cxx
// \brief   An SREventQual contains data quality metrics for the event
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SREventQual.h"

#include <limits>

namespace caf
{
  SREventQual::SREventQual():
    hastrigger(std::numeric_limits<bool>::signaling_NaN()),
    hast0caen(std::numeric_limits<bool>::signaling_NaN()),
    haslgcalohits(std::numeric_limits<bool>::signaling_NaN()),
    hasbackovhits(std::numeric_limits<bool>::signaling_NaN()),
    hasgasckovhits(std::numeric_limits<bool>::signaling_NaN()),
    hast0trb3(std::numeric_limits<bool>::signaling_NaN()),
    hasarichhits(std::numeric_limits<bool>::signaling_NaN()),
    hasrpchits(std::numeric_limits<bool>::signaling_NaN()),
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
