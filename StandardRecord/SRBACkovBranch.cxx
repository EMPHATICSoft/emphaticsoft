////////////////////////////////////////////////////////////////////////
// \file    SRBACkovBranch.cxx
// \brief   An SRBAckovBranch contains vectors of SRBACkovs.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRBACkovBranch.h"


namespace caf
{
  SRBACkovBranch::SRBACkovBranch():
    nbackovhits(-1)
  {
  }

  //--------------------------------------------------------------------
  SRBACkovBranch::~SRBACkovBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRBACkovBranch::fillSizes()
  {
    nbackovhits = backovhits.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
