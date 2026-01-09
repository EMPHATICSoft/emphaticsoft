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
    beamtrk.setDefault();
    sectrk.clear();
    isPrimary = true;
    pos.SetXYZ(-999999.,-999999.,-999999.);
    chi2 = -999999;
    nsectrk = 0;
  }

} // end namespace
