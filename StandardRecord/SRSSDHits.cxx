////////////////////////////////////////////////////////////////////////
// \file    SRSSDHits.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRSSDHits.h"

namespace caf
{
  SRSSDHits::SRSSDHits() :
    FER(std::numeric_limits<float>::signaling_NaN()),
    Module(std::numeric_limits<float>::signaling_NaN()),
    Chip(std::numeric_limits<float>::signaling_NaN()),
    Set(std::numeric_limits<float>::signaling_NaN()),
    Strip(std::numeric_limits<float>::signaling_NaN()),
    Time(std::numeric_limits<float>::signaling_NaN()),
    ADC(std::numeric_limits<float>::signaling_NaN()),
    TrigNum(std::numeric_limits<float>::signaling_NaN()),
    Station(std::numeric_limits<int>::signaling_NaN()),
    Sensor(std::numeric_limits<int>::signaling_NaN()),
    Plane(std::numeric_limits<int>::signaling_NaN())

 {
  }

  //--------------------------------------------------------------------
  SRSSDHits::~SRSSDHits()
  {
  }

  //--------------------------------------------------------------------
  void SRSSDHits::setDefault()
  {
    FER = -5;
    Module = -5;
    Chip = -5;
    Set = -5;
    Strip = -5;
    Time = -5;
    ADC = -5;
    TrigNum = -5;
    Row = -5;
    Station = -5;
    Sensor = -5;
    Plane = -5;
  }

} // end namespace
