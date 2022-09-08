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
    subevt(0)
  {
  }

  SRHeader::~SRHeader()
  {
  }

  void SRHeader::setDefault()
  {
  }

} // end namespace caf

