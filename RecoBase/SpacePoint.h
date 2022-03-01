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

#include "RecoBase/SSDHit.h"

namespace rb {
  
  class TrackSegment {
  public:
    TrackSegment(); // Default constructor
    TrackSegment(std::vector<rb::SSDHit> hits); // Default constructor
    virtual ~TrackSegment() {}; //Destructor
    
  private:

    double _x0[3]; // rotation angle about the vertical y-axis
    double _p[3];  // avg. strip position
    std::vector<rb::SSDHit> _hit; // vector of SSD hits

  public:
    // Getters
    double* X0() const { return _x0; }
    double* P() const { return _p; }
    rb::SSDHit* Hit(int i) const;

    // Setters
    void AddHit(rb::SSDHit hit) { _hit.push_back(hit); }
    void SetX0(double* x0) { for (int i=0; i<3; ++i) _x0[i] = x0[i]; }
    void SetP(double* p) { for (int i=0; i<3; ++i) _p[i] = p[i]; }
    
    friend std::ostream& operator << (std::ostream& o, const TrackSegment& h);
  };
  
}

#endif // SPACEPOINT_H
