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
     sigma = 0.;
 }
  //--------------------------------------------------------------------
  SRLineSegment::~SRLineSegment()
  {
  }

  //--------------------------------------------------------------------
  void SRLineSegment::setDefault()
  {
    x0.SetXYZ(std::numeric_limits<double>::signaling_NaN(),
	      std::numeric_limits<double>::signaling_NaN(),
	      std::numeric_limits<double>::signaling_NaN());
    x1.SetXYZ(std::numeric_limits<double>::signaling_NaN(),
	      std::numeric_limits<double>::signaling_NaN(),
	      std::numeric_limits<double>::signaling_NaN());
    sigma=0.;
  }

} // end namespace
