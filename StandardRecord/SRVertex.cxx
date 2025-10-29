////////////////////////////////////////////////////////////////////////
// \file    SRVertex.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRVertex.h"

namespace caf
{
  SRVertex::SRVertex() 
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  SRVertex::~SRVertex()
  {
  }

  //--------------------------------------------------------------------
  void SRVertex::setDefault()
  {
    trk.clear();
    isPrimary = true;
    pos.SetXYZ(-999999.,-999999.,-999999.);
//pos[0] = pos[1] = pos[2] = -999999.;

  }

} // end namespace
