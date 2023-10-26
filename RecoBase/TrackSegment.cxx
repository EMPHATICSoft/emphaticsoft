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
  
  void TrackSegment::Add(const rb::SpacePoint& sp) 
  {
    assert(_clust.empty());
    _spcpt.push_back(sp);
  }
  
  //------------------------------------------------------------
  
  void TrackSegment::Add(const rb::SSDCluster& cl) 
  {
    assert(_spcpt.empty());
    _clust.push_back(cl);
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
