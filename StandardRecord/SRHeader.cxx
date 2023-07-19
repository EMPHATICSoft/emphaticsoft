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
    timestamp(0), intensity(0.), momentum(0.), 
    gcp5(0.), gcp5iFix(0.),
    gcp6(0.), gcp6iFix(0.),
    tbccdi(0.), tbccdo(0.),
    mt6ca1(0.), mt6ca2(0.), mt6ca3(0.)
  {
  }

  SRHeader::~SRHeader()
  {
  }

  void SRHeader::setDefault()
  {
  }

} // end namespace caf
