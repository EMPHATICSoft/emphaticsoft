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
    // set to initially crazy numbers
    this->Clear();

    arich.setdefault(); // arich loglikelihooods values
  }
  void SRTrack::Clear() {
    // set to initially crazy numbers
    for(auto& station : posSSD)
      for(auto& plane : station)
          plane.SetXYZ(-999999.,-999999.,-999999.);
    for(auto& station : momSSD)
      for(auto& plane : station)
          plane.SetXYZ(0,0,0);
    for(auto& station : pullSSD)
      for(auto& plane : station)
          plane = -999999.;
    for(auto& station : uncPull)
      for(auto& plane : station)
          plane = -999999.;
  }

} // end namespace
