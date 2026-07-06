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

    double dx = x1.X() - x0.X();
    double dy = x1.Y() - x0.Y();
    if (TMath::Abs(dx) < 1e-6) dx = 0.;
    if (TMath::Abs(dy) < 1e-6) dy = 0.;

    fAlpha = TMath::ATan(dy/dx) + TMath::Pi()/2.0; // angle of normal vector to line
    fU = TMath::Cos(fAlpha);
    fV = TMath::Sin(fAlpha);
    if (TMath::Abs(fU) < 1e-6) {
      fU = 0.;
      fV = 1.;
    }
    if (TMath::Abs(fV) < 1e-6) {
      fU = 1.;
      fV = 0.;
    }
  }

  //----------------------------------------------------------------------

  double LineSegment::DistanceToPoint(double x, double y, double z) const
  {
    double dx = x1.X() - x0.X();
    double dy = x1.Y() - x0.Y();
    double dz = x1.Z() - x0.Z();
    
    ROOT::Math::XYZVector v(dx,dy,dz);
    auto vu = v.Unit();

    ROOT::Math::XYZVector P(x,y,z);
    auto Px0 = x0 - P;

    return sqrt(Px0.Cross(vu).Mag2());

  }

  //----------------------------------------------------------------------

  double LineSegment::DistanceToPoint(double x, double y) const
  {
    if (fU != 0 || fV != 0 || fAlpha != 0) {
      double d = fU * x0.X() + fV * x0.Y();
      return fU * x + fV * y - d;
    }

    double dx = x1.X() - x0.X();
    double dy = x1.Y() - x0.Y();

    return (dy*x - dx*y + x1.X()*x0.Y() - x1.Y()*x0.X())/sqrt(dy*dy+dx*dx);
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const LineSegment& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Line Segment --> x0(" << h.x0.X() << "," << h.x0.Y() << ","
      << h.x0.Z() << "), x1(" << h.x1.X() << "," << h.x1.Y() << ","
      << h.x1.Z() << ") +- " << h.Sigma() << " from SSD(" << h.SSDStation() << "," << h.SSDPlane() << "," << h.SSDSensor() << ") "; 
    return o;
  }
 
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
