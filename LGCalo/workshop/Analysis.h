////////////////////////////////////////////////////////////////////////
/// \brief   Definition of ARICH ring
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <vector>
#include <stdint.h>
#include <iostream>

//#include "RawData/RawDigit.h"

namespace rb {
  
  class Analysis {
  public:
    Analysis(); // Default constructor
    virtual ~Analysis() {}; //Destructor
    
  private:

    float _center[3]; // rotation angle about the vertical y-axis
    float _radius;
    int _nhits;
    int _ntrig;
    
  public:
    // Getters

    int    NTrig() const { return _ntrig; }
    float  XCenter() const { return _center[0]; }
    float  YCenter() const { return _center[1]; }
    float  ZCenter() const { return _center[2]; }
    const float* Center() const { return _center; }
    float  Radius() const { return _radius; }
    int    NHits() const { return _nhits; }

    // Setters
    void SetCenter(float x[3]) { for (int i=0; i<3; ++i) _center[i] = x[i]; }
    void SetRadius(float r) { _radius = r; }
    void SetNHits(int nh) { _nhits = nh; }
    
    //    friend std::ostream& operator << (std::ostream& o, const ARing& h);
  };
  
}

#endif // ANALYSIS_H
