////////////////////////////////////////////////////////////////////////
// \file    SRGasCkov.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>
#include <string.h>

#include "StandardRecord/SRGasCkov.h"

namespace caf
{
  SRGasCkov::SRGasCkov() :
    charge(std::numeric_limits<float>::signaling_NaN()),
    time(std::numeric_limits<float>::signaling_NaN())
  {
  }

  //--------------------------------------------------------------------
  SRGasCkov::~SRGasCkov()
  {
  }

  //--------------------------------------------------------------------
  void SRGasCkov::setDefault()
  {
    charge = -999;
    time = -1;
  }

} // end namespace
