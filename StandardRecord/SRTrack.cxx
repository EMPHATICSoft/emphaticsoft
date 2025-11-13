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
    mom.SetXYZ(0.,0.,0.);
    posTrgt.SetXYZ(-999999.,-999999.,-999999.); 
    momTrgt.SetXYZ(0.,0.,0.);
    posARICH.SetXYZ(-999999.,-999999.,-999999.); 
    momARICH.SetXYZ(0.,0.,0.);
    posMagnet.SetXYZ(-999999.,-999999.,-999999.); 
    momMagnet.SetXYZ(0.,0.,0.);
    posSSD.clear();
    momSSD.clear();
    pullSSD.clear();
    
    arich.setdefault(); // arich loglikelihooods values 
  }

} // end namespace
