////////////////////////////////////////////////////////////////////////
// \file    SRTrueTargetHit.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRTrueTargetHit.h"

namespace caf
{
  SRTrueTargetHit::SRTrueTargetHit()
  {
    this->setDefault();
  }
  //--------------------------------------------------------------------
  SRTrueTargetHit::~SRTrueTargetHit()
  {
  }

  //--------------------------------------------------------------------
  void SRTrueTargetHit::setDefault()
  {
    pos_pre.SetX(std::numeric_limits<float>::signaling_NaN());
    pos_pre.SetY(std::numeric_limits<float>::signaling_NaN());
    pos_pre.SetZ(std::numeric_limits<float>::signaling_NaN());

    pos_post.SetX(std::numeric_limits<float>::signaling_NaN());
    pos_post.SetY(std::numeric_limits<float>::signaling_NaN());
    pos_post.SetZ(std::numeric_limits<float>::signaling_NaN());

    mom_pre.SetX(std::numeric_limits<float>::signaling_NaN());
    mom_pre.SetY(std::numeric_limits<float>::signaling_NaN());
    mom_pre.SetZ(std::numeric_limits<float>::signaling_NaN());

    mom_post.SetX(std::numeric_limits<float>::signaling_NaN());
    mom_post.SetY(std::numeric_limits<float>::signaling_NaN());
    mom_post.SetZ(std::numeric_limits<float>::signaling_NaN());

    dE = std::numeric_limits<float>::signaling_NaN();
    pid = std::numeric_limits<int>::signaling_NaN();
    trackID = std::numeric_limits<int>::signaling_NaN();
    process = "";
  }

} // end namespace
