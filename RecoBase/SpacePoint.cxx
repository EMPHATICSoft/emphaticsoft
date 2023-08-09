////////////////////////////////////////////////////////////////////////
/// \brief   Definition of 3D space point class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/SpacePoint.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  SpacePoint::SpacePoint() //:
  //fStation(-1)
  {
    for (int i=0; i<3; ++i) {
      _x[i] = -999999.;
    }
    fStation = -1; 
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const SpacePoint& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Spacepoint --> (" << h._x[0] << "," << h._x[1] << "," << h._x[2]
      << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
