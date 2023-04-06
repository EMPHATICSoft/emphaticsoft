////////////////////////////////////////////////////////////////////////
/// \brief   Line segment class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/LineSegment.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  LineSegment::LineSegment() 
  {
    for (int i=0; i<3; ++i) {
      _x0[i] = -999999.;
      _x1[i] = 999999.;
    }
  }
    
  //----------------------------------------------------------------------
  
  LineSegment::LineSegment(double x0[3], double x1[3]) 
  {
    for (int i=0; i<3; ++i) {
      _x0[i] = x0[i];
      _x1[i] = x1[i];
    }
  }
    
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const LineSegment& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Line Segment --> x0(" << h._x0[0] << "," << h._x0[1] << "," 
      << h._x0[2] << "), x1(" << h._x1[0] << "," << h._x1[1] << "," 
      << h._x1[2] << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
