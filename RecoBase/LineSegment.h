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
#include "StandardRecord/SRVector3D.h"

namespace rb {
  
  class LineSegment : public caf::SRLineSegment{
  public:
    LineSegment(); // Default constructor
    LineSegment(double _x0[3], double _x1[3]);
    virtual ~LineSegment() {}; //Destructor
    
    // Getters
    caf::SRVector3D X0() const { return x0; }
    caf::SRVector3D X1() const { return x1; }
    double Sigma() const { return sigma;}
    int SSDStation() const { return fSSDStation; }
    int SSDPlane() const { return fSSDPlane; }
    int SSDSensor() const { return fSSDSensor; }
    double SSDStrip() const {return fSSDStrip; }

    // Setters
    void SetX0(double* _x0) { x0.SetX(_x0[0]); x0.SetY(_x0[1]); x0.SetZ(_x0[2]); }
    void SetX1(double* _x1) { x1.SetX(_x1[0]); x1.SetY(_x1[1]); x1.SetZ(_x1[2]); }
    void SetSigma(double s) { sigma = s; }
    void SetSSDStation(int s) { fSSDStation = s; }
    void SetSSDPlane(int p) { fSSDPlane = p; }
    void SetSSDSensor(int s) { fSSDSensor = s; }
    void SetSSDStrip(double r)  { fSSDStrip = r;}

    double DistanceToPoint(double x, double y, double z) const;
    double DistanceToPoint(double x, double y) const;

    friend std::ostream& operator << (std::ostream& o, const LineSegment& h);

  private:

    int fSSDStation;
    int fSSDPlane;
    int fSSDSensor;
    int fSSDStrip;  
   };
  
}

#endif // LINESEGMENT_H
