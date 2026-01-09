////////////////////////////////////////////////////////////////////////
// \file    SRBeamTrack.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRBeamTrack.h"

namespace caf
{
  SRBeamTrack::SRBeamTrack() 
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  SRBeamTrack::~SRBeamTrack()
  {
  }

  //--------------------------------------------------------------------
  SRBeamTrack::SRBeamTrack(const SRTrack& t) : SRTrack(t)
  {
    truth.setDefault();
  }

  //--------------------------------------------------------------------
  void SRBeamTrack::setDefault()
  {
    truth.setDefault();
  }

} // end namespace
