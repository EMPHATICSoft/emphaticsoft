////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegment.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRTrackSegment.h"

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
    vtx[0] = vtx[1] = vtx[2] = -999999.;
    mom.SetX(0.);
    mom.SetY(0.);
    mom.SetZ(0.);
  }

} // end namespace
