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
    vtx.SetXYZ(-999999.,-999999.,-999999.); 
//[0] = vtx[1] = vtx[2] = -999999.;
    mom.SetXYZ(0.,0.,0.);
    arich.setdefault(); // arich loglikelihooods values 
  }

} // end namespace
