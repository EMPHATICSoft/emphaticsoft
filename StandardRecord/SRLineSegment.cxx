////////////////////////////////////////////////////////////////////////
// \file    SRLineSegment.cxx
// \brief   Line Segments made from SSD clusters.
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRLineSegment.h"

namespace caf
{
  SRLineSegment::SRLineSegment()
 { 
   this->setDefault();
 }
  //--------------------------------------------------------------------
  SRLineSegment::~SRLineSegment()
  {
  }

  //--------------------------------------------------------------------
  void SRLineSegment::setDefault()
  {
    for (unsigned int i=0; i<3; ++i) {
      x0[i] = std::numeric_limits<double>::signaling_NaN();
      x1[i] = std::numeric_limits<double>::signaling_NaN();
    }
  }

} // end namespace
