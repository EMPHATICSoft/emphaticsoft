////////////////////////////////////////////////////////////////////////
// \file    SRSecondaryTrack.cxx
// \author  jpaley@fnal.gov
// \brief   
////////////////////////////////////////////////////////////////////////
#include "StandardRecord/SRSecondaryTrack.h"
namespace caf
{
  SRSecondaryTrack::SRSecondaryTrack() 
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  SRSecondaryTrack::~SRSecondaryTrack()
  {
  }

  //--------------------------------------------------------------------
  SRSecondaryTrack::SRSecondaryTrack(const SRTrack& t) : SRTrack(t)
  {
    this->setDefault();
  }

  //--------------------------------------------------------------------
  void SRSecondaryTrack::setDefault()
  {
    for (auto& truthObj : truth)
      truthObj.setDefault();
    
  }
} // end namespace
