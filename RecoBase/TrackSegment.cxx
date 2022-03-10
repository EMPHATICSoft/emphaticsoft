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
  
  //----------------------------------------------------------------------
  
  TrackSegment::TrackSegment(const art::PtrVector<rb::SSDHit>& hits) 
  {
    for (int i=0; i<3; ++i) {
      _x0[i] = -999999.;
      _p[i] = 0.;
    }
    for(size_t j=0; j<hits.size(); ++j)
      _hit.push_back(hits[j]);
  }
  
  //------------------------------------------------------------

  art::Ptr<rb::SSDHit> TrackSegment::Hit(int i) const
  {
    assert(((i>0) && (i < int(_hit.size()))));

    return _hit[i];
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const TrackSegment& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Track Segment --> x0(" << h._x0[0] << "," << h._x0[1] << "," 
      << h._x0[2] << "), p(" << h._p[0] << "," << h._p[1] << "," 
      << h._p[2] << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
