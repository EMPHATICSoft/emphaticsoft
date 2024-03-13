////////////////////////////////////////////////////////////////////////
// \file    SRBACkov.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>
#include <string.h>

#include "StandardRecord/SRBACkov.h"

namespace caf
{
  SRBACkov::SRBACkov() :
    charge(std::numeric_limits<float>::signaling_NaN()),
    time(std::numeric_limits<float>::signaling_NaN())
  {
  }

  //--------------------------------------------------------------------
  SRBACkov::~SRBACkov()
  {
  }

  //--------------------------------------------------------------------
  void SRBACkov::setDefault()
  {
    charge = -999;
    time = -1;
  }

} // end namespace
