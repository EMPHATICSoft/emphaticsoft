////////////////////////////////////////////////////////////////////////
/// \brief   Definition of line segment (eg, ssd strip)
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "StandardRecord/SRLineSegment.h"
//#include "StandardRecord/SRVector3D.h"
#include "Math/SVector.h"
#include "Math/SMatrix.h"
#include "TMath.h"

namespace rb {
  
  class LineSegment : public caf::SRLineSegment{
  public:
    LineSegment(); // Default constructor
    LineSegment(double _x0[3], double _x1[3]);
    virtual ~LineSegment() {}; //Destructor
    
    // Getters
    ROOT::Math::XYZVector X0() const { return x0; }
    ROOT::Math::XYZVector X1() const { return x1; }
    double Sigma() const { return sigma;}
    int SSDStation() const { return fSSDStation; }
    int SSDPlane() const { return fSSDPlane; }
    int SSDSensor() const { return fSSDSensor; }
    int SSDStrip() const { return fSSDStrip; }
    double U() const { return fU; }
    double V() const { return fV; }
    double Alpha() const { return fAlpha; }
    // Setters
    void SetX0(double* _x0) { x0.SetX(_x0[0]); x0.SetY(_x0[1]); x0.SetZ(_x0[2]); }
    void SetX1(double* _x1) { x1.SetX(_x1[0]); x1.SetY(_x1[1]); x1.SetZ(_x1[2]); }
    void SetSigma(double s) { sigma = s; }
    void SetSSDStation(int s) { fSSDStation = s; }
    void SetSSDPlane(int p) { fSSDPlane = p; }
    void SetSSDSensor(int s) { fSSDSensor = s; }
    void SetSSDStrip(int s) { fSSDStrip = s; }
    void SetSSDInfo(int station, int plane, int sensor, int strip) {
      fSSDStation = station; fSSDPlane = plane;
      fSSDSensor = sensor; fSSDStrip = strip;
    }
    void SetU(double u) { fU = u; }
    void SetV(double v) { fV = v; }
    void SetAlpha(double alpha) { fAlpha = alpha; }
    virtual double DistanceToPoint(double x, double y, double z) const;
    virtual double DistanceToPoint(double x, double y) const;

    friend std::ostream& operator << (std::ostream& o, const LineSegment& h);

  private:

    int fSSDStation;
    int fSSDPlane;
    int fSSDSensor;
    int fSSDStrip;
    double fU = 0; 		// x-component of line normal (cos(alpha))
    double fV = 0;          // y-component of line normal (sin(alpha))
    double fAlpha = 0;      // angle of line normal from x-axis
  };
  
}

#endif // LINESEGMENT_H
