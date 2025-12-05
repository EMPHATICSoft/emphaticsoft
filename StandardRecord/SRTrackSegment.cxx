////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegment.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRTrackSegment.h"
#include <limits>

namespace caf
{
  SRTrackSegment::SRTrackSegment() 
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  SRTrackSegment::~SRTrackSegment()
  {
  }

  //--------------------------------------------------------------------
  void SRTrackSegment::setDefault()
  {
    for (int i=0; i<3; ++i){
      vtx[i] = std::numeric_limits<float>::signaling_NaN();
      pointA[i] = std::numeric_limits<float>::signaling_NaN();
      pointB[i] = std::numeric_limits<float>::signaling_NaN();
    }
    mom.SetX(std::numeric_limits<float>::signaling_NaN());
    mom.SetY(std::numeric_limits<float>::signaling_NaN());
    mom.SetZ(std::numeric_limits<float>::signaling_NaN());
    region = rb::Region::kRegionNull;
    nspacepoints = -1;
    chi2 = -1;
    
  }

} // end namespace
