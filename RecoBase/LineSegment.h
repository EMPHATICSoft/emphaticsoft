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

namespace rb {
  
  class LineSegment {
  public:
    LineSegment(); // Default constructor
    LineSegment(double x2[3], double x1[3]);
    virtual ~LineSegment() {}; //Destructor
    
  private:

    double _x0[3]; // closest to origin by defintion
    double _x1[3]; // farthest to origin

  public:
    // Getters
    const double* X0() const { return _x0; }
    const double* X1() const { return _x1; }

    // Setters
    void SetX0(double* x0) { for (int i=0; i<3; ++i) _x0[i] = x0[i]; }
    void SetX1(double* x1) { for (int i=0; i<3; ++i) _x1[i] = x1[i]; }
    
    friend std::ostream& operator << (std::ostream& o, const LineSegment& h);
  };
  
}

#endif // LINESEGMENT_H
