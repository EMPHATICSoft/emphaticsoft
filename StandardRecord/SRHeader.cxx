////////////////////////////////////////////////////////////////////////
// \file    SRHeader.cxx
// \brief   An  SRHeader contains identity information for a spill.
//          Spill is initial idea. TODO: Define an event record
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRHeader.h"

#include <limits>

namespace caf
{
  SRHeader::SRHeader():
    run(0),
    subrun(0),
    evt(0),
    timestamp(0), intensity(0.), momentum(0.), MT5CPR(0.), MT6CPR(0.),
    MT5CP2(0.), MT6CP2(0.), TBCCDI(0.), TBCCDO(0.), MT6CA1(0.), MT6CA2(0.),
    MT6CA3(0.)
  {
  }

  SRHeader::~SRHeader()
  {
  }

  void SRHeader::setDefault()
  {
  }

} // end namespace caf
