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
    _pos.clear();
    _lineseg.clear();
    _sgmnt.clear();
    _spcpt.clear();

    /*
    for (int i=0; i<3; ++i) {
      _vtx[i] = -999999.;
      _p[i] = 0.;
    }
    */

  }
  //------------------------------------------------------------

  void Track::Add(const rb::LineSegment& ls) 
  {
//    assert(_sgmnt.empty() && _spcpt.empty());
    _lineseg.push_back(rb::LineSegment(ls));
  }
  
  //------------------------------------------------------------

  void Track::Add(const rb::TrackSegment& ts)
  {
//    assert(_clust.empty() && _spcpt.empty());
    _sgmnt.push_back(rb::TrackSegment(ts));
  }

  //------------------------------------------------------------

  void Track::Add(const rb::SpacePoint& sp)
  {
//    assert(_clust.empty() && _sgmnt.empty());
    _spcpt.push_back(rb::SpacePoint(sp));
  }
  

  //------------------------------------------------------------

  void Track::AddPos(ROOT::Math::XYZVector &x)
  {
    _pos.push_back(x);
  }

  //------------------------------------------------------------

  const rb::LineSegment* Track::GetSSDLineSegment(int i) const
  {    
    assert(((i>=0) && (i < int(_lineseg.size()))));
    
    return &_lineseg[i];
  }
  
  //------------------------------------------------------------

  const rb::TrackSegment* Track::GetTrackSegment(int i) const
  {
    assert(((i>=0) && (i < int(_sgmnt.size()))));
    
    return &_sgmnt[i];
  }
  
  //------------------------------------------------------------

  const rb::SpacePoint* Track::GetSpacePoint(int i) const
  {
    assert(((i>=0) && (i < int(_spcpt.size()))));
    
    return &_spcpt[i];
  }
  
  //------------------------------------------------------------
  
  ROOT::Math::XYZVector Track::PosAt(double z) const
  {
    ROOT::Math::XYZVector pos;
    double x = -9999.;
    double y = -9999.;

    assert(_pos.size()>= 2);

    if (z >= vtx.Z()) {
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

  ROOT::Math::XYZVector Track::MomAt(double z) const
  {
    ROOT::Math::XYZVector tmom;
    double px = -9999.;
    double py = -9999.;

    assert(_mom.size()>= 2);

    if (z >= vtx.Z()) {
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
    
    tmom.SetX(px);
    tmom.SetY(py);
    tmom.SetZ(sqrt(mom.Mag2() - px*px - py*py));
    
    return tmom;
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
