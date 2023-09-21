////////////////////////////////////////////////////////////////////////
/// \brief   Definition of 3D space point class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SPACEPOINT_H
#define SPACEPOINT_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace rb {
  
  class SpacePoint {
  public:
    SpacePoint(); // Default constructor
    //    SpacePoint(std::vector<rb::SSDHit> hits); // Default constructor
    virtual ~SpacePoint() {}; //Destructor
    
  private:

    double _x[3]; // rotation angle about the vertical y-axis
    int    fStation; //SSD station of the Space Point

  public:
    // Getters
    const double* Pos() const { return _x; }
    int           Station() const { return fStation; }

    // Setters
    void SetX(double* x0) { for (int i=0; i<3; ++i) _x[i] = x0[i]; }
    void SetStation(int station) {fStation = station;}   
 
    friend std::ostream& operator << (std::ostream& o, const SpacePoint& h);
  };
  
}

#endif // SPACEPOINT_H
