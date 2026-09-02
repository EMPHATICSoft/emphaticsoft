////////////////////////////////////////////////////////////////////////
/// \brief   Track class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/Track.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  Track::Track() : caf::SRTrack()
  {
    lineseg.clear();
    sgmnt.clear();
    spcpt.clear();

  }
  //------------------------------------------------------------

  void Track::Add(const rb::LineSegment& ls) 
  {
//    assert(_sgmnt.empty() && _spcpt.empty());
    lineseg.push_back(rb::LineSegment(ls));
  }
  
  //------------------------------------------------------------

  void Track::Add(const rb::TrackSegment& ts)
  {
//    assert(_clust.empty() && _spcpt.empty());
    sgmnt.push_back(rb::TrackSegment(ts));
  }

  //------------------------------------------------------------

  void Track::Add(const rb::SpacePoint& sp)
  {
//    assert(_clust.empty() && _sgmnt.empty());
    spcpt.push_back(rb::SpacePoint(sp));
  }  

  //------------------------------------------------------------

  const rb::LineSegment* Track::GetSSDLineSegment(int i) const
  {    
    assert(((i>=0) && (i < int(lineseg.size()))));
    
    return &lineseg[i];
  }
  
  //------------------------------------------------------------

  const rb::TrackSegment* Track::GetTrackSegment(int i) const
  {
    assert(((i>=0) && (i < int(sgmnt.size()))));
    
    return &sgmnt[i];
  }
  
  //------------------------------------------------------------

  const rb::SpacePoint* Track::GetSpacePoint(int i) const
  {
    assert(((i>=0) && (i < int(spcpt.size()))));
    
    return &spcpt[i];
  }
  
  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const Track& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Track --> x0(" << h.vtx << "), p(" << h.mom << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
