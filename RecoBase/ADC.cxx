////////////////////////////////////////////////////////////////////////
/// \brief   ADC class for functions involving waveforms from V1720s
/// \author  jmirabit@bu.ed
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/ADC.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace rb{

  //----------------------------------------------------------------------
  
  ADC::ADC()
  {
    _baseline = -999;
    _time = -999;
    _charge = -999;
  }
  
} //end namespace rb  
//////////////////////////////////////////////////////////////////////////////
