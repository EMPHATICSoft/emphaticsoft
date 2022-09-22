////////////////////////////////////////////////////////////////////////
/// \brief   BACkovHit class
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/BACkovHit.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  BACkovHit::BACkovHit()    
  {
    for (int i=0; i<6; ++i) { _charge[i] = -9999.; }
    for (int i=0; i<5; ++i) { _PID[i] = 0; }
  }
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
