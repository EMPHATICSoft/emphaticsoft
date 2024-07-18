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
  }
    
  //----------------------------------------------------------------------
  
  LineSegment::LineSegment(double _x0[3], double _x1[3]) : caf::SRLineSegment()  {
    x0.SetXYZ(_x0[0],_x0[1],_x0[2]);
    x1.SetXYZ(_x1[0],_x1[1],_x1[2]);
    //x0.SetX(_x0[0]);
    //x0.SetY(_x0[1]);
    //x0.SetZ(_x0[2]);
    //x1.SetX(_x1[0]);
    //x1.SetY(_x1[1]);
    //x1.SetZ(_x1[2]);
  }
    
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const LineSegment& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Line Segment --> x0(" << h.x0.X() << "," << h.x0.Y() << ","
      << h.x0.Z() << "), x1(" << h.x1.X() << "," << h.x1.Y() << ","
      << h.x1.Z() << ")";
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
