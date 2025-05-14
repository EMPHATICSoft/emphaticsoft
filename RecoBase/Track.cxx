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
  
  Track::Track() 
  {
    _pos.clear();
    _clust.clear();
    _sgmnt.clear();
    _spcpt.clear();

    for (int i=0; i<3; ++i) {
      _vtx[i] = -999999.;
      _p[i] = 0.;
    }
  }
  
  //------------------------------------------------------------

  void Track::Add(const rb::SSDCluster& cl) 
  {
    assert(_sgmnt.empty() && _spcpt.empty());
    _clust.push_back(rb::SSDCluster(cl));
  }
  
  //------------------------------------------------------------

  void Track::Add(const rb::TrackSegment& ts)
  {
    assert(_clust.empty() && _spcpt.empty());
    _sgmnt.push_back(rb::TrackSegment(ts));
  }

  //------------------------------------------------------------

  void Track::Add(const rb::SpacePoint& sp)
  {
    assert(_clust.empty() && _sgmnt.empty());
    _spcpt.push_back(rb::SpacePoint(sp));
  }

  //------------------------------------------------------------

  void Track::AddPos(TVector3 &x)
  {
    _pos.push_back(x);
  }

  //------------------------------------------------------------

  const rb::SSDCluster* Track::GetSSDCluster(int i) const
  {    
    assert(((i>0) && (i < int(_clust.size()))));
    
    return &_clust[i];
  }
  
  //------------------------------------------------------------

  const rb::TrackSegment* Track::GetTrackSegment(int i) const
  {
    assert(((i>0) && (i < int(_sgmnt.size()))));
    
    return &_sgmnt[i];
  }
  
  //------------------------------------------------------------

  const rb::SpacePoint* Track::GetSpacePoint(int i) const
  {
    assert(((i>0) && (i < int(_spcpt.size()))));
    
    return &_spcpt[i];
  }
  
  //------------------------------------------------------------
  
  TVector3 Track::PosAt(double z) const
  {
    TVector3 pos;
    double x = -9999.;
    double y = -9999.;

    assert(_pos.size()>= 2);

    if (z >= _vtx[2]) {
      size_t i=0; 
      for (; i<_pos.size()-1; ++i) {
	if ((z>_pos[i].Z()) && (z<_pos[i+1].Z())) {
	  double dz = _pos[i+1].Z() - _pos[i].Z();
	  double dx = _pos[i+1].X() - _pos[i].X();
	  double dy = _pos[i+1].Y() - _pos[i].Y();
	  x = _pos[i].X() + z*(dx/dz);
	  y = _pos[i].Y() + z*(dy/dz);	  
	  break;
	}
      }
      assert(i <= _pos.size());
    }

    pos.SetX(x);
    pos.SetY(y);
    pos.SetZ(z);
    
    return pos;
  }

  //------------------------------------------------------------

  TVector3 Track::MomAt(double z) const
  {
    TVector3 mom;
    double px = -9999.;
    double py = -9999.;

    assert(_mom.size()>= 2);

    if (z >= _vtx[2]) {
      size_t i=0; 
      for (; i<_pos.size()-1; ++i) {
	if ((z>_pos[i].Z()) && (z<_pos[i+1].Z())) {
 	  double dz = _pos[i+1].Z() - _pos[i].Z();
	  double dpx = _mom[i+1].X() - _mom[i].X();
	  double dpy = _mom[i+1].Y() - _mom[i].Y();
	  px = _mom[i].X() + z*(dpx/dz);
	  py = _mom[i].Y() + z*(dpy/dz);	  
	  break;
	}
      }
      assert(i <= _pos.size());
    }
    
    mom.SetX(px);
    mom.SetY(py);
    mom.SetZ(sqrt(_p[0]*_p[0] + _p[1]*_p[1] + _p[2]*_p[2] - px*px - py*py));
    
    return mom;
  }

  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const Track& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Track Segment --> x0(" << h._vtx[0] << "," << h._vtx[1] << "," 
      << h._vtx[2] << "), p(" << h._p[0] << "," << h._p[1] << "," 
      << h._p[2] << ")"; 
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
