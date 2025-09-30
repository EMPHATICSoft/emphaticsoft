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

  //----------------------------------------------------------------------

  const rb::LineSegment* SpacePoint::GetLineSegment(int i) const
  {
    assert((i >= 0) && (i < int(_lineseg.size())));
    return &_lineseg[i];
  }

  //----------------------------------------------------------------------
  
  const rb::SSDCluster* SpacePoint::GetSSDCluster(int i) const
  {
    assert((i >= 0) && (i < int(_clust.size())));
    return &_clust[i];
  }

  //----------------------------------------------------------------------

  void SpacePoint::Add(const rb::LineSegment& ls)
  {
    // Enforce design: the assertion would mean only one LineSegment per SpacePoint
    //assert(_lineseg.empty());
    _lineseg.push_back(ls);
  }

  //----------------------------------------------------------------------

  void SpacePoint::Add(const rb::SSDCluster& cl)
  {
    // Enforce design: the assertion would mean only one SSDCluster per SpacePoint
    //assert(_clust.empty());
    _clust.push_back(cl);
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
