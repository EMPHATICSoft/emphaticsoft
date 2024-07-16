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
    
  //private:

    //double _x0[3]; // closest to origin by defintion
    //double _x1[3]; // farthest to origin

  //public:
    // Getters
    //const double* X0() const { return _x0; }
    //const double* X1() const { return _x1; }
    caf::SRVector3D X0() const { return x0; }
    caf::SRVector3D X1() const { return x1; }
    //const double* X0() const { double x[3] = {x0.X(),x0.Y(),x0.Z()}; return x; }
    //const double* X1() const { double x[3] = {x1.X(),x1.Y(),x1.Z()}; return x; }

    // Setters
    //void SetX0(double* x0) { for (int i=0; i<3; ++i) _x0[i] = x0[i]; }
    //void SetX1(double* x1) { for (int i=0; i<3; ++i) _x1[i] = x1[i]; }
    void SetX0(double* _x0) { x0.SetX(_x0[0]); x0.SetY(_x0[1]); x0.SetZ(_x0[2]); }
    void SetX1(double* _x1) { x1.SetX(_x1[0]); x1.SetY(_x1[1]); x1.SetZ(_x1[2]); }

    friend std::ostream& operator << (std::ostream& o, const LineSegment& h);
  };
  
}

#endif // LINESEGMENT_H
