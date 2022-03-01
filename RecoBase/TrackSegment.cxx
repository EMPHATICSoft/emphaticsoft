////////////////////////////////////////////////////////////////////////
/// \brief   Track segment class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/TrackSegment.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  TrackSegment::TrackSegment() 
  {
    for (int i=0; i<3; ++i) {
      _x0[i] = -999999.;
      _p[i] = 0.;
    }
  }
  
  //------------------------------------------------------------

  rb::SSDHit* TrackSegment:Hit(int i)
  {
    if ((i>0) && (i < int(_hit.size()))) 
      return _hit[i];
    else
      return 0;
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const TrackSegment& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Track Segment --> x0(" << _x0[0] << "," << _x0[1] << "," << _x0[2]
      << "), p(" << _p[0] << "," << _p[1] << "," << _p[2] << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
