////////////////////////////////////////////////////////////////////////
// \file    SRARingBranch.cxx
// \brief   An SRARingBranch contains vectors of SRARings.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRARingBranch.h"


namespace caf
{
  SRARingBranch::SRARingBranch():
    narich(-1)
  {
  }

  //--------------------------------------------------------------------
  SRARingBranch::~SRARingBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRARingBranch::fillSizes()
  {
    narich = arich.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
