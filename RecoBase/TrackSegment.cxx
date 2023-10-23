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
      _vtx[i] = -999999.;
      _p[i] = 0.;
    }
  }
  
  //------------------------------------------------------------

  const rb::SSDCluster* TrackSegment::GetSSDCluster(int i) const
  {    
    assert(((i>0) && (i < int(_clust.size()))));
    
    return &_clust[i];
  }
  
  
  //------------------------------------------------------------
  
  const rb::SpacePoint* TrackSegment::GetSpacePoint(int i) const
  {
    assert(((i>0) && (i < int(_spcpt.size()))));
    
    return &_spcpt[i];
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const TrackSegment& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Track Segment --> x0(" << h._vtx[0] << "," << h._vtx[1] << "," 
      << h._vtx[2] << "), p(" << h._p[0] << "," << h._p[1] << "," 
      << h._p[2] << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
