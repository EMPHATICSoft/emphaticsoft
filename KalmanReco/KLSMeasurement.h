#ifndef KMEASUREMENT_H
#define KMEASUREMENT_H

#include <vector>
#include <iostream>
#include "Math/SVector.h"
#include "Math/SMatrix.h"
#include "TMath.h"
#include "RecoBase/LineSegment.h"

using namespace std;

typedef ROOT::Math::SVector<double,1> KMeas;
typedef ROOT::Math::SMatrix<double,1,1> KMeasCov;

namespace kalman {
  
  class KLSMeasurement : public rb::LineSegment {
  public:
    KLSMeasurement();
    KLSMeasurement(const rb::LineSegment& line);

    ~KLSMeasurement() {};

    double GetU() const { return fU; }
    double GetV() const { return fV; }
    double GetAlpha() const { return fAlpha; }
    double GetZ() const { return (X0().Z() + X1().Z()) / 2.0; }
    
    void SetU(double u_comp) { fU = u_comp; }
    void SetV(double v_comp) { fV = v_comp; }
    void SetAlpha(double angle) { fAlpha = angle; }

    virtual double DistanceToPoint(double x, double y) const;
    // Get closest point on line to (x,y)

    friend std::ostream& operator << (std::ostream& o, const KLSMeasurement& s);

  private:
    double fU;              // x-component of line normal (cos(alpha))
    double fV;              // y-component of line normal (sin(alpha))
    double fAlpha;          // angle of line normal from x-axis
  };
}  
  

#endif
