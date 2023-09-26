////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHits.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRTrueSSDHits.h"

namespace caf
{
  SRTrueSSDHits::SRTrueSSDHits() : 
    GetX(std::numeric_limits<float>::signaling_NaN()),
    GetY(std::numeric_limits<float>::signaling_NaN()),
    GetZ(std::numeric_limits<float>::signaling_NaN()),
    GetPx(std::numeric_limits<float>::signaling_NaN()),
    GetPy(std::numeric_limits<float>::signaling_NaN()),
    GetPz(std::numeric_limits<float>::signaling_NaN()),
    GetDE(std::numeric_limits<float>::signaling_NaN()),
    GetPId(std::numeric_limits<int>::signaling_NaN()),
    GetStation(std::numeric_limits<int>::signaling_NaN()),
    GetPlane(std::numeric_limits<int>::signaling_NaN()),
    GetSensor(std::numeric_limits<int>::signaling_NaN()),
    GetStrip(std::numeric_limits<int>::signaling_NaN()),
    GetTrackID(std::numeric_limits<int>::signaling_NaN())
  {
  }
  //--------------------------------------------------------------------
  SRTrueSSDHits::~SRTrueSSDHits()
  {
  }

  //--------------------------------------------------------------------
  void SRTrueSSDHits::setDefault()
  {
    GetX = -5;
    GetY = -5;
    GetZ = -5;
    GetPx = -5;
    GetPy = -5;
    GetPz = -5;
    GetDE = -5;
    GetPId = -5;
    GetStation = -5;
    GetPlane = -5;
    GetSensor = -5;
    GetStrip = -5;
    GetTrackID = -5;
  }

} // end namespace
