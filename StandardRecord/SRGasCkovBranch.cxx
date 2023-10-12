////////////////////////////////////////////////////////////////////////
// \file    SRGasCkovBranch.cxx
// \brief   An SRGasCkovbranch contains vectors of SRGasCkovs.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRGasCkovBranch.h"

namespace caf
{
  SRGasCkovBranch::SRGasCkovBranch():
    ngasckovhits(-1),
    PID{std::numeric_limits<bool>::signaling_NaN(),
        std::numeric_limits<bool>::signaling_NaN(),
        std::numeric_limits<bool>::signaling_NaN(),
        std::numeric_limits<bool>::signaling_NaN(),
        std::numeric_limits<bool>::signaling_NaN()}
  {
  }

  //--------------------------------------------------------------------
  SRGasCkovBranch::~SRGasCkovBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRGasCkovBranch::fillSizes()
  {
    ngasckovhits = gasckovhits.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
