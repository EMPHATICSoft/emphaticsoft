////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHits.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRTrueSSDHits.h"

namespace caf
{
  SRTrueSSDHits::SRTrueSSDHits()
  {
    this->setDefault();
  }
  //--------------------------------------------------------------------
  SRTrueSSDHits::~SRTrueSSDHits()
  {
  }

  //--------------------------------------------------------------------
  void SRTrueSSDHits::setDefault()
  {
    pos.SetX(std::numeric_limits<float>::signaling_NaN());
    pos.SetY(std::numeric_limits<float>::signaling_NaN());
    pos.SetZ(std::numeric_limits<float>::signaling_NaN());

    mom.SetX(std::numeric_limits<float>::signaling_NaN());
    mom.SetY(std::numeric_limits<float>::signaling_NaN());
    mom.SetZ(std::numeric_limits<float>::signaling_NaN());

    dE = std::numeric_limits<float>::signaling_NaN();
    pid = std::numeric_limits<int>::signaling_NaN();
    station = std::numeric_limits<int>::signaling_NaN();
    plane = std::numeric_limits<int>::signaling_NaN();
    sensor = std::numeric_limits<int>::signaling_NaN();
    strip = std::numeric_limits<int>::signaling_NaN();
    trackID = std::numeric_limits<int>::signaling_NaN();

  }

} // end namespace
