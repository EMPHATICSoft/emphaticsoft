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
  
  TrackSegment::TrackSegment() : caf::SRTrackSegment()
  {
    _clust.clear();
    _spcpt.clear();
  }
  
  //------------------------------------------------------------

  const rb::SSDCluster* TrackSegment::GetSSDCluster(size_t i) const
  {    
    assert(i < _clust.size());

    return &_clust[i];
  }
  
  
  //------------------------------------------------------------
  
  const rb::SpacePoint* TrackSegment::GetSpacePoint(size_t i) const
  {
    assert(i < _spcpt.size());

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
    o << " Track Segment --> x0(" << h.vtx << "), p(" << h.mom << ")"; 
    for (auto spcpt : h._spcpt) {
      o << "\n   spcpt " << spcpt;
    } 
    for (auto clust : h._clust) {
      o << "\n   clust " << clust;
    }
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
