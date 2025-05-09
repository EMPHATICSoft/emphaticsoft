////////////////////////////////////////////////////////////////////////
// \file    SRTrack.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRTrack.h"

namespace caf
{
  SRTrack::SRTrack() 
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  SRTrack::~SRTrack()
  {
  }

  //--------------------------------------------------------------------
  void SRTrack::setDefault()
  {
    vtx[0] = vtx[1] = vtx[2] = -999999.;
    mom.SetX(0.);
    mom.SetY(0.);
    mom.SetZ(0.);
    arLL.setdefault();
//    arML.setdefault();
  }

} // end namespace
