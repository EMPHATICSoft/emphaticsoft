////////////////////////////////////////////////////////////////////////
/// \brief   ARing class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/ARing.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  ARing::ARing()
  {
     for (int i=0; i<3; ++i) _center[i] = -9999.;
    _radius = -1.;
    _nhits = 0;
  }
  
  //------------------------------------------------------------
  /*
  std::ostream& operator<< (std::ostream& o, const ARing& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " SSD Strip = "     << std::setw(5) << std::right << h.Strip()
      << " Angle = "     << std::setw(5) << std::right << h.Angle()
      << " Pitch = "        << std::setw(5) << std::right << h.Pitch();     
    return o;
  }
  */
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
