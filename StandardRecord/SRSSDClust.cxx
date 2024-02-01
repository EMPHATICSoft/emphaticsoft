////////////////////////////////////////////////////////////////////////
// \file    SRSSDClust.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRSSDClust.h"

namespace caf
{
  SRSSDClust::SRSSDClust() :
  
    station(std::numeric_limits<int>::signaling_NaN()),
    sens(std::numeric_limits<int>::signaling_NaN()),
    plane(std::numeric_limits<int>::signaling_NaN()),
    avgstrip(std::numeric_limits<double>::signaling_NaN()),
    wgtavgstrip(std::numeric_limits<double>::signaling_NaN()),
    wgtrmsstrip(std::numeric_limits<double>::signaling_NaN()),
    minstrip(std::numeric_limits<int>::signaling_NaN()),
    maxstrip(std::numeric_limits<int>::signaling_NaN()),
    avgadc(std::numeric_limits<double>::signaling_NaN()),
    avgtime(std::numeric_limits<double>::signaling_NaN()),
    mintime(std::numeric_limits<int>::signaling_NaN()),
    maxtime(std::numeric_limits<int>::signaling_NaN()),
    timerange(std::numeric_limits<int>::signaling_NaN()),
    width(std::numeric_limits<int>::signaling_NaN()),
    ndigits(std::numeric_limits<unsigned int>::signaling_NaN())
 { 
 }
  //--------------------------------------------------------------------
  SRSSDClust::~SRSSDClust()
  {
  }

  //--------------------------------------------------------------------
  void SRSSDClust::setDefault()
  {
    station = -5;
    sens = -5;
    avgstrip = -5;
    wgtavgstrip = -5;
    wgtrmsstrip = -5;
    minstrip = -5;
    maxstrip = -5;
    avgadc = -5;
    avgtime = -5;
    mintime = -5;
    maxtime = -5;
    ndigits = -5;
    timerange = -5;
    width = -5;
  }

} // end namespace
