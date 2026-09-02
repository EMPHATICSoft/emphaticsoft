#include "KalmanReco/KLSMeasurement.h"

namespace kalman {

  KLSMeasurement::KLSMeasurement(const rb::LineSegment& line) : rb::LineSegment(line){
    double dx = line.X1().X() - line.X0().X();
    double dy = line.X1().Y() - line.X0().Y();
    if (TMath::Abs(dx) < 1e-6) dx = 0.;
    if (TMath::Abs(dy) < 1e-6) dy = 0.;

//    if (dx<0) { //  && dy<0) {
//      dx = -dx;
//      dy = -dy;
//    }
    fAlpha = TMath::ATan(dy/dx) + TMath::Pi()/2.0; // angle of normal vector to line
    fU = TMath::Cos(fAlpha);
    fV = TMath::Sin(fAlpha);
    if (TMath::Abs(fU) < 1.e-6) {
      fU = 0.;
      fV = 1.;
    }
    if (TMath::Abs(fV) < 1.e-6) {
      fU = 1.;
      fV = 0.;
    }
  }

  double KLSMeasurement::DistanceToPoint(double x, double y) const {
    // Calculate the signed distance from a point (x,y) to the infinite line.
    // The normal vector (fU,fV) is stored in the measurement and defines the
    // sign convention used by the Kalman filter Jacobian.
    double d = fU * X0().X() + fV * X0().Y();
    if (this->SSDStation() == 3 && this->SSDPlane() == 2) {
      return fU * x + fV * y - d;
    }
    else
      return fU * x + fV * y - d;
  }

  //*******************************************************************

  std::ostream& operator << (std::ostream& o, const KLSMeasurement& m)
  {
    o << "KLSMeasurement: " << std::endl;
    o << static_cast<const rb::LineSegment&>(m) << std::endl;
    o << "u = " << m.fU << std::endl;
    o << "v = " << m.fV << std::endl;
    o << "alpha = " << m.fAlpha << std::endl;
    return o;
 }

}

