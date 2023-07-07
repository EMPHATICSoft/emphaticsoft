////////////////////////////////////////////////////////////////////////
// \file    SRCaloHit.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRCaloHit.h"

namespace caf
{
  SRCaloHit::SRCaloHit() :
    channel(4),
    time(0),
    intchg(std::numeric_limits<float>::signaling_NaN())
  {
  }

  //--------------------------------------------------------------------
  SRCaloHit::~SRCaloHit()
  {
  }

  //--------------------------------------------------------------------
  void SRCaloHit::setDefault()
  {
    channel = 4; //change this?
    time    = 0;
    intchg  = 0.; 
  }

} // end namespace
