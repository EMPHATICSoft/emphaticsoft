////////////////////////////////////////////////////////////////////////
// \file    SRCaloHit.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRCaloHit.h"

namespace caf
{
  SRCaloHit::SRCaloHit() :
    _channel(4),
    _time(0),
    _intchg(std::numeric_limits<float>::signaling_NaN())
  {
  }

  //--------------------------------------------------------------------
  SRCaloHit::~SRCaloHit()
  {
  }

  //--------------------------------------------------------------------
  void SRCaloHit::setDefault()
  {
    _channel = 4; //change this?
    _time    = 0;
    _intchg  = 0.; 
  }

} // end namespace
