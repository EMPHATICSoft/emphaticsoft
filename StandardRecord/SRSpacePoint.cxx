////////////////////////////////////////////////////////////////////////
// \file    SRSpacePoint.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRSpacePoint.h"

namespace caf
{
  SRSpacePoint::SRSpacePoint() 
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  SRSpacePoint::~SRSpacePoint()
  {
  }

  //--------------------------------------------------------------------
  void SRSpacePoint::setDefault()
  {
    x[0] = x[1] = x[2] = -999999.;
    station = -1;
  }

} // end namespace
