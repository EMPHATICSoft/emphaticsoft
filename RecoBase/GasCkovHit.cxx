////////////////////////////////////////////////////////////////////////
/// \brief   GasCkovHit class
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/GasCkovHit.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  GasCkovHit::GasCkovHit()    
  {
    for (int i=0; i<3; ++i) { _charge[i] = -9999.; }
    for (int i=0; i<3; ++i) { _time[i] = -9999.; }
    for (int i=0; i<5; ++i) { _PID[i] = 0; }
  }
  
  } // end namespace rb
//////////////////////////////////////////////////////////////////////////////
