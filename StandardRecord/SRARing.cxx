////////////////////////////////////////////////////////////////////////
// \file    SRARing.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRARing.h"

namespace caf
{
  SRARing::SRARing() :
    nhit(0),
    center(std::numeric_limits<float>::signaling_NaN(),
	   std::numeric_limits<float>::signaling_NaN(),
	   std::numeric_limits<float>::signaling_NaN()),
    radius(std::numeric_limits<float>::signaling_NaN())
  {
  }

  //--------------------------------------------------------------------
  SRARing::~SRARing()
  {
  }

  //--------------------------------------------------------------------
  void SRARing::setDefault()
  {
    center.SetXYZ(0,0,0);
    radius = -5;
  }

} // end namespace
